#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define BUF_SIZE 512

typedef struct{
	long type;
	char text[BUF_SIZE];
	char path[BUF_SIZE];
} msgData;

void Reader(int dsMsg, char *file){
	FILE *fd;
	msgData msg;
	char tmp[BUF_SIZE];
	if((fd = fopen(file, "r")) == NULL){
		perror("fopen");
		exit(1);
	}
	msg.type=1;
	strncpy(msg.path, file, BUF_SIZE);
	while((fgets(tmp, BUF_SIZE, fd)) != NULL){
		strncpy(msg.text, tmp, BUF_SIZE);
		//printf("msg.text: %s\n", msg.text);
		if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
			perror("msgsnd");
			exit(1);
		}
	}
	fclose(fd);
	msg.type = 99; //synchro type
	if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
		perror("msgsnd");
		exit(1);
	}
	//printf("Reader - exit\n");
	exit(0);
}

void Filterer(int dsMsg, int *pipeFd, int nFile, int s, int v, int i, char *word){
	//printf("Filterer - s: %d, v: %d, i: %d, word: %s\n", s, v, i, word);
	msgData msg;
	char tmp[BUF_SIZE];
	char text[BUF_SIZE];
	//printf("nFile: %d\n", nFile);
	for(int j=0; j<nFile; j++){
		int end = 0;
		while(!end){
		//	printf("Filterer - before msgrcv\n");
			if((msgrcv(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0, 0)) == -1){
				perror("msgrcv");
				exit(1);
			}
			if(msg.type == 99){
				end = 1;
				//printf("Filterer 99\n");
			}
			if(!end){
				strncpy(tmp, msg.text, BUF_SIZE);
				int isPresent = 0; //to know if the string must be sent to father

				//filter-zone
				if(s == 1){
					//printf("s\n");
					if((strstr(tmp, word)) != NULL){
						strncpy(text, msg.path, BUF_SIZE);
						strncat(text, ":", BUF_SIZE);
						strncat(text, tmp, BUF_SIZE);
						isPresent = 1;
					}
				}
				else if(v == 1 && i == 0){
					//printf("v\n");
					if((strstr(tmp, word)) == NULL){
						strncpy(text, msg.path, BUF_SIZE);
						strncat(text, ":", BUF_SIZE);
						strncat(text, tmp, BUF_SIZE);
						isPresent = 1;
					}
				}
				else if(v == 0 && i == 1){
					//printf("i\n");
					if((strcasestr(tmp, word)) != NULL){
						strncpy(text, msg.path, BUF_SIZE);
						strncat(text, ":", BUF_SIZE);
						strncat(text, tmp, BUF_SIZE);
						isPresent = 1;
					}
				}
				else if(v == 1 && i == 1){
					//printf("v e i\n");
					if((strcasestr(tmp, word)) == NULL){
						strncpy(text, msg.path, BUF_SIZE);
						strncat(text, ":", BUF_SIZE);
						strncat(text, tmp, BUF_SIZE);
						isPresent = 1;
					}
				}
				//printf("text: %s\n", text);
				//use of pipe only in writing
				if(isPresent){
					close(pipeFd[0]);
					if((write(pipeFd[1], text, BUF_SIZE)) == -1){
						perror("write");
						exit(1);
					}
				}
			}
		}
	}
	strncpy(text, "99", BUF_SIZE); //synchro msg
	//printf("Filterer - synchro\n");
	if((write(pipeFd[1], text, BUF_SIZE)) == -1){
		perror("write");
		exit(1);
	}

	//printf("Filterer - exit\n");
}

int main(int argc, char **argv){

	int dsMsg;
	int pipeFd[2];

	if(argc<3){
		printf("Usage [-v] [-i] <word> <file> [<file> ...]: %s\n", argv[0]);
		exit(0);
	}
	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("msgget");
		exit(1);
	}
	if((pipe(pipeFd)) == -1){
		perror("pipe");
		exit(1);
	}

	int s = 0, v = 0, i = 0; //mode of cmd myfgrep
	char word[BUF_SIZE];
	int j = 1; //indicate the position of the word
	while(j<argc){
		if((strcmp(argv[j], "-v")) == 0) v = 1;
		else if((strcmp(argv[j], "-i")) == 0) i = 1;
		else{
			strncpy(word, argv[j], BUF_SIZE);
			break;
		}
		j++;
	}
	if(v == 0 && i == 0) s = 1;
	//printf("s: %d, v: %d, i: %d, word: %s\n", s, v, i, word);
	pid_t pidFather = getpid();

	for(int k=j+1; k<argc && getpid() == pidFather; k++){ //son_2: reader
		if((fork()) == 0){
		//	printf("I'm Reader\n");
			//printf("k: %d, argc: %d\n", k, argc);
			Reader(dsMsg, argv[k]);
		}
	}
	if((getpid() == pidFather)){
		if((fork()) == 0){
		//	printf("I'm Filterer\n");
			Filterer(dsMsg, pipeFd, (argc-j-1), s, v, i, word);
		}
		else{
			sleep(1);
		//	printf("I'm father\n");

			close(pipeFd[1]);
			char text[BUF_SIZE];
			while(1){
				//printf("Fahter - read\n");
				if((read(pipeFd[0], text, BUF_SIZE)) == -1){
					perror("read");
					exit(1);
				}
				if((strncmp(text, "99", BUF_SIZE)) == 0) break;
				printf("%s\n", text);
			}
			if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
				perror("msgctl");
				exit(1);
			}

		//	printf("Fahter - exit\n");
		}
	}
}
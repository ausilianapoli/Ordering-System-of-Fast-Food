#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUF_SIZE 4096
#define TYPE_A 1 //for messages between Scanner and Analyzer
#define TYPE_B 2 //for messages between Analyzer and Father

typedef struct{
	long type;
	char buf[BUF_SIZE];
} msg;

void Analyzer(int dsMsg){

	msg message;
	char path[BUF_SIZE]; //to store the path read from the messages' queue
	int fd; //file descriptor for the file
	char *map; //address of file's map
	int count=0;

	while(1){
		memset(message.buf, '\0', BUF_SIZE);
		//printf("Analyzer, message.buf%s\n", message.buf);
		if((msgrcv(dsMsg, &message, BUF_SIZE, TYPE_A, 0)) == -1){
			perror("msgrcv");
			exit(1);
		}

		if((strcmp(message.buf, "end")) == 0){ 
			printf("Analyzer, end received\n");
			break; //synchro-msg from the Scanner
		}

		strncpy(path, message.buf, BUF_SIZE);
		//path[strlen(path)]='\0';

		//printf("Analyzer, path %s\n", path);

		if((fd = open(path, O_RDONLY)) == -1){
			perror("open");
			exit(1);
		}

		if((map = mmap(NULL, BUF_SIZE, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
			perror("mmap");
			exit(1);
		}
		close(fd);

		int i=0;
		while(map[i] != '\0'){
			if((map[i] >= 65 && map[i] >= 90) || (map[i] >= 97 && map[i] <= 122)){
				//printf("%c  ", map[i]);
				count++;
			}
			i++;
		}

		printf("Analyzer: percorso/nome-del-file <totale conteggiato>: %s  %d\n", path, count);

		if((munmap(map, BUF_SIZE)) == -1){
			perror("munmap");
			exit(1);
		}

		message.type=TYPE_B;
		sprintf(message.buf, "%d", count);
		//printf("message.buf:%s\n", message.buf);

		//write the message for the father
		if((msgsnd(dsMsg, &message, strlen(message.buf)+1, IPC_NOWAIT)) == -1){
			perror("msgsend");
			exit(1);
		}

		//write a synchro-msg for Scanner
		message.type=TYPE_A;
		strcpy(message.buf, "end");
		if((msgsnd(dsMsg, &message, strlen(message.buf)+1, 0)) == -1){
			perror("msgsend");
			exit(1);
		}
	}

	//synchro-msg for the father that print the total
	message.type=TYPE_B;
	strcpy(message.buf, "end");
	message.buf[strlen(message.buf)]='\0';
	if((msgsnd(dsMsg, &message, strlen(message.buf)+1, 0)) == -1){
		perror("msgsend");
		exit(1);
	}
	//printf("Analyzer, msg sent: %s\n", message.buf);

	exit(0);
}

void Scanner(int dsMsg, char *directory, char *signDir){
	DIR *current;
	struct dirent *dirCurrent;
	struct stat info;
	char path[BUF_SIZE]; //to build recursively the path
	msg message;
	message.type=TYPE_A;

	if((current = opendir(directory)) == NULL){
		perror("opendir");
		exit(1);
	}
	while((dirCurrent = readdir(current)) != NULL){
		//printf("directory:%s\n", directory);
		if((chdir(directory)) == -1){
			perror("chdir");
			exit(1);
		}
		if((lstat(dirCurrent->d_name, &info)) == -1){
			perror("lstat");
			exit(1);
		}
		strcpy(path, directory);
		path[strlen(path)]='\0';
		strcat(path, "/");
		strcat(path, dirCurrent->d_name);
		//printf("path:%s\n", path);
		if(S_ISREG(info.st_mode)){
			//printf("There is a regular file. I send the path to the Analyzer\n");
			//printf("Scanner:%s\n", path);
			memset(message.buf, '\0', BUF_SIZE);
			//for(int i=0; i<BUF_SIZE; i++) message.buf[i]='\0';
			//printf("Scanner, message.buf %s\n", message.buf);
			//int len=strlen(path);
			strcpy(message.buf, path);
			//message.buf[len]='\0';
			//printf("Scanner, message.buf %s\n", message.buf);
			//printf("Message:%s\n", message.buf);
			if((msgsnd(dsMsg, &message, strlen(message.buf)+1, IPC_NOWAIT)) == -1){
				perror("msgsend");
				exit(1);
			}
			
			memset(message.buf, '\0', BUF_SIZE);
			//synchro-msg with the Scanner that has finished own work
			if((msgrcv(dsMsg, &message, BUF_SIZE, TYPE_A, 0)) == -1){
				perror("msgrcv");
				exit(1);
			}
		}
		else if(S_ISDIR(info.st_mode)){
			if((strcmp(dirCurrent->d_name, ".")) == 0 || (strcmp(dirCurrent->d_name, "..") == 0)) continue;
			//printf("It's a dir\n");
			Scanner(dsMsg, path, signDir);
		}
	}
	if((chdir("..")) == -1){
		perror("chdir");
		exit(1);
	}

	closedir(current);

	if((strcmp(directory, signDir)) == 0){
		//printf("Scanner, synchro\n");
	//synchro-msg with the Scanner to signal the end of files
		strcpy(message.buf, "end");
		message.buf[strlen(message.buf)]='\0';
		if((msgsnd(dsMsg, &message, strlen(message.buf)+1, IPC_NOWAIT)) == -1){
			perror("msgsend");
			exit(1);
		}
		exit(0);
	}
}

int main(int argc, char **argv){

	char *directory; //the pathname of dir to analyze
	int dsMsg; //descriptor of messages' queue
	pid_t pidFather=getpid();
	msg message;
	int tot=0;

	/*check if the dir is been taken*/
	if(argc<2){
		if((directory=getcwd(directory, BUF_SIZE)) == NULL){
			perror("getcwd");
			exit(1);
		}
	}
	else directory=strdup(argv[1]);

	//char *copyStartDir=strdup(directory);

	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1){
		perror("msgget");
		exit(1);
	}

	if(fork()==0){
		//printf("I'm the Scanner\n");
		Scanner(dsMsg, directory, directory);
	}
	if(getpid() == pidFather){
		if(fork()==0){
			Analyzer(dsMsg);
		}
		else{
			//printf("Father\n");
			while(1){
				memset(message.buf, '\0', BUF_SIZE);
				if((msgrcv(dsMsg, &message, BUF_SIZE, TYPE_B, 0)) == -1){
					perror("msgrcv");
					exit(1);
				}
				//printf("Father, %s\n", message.buf);
				if((strncmp(message.buf, "end", 3)) != 0) tot+=atoi(message.buf);
				else{
					printf("Padre: totale di %d caratteri alfabetici\n", tot);
					if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
						perror("msgctl");
						exit(1);	
					}
					break;
				}
			}
		}
		exit(0);
	}
}
	

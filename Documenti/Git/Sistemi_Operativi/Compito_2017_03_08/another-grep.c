#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LINE_SIZE 4096

typedef struct{
	long type;
	char line[LINE_SIZE];
} msgQueue;

void processR(char *file, int pipeFd[]){
	FILE *fd;
	if((fd=fopen(file, "r")) == NULL){
		perror("fopen");
		exit(1);
	}

	close(pipeFd[0]);
	char buf[LINE_SIZE];
	while(1){
		if((fgets(buf, LINE_SIZE, fd)) == NULL) break;
		if((write(pipeFd[1], buf, LINE_SIZE)) == -1){
			perror("write");
			exit(1);
		}
		sleep(1);
	}
	fclose(fd);
	//printf("R - end\n");
	//synchro-msg
	strncpy(buf, ".exit", LINE_SIZE);
	if((write(pipeFd[1], buf, LINE_SIZE)) == -1){
		perror("write_synchro");
		exit(1);
	}
}

void processP(char *word, int pipeFd[], int dsMsg){

	char buf[LINE_SIZE];
	msgQueue msg;
	msg.type=1; //valid msg
	close(pipeFd[1]);
	while(1){
		if((read(pipeFd[0], buf, LINE_SIZE)) == -1){
			perror("read");
			exit(1);
		}
		if((strncmp(buf, ".exit", LINE_SIZE)) == 0) break;
		//printf("buf: %s\n", buf);
		if((strstr(buf, word)) != NULL){
			//printf("Send msg with line: %s\n", buf);
			strncpy(msg.line, buf, LINE_SIZE);
			if((msgsnd(dsMsg, &msg, strlen(msg.line), 0)) == -1){
				perror("msgsnd");
				exit(1);
			}
		}
	}

	msg.type=2; //synchro-msg
	strncpy(msg.line, ".exit", LINE_SIZE);
	if((msgsnd(dsMsg, &msg, strlen(msg.line), 0)) == -1){
		perror("msgsnd");
		exit(1);
	}
}

void processW(int dsMsg){
	msgQueue msg;
	while(1){
		if((msgrcv(dsMsg, &msg, LINE_SIZE, 0, 0)) == -1){
			perror("msgrcv");
			exit(1);
		}
		if(msg.type == 2){ 
			break;
		}
		else printf("%s\n", msg.line);
	}
}

int main(int argc, char **argv){

	int pipeFd[2];
	int dsMsg;

	if(argc<3){
		printf("Usage <word> <pathname file>: %s\n", argv[0]);
		exit(0);
	}

	if((pipe(pipeFd)) == -1){
		perror("pipe");
		exit(1);
	}
	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("msgget");
		exit(1);
	}

	if(fork() != 0){ //father
		if(fork() == 0){ // son_2: R
			printf("I'm R\n");
			processR(argv[2], pipeFd);
		}
		else{// father
			printf("I'm P\n");
			processP(argv[1], pipeFd, dsMsg);

			if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
				perror("msgctl");
				exit(1);
			}
		}
	}
	else{//son_1: W
		printf("I'm W\n");
		processW(dsMsg);
	}

}
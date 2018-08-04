#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MAX_WORD_LEN 1024
#define TYPE_REQUEST 1
#define TYPE_ANSWER 2
#define SYNCHRO_MSG -999

typedef struct{
	long type; //1: question, 2: answer
	char text1[MAX_WORD_LEN];
	char text2[MAX_WORD_LEN];
	int result;
} msgData;

void Sorter(int dsMsg, int *pipeFd, char *file){
	msgData msg;
	FILE *fd;
	if((fd = fopen(file, "r")) == NULL){
		perror("fopen");
		exit(1);
	}
	int nLines = 0;
	char *tmp = (char*)malloc(sizeof(char));
	while((fgets(tmp, MAX_WORD_LEN, fd)) != NULL) nLines++;
	rewind(fd);
	char *buffer[MAX_WORD_LEN];
	int i=0;
	while((fgets(tmp, MAX_WORD_LEN, fd)) != NULL){
		buffer[i] = strdup(tmp);
		//printf("buffer[i]:%s", buffer[i]);
		i++;
	}
	fclose(fd);
	for(int j=0; j<nLines-1; j++){
		for(int k=j+1; k<nLines; k++){
			//pack the msg
			msg.type = TYPE_REQUEST;
			strncpy(msg.text1, buffer[j], MAX_WORD_LEN);
			strncpy(msg.text2, buffer[k], MAX_WORD_LEN);
			if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
				perror("msgsnd_SORTER");
				exit(1);
			}
			//read the answer
			if((msgrcv(dsMsg, &msg, sizeof(msgData) - sizeof(long), TYPE_ANSWER, 0)) == -1){
				perror("msgrcv_SORTER");
				exit(1);
			}
			if(msg.result > 0){
				tmp = buffer[j];
				buffer[j] = buffer[k];
				buffer[k] = tmp;
			}
		}
	}

	//synchro-msg for Comparer
	msg.type = TYPE_REQUEST;
	msg.result = SYNCHRO_MSG;
	if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
		perror("msgsnd_SORTER");
		exit(1);
	}

	//pipe for the Father
	close(pipeFd[0]);

	for(int j=0; j<nLines; j++){	
		if((write(pipeFd[1], buffer[j], MAX_WORD_LEN)) == -1){
			perror("write");
			exit(1);
		}
		//sleep(1);
	}
	snprintf(tmp, MAX_WORD_LEN, "%d", SYNCHRO_MSG);
	if((write(pipeFd[1], tmp, MAX_WORD_LEN)) == -1){
		perror("write");
		exit(1);
	}
	free(tmp);
}

void Comparer(int dsMsg){
	msgData msg;
	int result;
	while(1){
		if((msgrcv(dsMsg, &msg, sizeof(msgData) - sizeof(long), TYPE_REQUEST, 0)) == -1){
			perror("msgrcv_COMPARER");
			exit(1);
		}
		if(msg.result == SYNCHRO_MSG) break;
		result = strncasecmp(msg.text1, msg.text2, MAX_WORD_LEN);
		msg.type = TYPE_ANSWER;
		msg.result = result;
		if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
			perror("msgsnd_COMPARER");
			exit(1);
		}
	}

	printf("Comparer - exit\n");
}

int main(int argc, char **argv){
	int dsMsg;
	int pipeFd[2];

	if(argc != 2){
		printf("Usage <file name>: %s\n", argv[0]);
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

	pid_t pidComparer;

	if((fork() != 0)){
		if((fork() == 0)){ //son2:Comparer
			printf("I'm Comparer\n");
			pidComparer = getpid();
			Comparer(dsMsg);
		}
		else{ //Father
			printf("I'm Father\n");
			waitpid(pidComparer, NULL, 0);

			close(pipeFd[1]);
			char tmp[MAX_WORD_LEN];
			while(1){
				if((read(pipeFd[0], tmp, MAX_WORD_LEN)) == -1){
					perror("read");
					exit(1);
				}
				if((strncmp(tmp, "-999", MAX_WORD_LEN)) == 0) break;
				printf("%s", tmp);
			}

			if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
				perror("msgctl");
				exit(1);
			}
		}
	}
	else{ //son1:Sorter
		printf("I'm Sorter\n");
		Sorter(dsMsg, pipeFd, argv[1]);
	}
}

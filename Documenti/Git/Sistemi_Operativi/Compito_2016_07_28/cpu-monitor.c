#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N_RAW 3
#define N_DELTA 2
#define TYPE_RAW 1
#define TYPE_DELTA 2
#define BUF_SIZE 1024
#define DISPLAY_COLUMNS 60

typedef struct{
	long type;
	unsigned long raw[N_RAW]; //0: user, 1: system, 2: idle
	float delta[N_DELTA]; //0: system, 1: user
} msgData;

void Sampler(int dsMsg, int nSample){
	msgData msg;
	msg.type = TYPE_RAW;
	FILE *fd;
	char tmp[BUF_SIZE];
	long dirty;
	//char user[BUF_SIZE], system[BUF_SIZE], idle[BUF_SIZE];

	for(int i=0; i<nSample; i++){
		if((fd = fopen("/proc/stat", "r")) == NULL){
			perror("fopen");
			exit(1);
		}
		if((fgets(tmp, BUF_SIZE, fd)) == NULL){
			perror("fgets");
			exit(1);
		}
		sscanf(tmp, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &msg.raw[0], &dirty, &msg.raw[1], &msg.raw[2], &dirty, &dirty, &dirty, &dirty, &dirty, &dirty);
		/*printf("user:%ld\n", msg.raw[0]);
		printf("system:%ld\n", msg.raw[1]);
		printf("idle:%ld\n", msg.raw[2]);*/

		if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
			perror("msgsnd_RAW");
			exit(1);
		}

		fclose(fd);
		sleep(1);
	}

//	printf("Sampler - exit\n");
	exit(0);
}

void Analyzer(int dsMsg, int nSample){
	msgData msg;
	long lastUser = 0, lastSystem = 0, lastIdle = 0;
	long total = 0;

	for(int i=0; i<nSample; i++){
		if((msgrcv(dsMsg, &msg, sizeof(msgData) - sizeof(long), TYPE_RAW, 0)) == -1){
			perror("msgrcv_RAW");
			exit(1);
		}

		msg.type = TYPE_DELTA;

		total = (msg.raw[0] - lastUser) + (msg.raw[1] - lastSystem) + (msg.raw[2] - lastIdle);
		msg.delta[0] = ((msg.raw[1] - lastSystem)/(float)total);
		msg.delta[1] = ((msg.raw[0] - lastUser)/(float)total);

		lastUser = msg.raw[0];
		lastSystem = msg.raw[1];
		lastIdle = msg.raw[2];

		/*printf("system:%f\n", msg.delta[0]);
		printf("user:%f\n", msg.delta[1]);*/

		if((msgsnd(dsMsg, &msg, sizeof(msgData) - sizeof(long), 0)) == -1){
			perror("msgsnd_DELTA");
			exit(1);
		}
	}

//	printf("Analyzer - exit\n");
	exit(0);
}

void Plotter(int dsMsg, int nSample){
	msgData msg;
	unsigned short systemSpace, userSpace, idleSpace;
	for(int i=0; i<nSample; i++){
		if((msgrcv(dsMsg, &msg, sizeof(msgData) - sizeof(long), TYPE_DELTA, 0)) == -1){
			perror("msgrcv");
			exit(1);
		}

		systemSpace = msg.delta[0]*DISPLAY_COLUMNS;
		userSpace = msg.delta[1]*DISPLAY_COLUMNS;
		idleSpace = DISPLAY_COLUMNS - systemSpace - userSpace;

		for(int i=0; i<systemSpace; i++) printf("%c", '#');
		for(int i=0; i<userSpace; i++) printf("%c", '*');
		for(int i=0; i<idleSpace; i++) printf("%c", '_');

		printf(" s:%5.2f%% u:%5.2f%%\n", msg.delta[0]*100, msg.delta[1]*100);
	}

	exit(0);
}

int main(int argc, char **argv){

	int nSample, dsMsg;

	if(argc < 2) nSample = 30;
	else nSample = atoi(argv[1]);

	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("msgget");
		exit(1);
	}

	if((fork() != 0)){
		if((fork()) != 0){
			if((fork()) == 0){ //son_3: Plotter
				printf("I'm Plotter\n");
				Plotter(dsMsg, nSample);
			}
			else{ //father
				printf("I'm father\n");
				wait(NULL);
				wait(NULL);
				wait(NULL);
				if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
					perror("msgctl");
					exit(1);
				}
			}
		}
		else{ //son_2: Analyzer
			printf("I'm Analyzer\n");
			Analyzer(dsMsg, nSample);
		}
	}
	else{ //son_1: Sampler
		printf("I'm Sampler\n");
		Sampler(dsMsg, nSample);
	}
}
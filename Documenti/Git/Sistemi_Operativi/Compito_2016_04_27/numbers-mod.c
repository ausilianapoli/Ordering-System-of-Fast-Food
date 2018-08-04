#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 10
#define S_EMPTY_X 0 //initial 1 wait(P - before write), signal(Mod - after read)
#define S_FULL_X 1 //initial 0 wait(Mod - before read), signal(P - after write)
#define S_EMPTY_FX 2
#define S_FULL_FX 3
#define BUF_SIZE 16

typedef struct{
	int number[N];
	short type[N]; //0->x; 1->f(x)
} shmData;

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}
int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

void processP(int dsSem, int dsShm, FILE *fd){
	shmData *buffer;
	if((buffer = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmat_P");
		exit(1);
	}

	char tmp[BUF_SIZE];
	int numbers[N];
	int lastPos = N;
	while((fgets(tmp, BUF_SIZE, fd)) != NULL){
		numbers[0] = atoi(tmp);
		for(int i=1; i<N; i++){
			if((fgets(tmp, BUF_SIZE, fd) != NULL)) numbers[i] = atoi(tmp);
			else{
				lastPos = i;
				break;
			}
		}
		if((WAIT(dsSem, S_EMPTY_X)) == -1){
			perror("WAIT_S_EMPTY_X");
			exit(1);
		}
		for(int i=0; i<lastPos; i++){
			//printf("%d\n", numbers[i]);
			buffer->number[i] = numbers[i];
			buffer->type[i] = 0;
		   // printf("%d\n", buffer->number[i]);
		}
		if(lastPos < N){
			for(int i=lastPos; i<N; i++){
				buffer->number[i] = -999;
				buffer->type[i] = -999;
			}
		}
		if((SIGNAL(dsSem, S_FULL_X)) == -1){
			perror("SIGNAL_S_FULL_x");
			exit(1);
		}
	}
	fclose(fd);

	//synchro
	if((WAIT(dsSem, S_EMPTY_X)) == -1){
		perror("WAIT_S_EMPTY_X");
		exit(1);
	}
	buffer->type[0] = -999;
	if((SIGNAL(dsSem, S_FULL_X)) == -1){
		perror("SIGNAL_S_FULL_x");
		exit(1);
	}

	//printf("P - exit\n");
}

void processMOD(int dsSem, int dsShm, int modulus){
	shmData *buffer;
	if((buffer = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmat_MOD");
		exit(1);
	}
	int fx[N];
	int lastPos = N; //for incomplete buffer
	int end = 0;
	while(!end){
		//critical region 1: read
		if((WAIT(dsSem, S_FULL_X)) == -1){
			perror("WAIT_S_FULL_X");
			exit(1);
		}
		for(int i=0; i<lastPos; i++){
			if(buffer->type[i] == 0){
				fx[i] = buffer->number[i];
				//printf("MOD - if\n");
			}
			else if(buffer->type[i] == -999){
				lastPos = i;
				end = 1;
			}
			//printf("%d\n", fx[i]);
			//elaboration of modulus
			fx[i]%=modulus;
			//critical region 2: write
			if((WAIT(dsSem, S_EMPTY_FX)) == -1){
				perror("WAIT_S_EMPTY_FX");
				exit(1);
			}
			if(!end){
				buffer->type[i] = 1;
				buffer->number[i] = fx[i];
			}
			else buffer->type[i] = -999;
			printf("%d\n", buffer->number[i]);
			if((SIGNAL(dsSem, S_FULL_FX)) == -1){
				perror("SIGNAL_S_FULL_FX");
				exit(1);
			}
		}
		if((SIGNAL(dsSem, S_EMPTY_X)) == -1){
			perror("SIGNAL_S_EMPTY_X");
			exit(1);
		}
	}
	//printf("MOD - exit\n");
	exit(0);
}

void processOUT(int dsSem, int dsShm){
	shmData *buffer;
	if((buffer = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmat_OUT");
		exit(1);
	}
	int end=0;
	int i=0;
	while(!end){
		if((WAIT(dsSem, S_FULL_FX)) == -1){
			perror("WAIT_S_FULL_FX");
			exit(1);
		}
		if(buffer->type[i] == -999) end = 1;
		else if(buffer->type[i] == 1){
			printf("%d\n", buffer->number[i]);
		}
		i=(i+1)%N;
		if((SIGNAL(dsSem, S_EMPTY_FX)) == -1){
			perror("SIGNAL_S_EMPTY_FX");
			exit(1);
		}
	}

	//printf("OUT - exit\n");
	exit(0);
}

int main(int argc, char **argv){

	int dsShm, dsSem;
	FILE *fd;

	if(argc != 3){
		printf("Usage <input files> <modulus number>: %s\n", argv[0]);
		exit(0);
	}

	if((dsShm = shmget(IPC_PRIVATE, N*sizeof(shmData), IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsSem = semget(IPC_PRIVATE, 4, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}
	if((semctl(dsSem, S_EMPTY_X, SETVAL, 1)) == -1){
		perror("setVal_S_EMPTY_X");
		exit(1);
	}
	if((semctl(dsSem, S_FULL_X, SETVAL, 0)) == -1){
		perror("setVal_S_FULL_X");
		exit(1);
	}
	if((semctl(dsSem, S_EMPTY_FX, SETVAL, 1)) == -1){
		perror("setVal_S_EMPTY_FX");
		exit(1);
	}
	if((semctl(dsSem, S_FULL_FX, SETVAL, 0)) == -1){
		perror("setVal_S_FULL_FX");
		exit(1);
	}

	if((fd = fopen(argv[1], "r")) == NULL){
		perror("fopen");
		exit(1);
	}

	if((fork() != 0)){
		if((fork() == 0)){// son_2: Mod
			//printf("I'm MOD\n");
			processMOD(dsSem, dsShm, atoi(argv[2]));
		}
		else{ // father: P
			processP(dsSem, dsShm, fd);
			wait(NULL);
			wait(NULL);

		//	printf("father\n");
			if((semctl(dsSem, 0, IPC_RMID, NULL)) == -1){
				perror("semctl");
				exit(1);
			}
			if((shmctl(dsShm, IPC_RMID, NULL)) == -1){
				perror("shmctl");
				exit(1);
			}
		}
	}
	else{ //son_1: OUT
		//printf("I'm OUT\n");
		processOUT(dsSem, dsShm);
	}
}
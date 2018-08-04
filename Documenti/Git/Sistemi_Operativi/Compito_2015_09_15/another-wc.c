#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define S_FULL 0 //initial 0	SIGNAL(son-after_write) - WAIT(father-before_read)
#define S_EMPTY 1 //initial 1	 WAIT(son-before_write) - SIGNAL(father-after_read)
#define BUF_SIZE 4096	

typedef struct{
	char c;
	int eof;
} shmData;

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}
int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

void Son(FILE *fd, int dsSem, int dsShm){
	shmData *p; //for shared memory
	if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmat_Son");
		exit(1);
	}

	int c;
	//long filePointer=ftell(fd);
	//printf("FilePointer: %ld\n", filePointer);
	while((c=fgetc(fd)) != EOF){
		/*critical region 1:write*/
		if((WAIT(dsSem, S_EMPTY)) == -1){
			perror("WAIT_S_Empty");
			exit(1);
		}
		p->c=c;
		p->eof=0;
		//printf("p: %c\n", p->c);
		if((SIGNAL(dsSem, S_FULL)) == -1){
			perror("SIGNAL_S_Full");
			exit(1);
		}
	}
	if((fclose(fd)) == -1){
		perror("close");
		exit(1);
	}

	if((WAIT(dsSem, S_EMPTY)) == -1){
		perror("WAIT_S_Empty");
		exit(1);
	}
	p->eof=1;
	if((SIGNAL(dsSem, S_FULL)) == -1){
		perror("SIGNAL_S_Full");
		exit(1);
	}

	exit(0);
}

void Father(int dsSem, int dsShm, char *file){
	shmData *p; //for shared memory
	if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmat_Father");
		exit(1);
	}

	int countChar=0; //count also special char as , . - ...
	int countWord=0; //count if there is " " | "	"
	int countRow=0; //count if there is '\n'

	char c;

	while(1){
		/*critical region 2: read*/
		if((WAIT(dsSem, S_FULL)) == -1){
			perror("WAIT_S_Full");
			exit(1);
		}
		if(p->eof == 1){
			//printf("Father - My son has finished\n");
			break;
		}
		c=p->c;
		//printf("Father - c: %c\n", c);
		if((SIGNAL(dsSem, S_EMPTY)) == -1){
			perror("SIGNAL_F_Empty");
			exit(1);
		}

		//parsing of buf
		countChar++;
		if(isspace(c)) countWord++;
		if(c == '\n' || c == '\r') countRow++;
	}

	printf("%d  %d  %d  %s\n", countRow, countWord, countChar, file);
}

int main(int argc, char **argv){
	FILE *fd;
	char tmp[BUF_SIZE];
	char dir[BUF_SIZE];
	
	if(argc<2){
		if((getcwd(dir, BUF_SIZE)) == (char*)-1){
			perror("getcwd");
			exit(1);
		}
		strncat(dir, "/sample.txt", BUF_SIZE);
		//printf("dir: %s\n", dir);
		printf("Write a text. To terminate send '--quit--'\n");
		while(1){
			if((fgets(tmp, BUF_SIZE, stdin)) == NULL){
				perror("fgets");
				exit(1);
			}
			if((fd=fopen(dir, "a+")) == NULL){
				perror("fopen");
				exit(1);
			}
			if((strncmp(tmp, "--quit--\n", BUF_SIZE)) == 0) break;
			fprintf(fd, "%s\n", tmp);
			fflush(fd);
			memset(tmp, '\0', BUF_SIZE);
		}
		
		rewind(fd);
	}
	else{
		if((fd=fopen(argv[1], "a+")) == NULL){
			perror("fopen");
			exit(1);
		}
		strncpy(dir, argv[1], BUF_SIZE);
	}

	int dsShm, dsSem;

	if((dsShm=shmget(IPC_PRIVATE, sizeof(shmData), IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsSem=semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}
	if((semctl(dsSem, S_EMPTY, SETVAL, 1)) == -1){
		perror("SETVAL_S_Empty");
		exit(1);
	}
	if((semctl(dsSem, S_FULL, SETVAL, 0)) == -1){
		perror("SETVAL_S_Full");
		exit(1);
	}

	if(fork()==0){ //son
		//printf("I'm Son\n");
		Son(fd, dsSem, dsShm);
	}
	else{ //father
		sleep(1);
		//printf("I'm father\n");
		Father(dsSem, dsShm, dir);
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("RMID_sem");
			exit(1);
		}
		if((shmctl(dsShm, IPC_RMID, 0)) == -1){
			perror("RMID_shm");
			exit(1);
		}
	}

}

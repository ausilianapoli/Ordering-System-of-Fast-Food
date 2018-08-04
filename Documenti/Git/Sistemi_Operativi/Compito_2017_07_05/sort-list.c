#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MAX_WORD_LEN 50
#define SHM_SIZE 1024
#define S_COMPARE 0 //for Sorter-Comparer
#define S_RESULT 1 //for Comparer-Sorter
#define S_COUNT 2 //for Father-Sorter
#define S_OUTPUT 3 //for Sorter-Father

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}

int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

void Sorter(int dsShm, int dsSem, char *file){
	FILE *fd;
	if((fd = fopen(file, "r")) == NULL){
		perror("fdopen");
		exit(1);
	}
	int count=0;
	char tmp[SHM_SIZE];
	while((fgets(tmp, SHM_SIZE, fd)) != NULL){
		if(strlen(tmp)<=MAX_WORD_LEN) count++;
	}
	memset(tmp, '\0', SHM_SIZE);

	rewind(fd);

	char *buf[count];
	int i=0;
	while(i<count){
		if((fgets(tmp, SHM_SIZE, fd)) == NULL){
			perror("fgets");
			exit(1);
		}
		buf[i]=strdup(tmp);
		i++;
	}
	if((fclose(fd) == -1)){
		perror("fclose");
		exit(1);
	}

	char *p; //addres of shared memory
	if((p = (char*)shmat(dsShm, NULL, 0)) == (char*)-1){
		perror("shmat_Sorter");
		exit(1);
	}

	//for(int i=0; i<count && buf[i]!=NULL; i++) printf("%s", buf[i]);
	char *swap;
	for(int i=0; i<count-1; i++){
		for(int j=i+1; j<count; j++){
			//memset(p, '\0', SHM_SIZE);
			/*critical region 1: write*/
			memcpy(p, buf[i], strlen(buf[i]));
			memcpy(p+strlen(buf[i]), buf[j], strlen(buf[j]));
			if((SIGNAL(dsSem, S_COMPARE)) == -1){
				perror("Signal_S_COMPARE");
				exit(1);
			}
			/*critical region 2: read*/
			if((WAIT(dsSem, S_RESULT)) == -1){
				perror("Wait_S_RESULT");
				exit(1);
			}
			int result = atoi(p);
			//printf("result:%d\n", result);
			if(result > 0){
				swap=buf[i];
				buf[i]=buf[j];
				buf[j]=swap;
			}
		}
	}
	/*synchro msg to terminate Comparer*/
	//memset(p, '\0', SHM_SIZE);
	strncpy(p, "exit", SHM_SIZE);
	printf("Sorter - p:%s\n", p);
	if((SIGNAL(dsSem, S_COMPARE)) == -1){
		perror("Signal_S_COMPARE");
		exit(1);
	}
	if((WAIT(dsSem, S_RESULT)) == -1){
		perror("Wait_S_RESULT");
		exit(1);
	}		


	//memset(p, '\0', SHM_SIZE);

	//for(int i=0; i<count; i++) printf("%s", buf[i]);

	/*critical region 3: write*/
	for(int i=0; i<count; i++){
		if((WAIT(dsSem, S_COUNT)) == -1){
			perror("Wait_S_COUNT");
			exit(1);
		}
		memcpy(p, buf[i], MAX_WORD_LEN);
		//strncpy(p, buf[i], SHM_SIZE);
		if((SIGNAL(dsSem, S_OUTPUT)) == -1){
			perror("Signal_S_OUTPUT");
			exit(1);
		}
	}

	if((WAIT(dsSem, S_COUNT)) == -1){
			perror("Wait_S_COUNT");
			exit(1);
		}
	strncpy(p, "exit", SHM_SIZE);
	if((SIGNAL(dsSem, S_OUTPUT)) == -1){
		perror("Signal_S_COMPARE");
		exit(1);
	}

	printf("Sorter - I'm exiting\n");
	
	exit(0);
}

void Comparer(int dsShm, int dsSem){
	char *p; //addres of shared memory
	if((p = (char*)shmat(dsShm, NULL, 0)) == (char*)-1){
		perror("shmat_Comparer");
		exit(1);
	}

	while(1){
		/*critical region 1: read*/
		sleep(1);
		if((WAIT(dsSem, S_COMPARE)) == -1){
			perror("Wait_S_COMPARE");
			exit(1);
		}
		char str1[MAX_WORD_LEN];
		char str2[MAX_WORD_LEN];
		//printf("Comparer - %s\n", str1);
		int i=0;
		while(p[i] != '\n') i++;
		memcpy(str1, p, i);
		str1[i]='\0';
		//printf("Comparer - str1: %s\n", str1);
		if((strncmp(str1, "exit", SHM_SIZE)) == 0){
			printf("Comparer - I'm exiting\n");
			if((SIGNAL(dsSem, S_RESULT)) == -1){
				perror("Signal_S_RESULT");
				exit(1);
			}
			break;
		}
		memcpy(str2, p+i+1, strlen(p));
		str2[strlen(p)]='\0';
		//printf("Comparer - str2: %s\n", str2);

		/*critical regione 2: write*/
		int result=strncasecmp(str1, str2, MAX_WORD_LEN);
		memset(p, '\0', SHM_SIZE);
		sprintf(p, "%d", result);
		//printf("p:%s\n", p);
		if((SIGNAL(dsSem, S_RESULT)) == -1){
			perror("Signal_S_RESULT");
			exit(1);
		}
	}
	exit(0);
}

void Father(int dsShm, int dsSem){
	char *p; //addres of shared memory
	if((p = (char*)shmat(dsShm, NULL, 0)) == (char*)-1){
		perror("shmat_Father");
		exit(1);
	}

	while(1){
		/*critical region 1: read*/
		if((WAIT(dsSem, S_OUTPUT)) == -1){
			perror("Wait_S_OUTPUT");
			exit(1);
		}
		int i=0;
		while(p[i] != '\n') i++;
		char str1[MAX_WORD_LEN];
		strncpy(str1, p, MAX_WORD_LEN);
		if((strncmp(str1, "exit", SHM_SIZE)) == 0) break;
		printf("%s", str1);

		/*critical region 2: write*/
		if((SIGNAL(dsSem, S_COUNT)) == -1){
			perror("Signal_S_COUNT");
			exit(1);
		}
	}
}

int main(int argc, char **argv){
	int dsShm;
	int dsSem;
	pid_t pidComparer;

	if(argc<2){
		printf("Usage: <input file> %s\n", argv[0]);
		exit(0);
	}

	if((dsShm = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}

	if((dsSem = semget(IPC_PRIVATE, 4, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}

	if((semctl(dsSem, S_COMPARE, SETVAL, 0)) == -1){
		perror("semctl_S_COMPARE");
		exit(1);
	}
	if((semctl(dsSem, S_RESULT, SETVAL, 0)) == -1){
		perror("semctl_S_RESULT");
		exit(1);
	}
	if((semctl(dsSem, S_COUNT, SETVAL, 1)) == -1){
		perror("semctl_S_COUNT");
		exit(1);
	}
	if((semctl(dsSem, S_OUTPUT, SETVAL, 0)) == -1){
		perror("semctl_S_OUTPUT");
		exit(1);
	}

	if(fork()!=0){ //father
		if(fork()==0){ //son_2: sorter
			printf("I'm Sorter\n");
			Sorter(dsShm, dsSem, argv[1]);
		}
		else{
			//waitpid(pidComparer, 0, 0);
			sleep(3);
			printf("I'm Father\n");
			Father(dsShm, dsSem);
			if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
				perror("semctl");
				exit(1);
			}
			if((shmctl(dsShm, IPC_RMID,0)) == -1){
				perror("shmctl");
				exit(1);
			}
			exit(0); 
		}
	}
	else{ //son_1: comparer
		printf("I'm Comparer\n");
		pidComparer = getpid();
		Comparer(dsShm, dsSem);
	}

}
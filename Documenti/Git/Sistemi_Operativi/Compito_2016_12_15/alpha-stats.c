#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define S_MUTEX 0
#define SIZE_SHM 4096
#define ALPHABET 26

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}
int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

void Son(int dsSem, int dsShm, char *file){
	int *alphabet; //for shared memory
	if((alphabet = (int*)shmat(dsShm, NULL, 0)) == (int*)-1){
		perror("shmat_Son");
		exit(1);
	}

	int fd;
	if((fd = open(file, O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	struct stat info;
	if((lstat(file, &info)) == -1){
		perror("lstat");
		exit(1);
	}

	char *p; //for map of file
	if((p = (char*)mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	close(fd);

	int letters[ALPHABET];
	memset(letters, 0, ALPHABET*sizeof(int));
	//for(int i=0; i<ALPHABET; i++) letters[i] = 0;
	int i=0;
	while(p[i] != '\0'){
		if(p[i] >= 65 && p[i] <= 90) letters[p[i]-65]++; //upper-case
		else if(p[i] >= 97 && p[i] <= 122) letters[p[i]-97]++; //lower-case
		i++;
	}
	//for(int i=0; i<ALPHABET; i++) printf("%c: %d\n", 'a'+i, letters[i]);

	if((munmap(p, info.st_size)) == -1){
		perror("munmap");
		exit(1);
	}

	if((WAIT(dsSem, S_MUTEX)) == -1){
		perror("WAIT");
		exit(1);
	}
	for(int i=0; i<ALPHABET; i++){
		alphabet[i]+=letters[i];
		usleep(rand()%80000);
	}
	if((SIGNAL(dsSem, S_MUTEX)) == -1){
		perror("SIGNAL");
		exit(1);
	}

	exit(0);
}

int main(int argc, char **argv){

	int dsSem, dsShm;
	int nSon = argc-1;
	pid_t pidFather = getpid();
	int *alphabet;

	if(argc<2){
		printf("Usage <pathaname file> [<pathaname file> ...]: %s\n", argv[0]);
		exit(0);
	}

	if((dsShm = shmget(IPC_PRIVATE, SIZE_SHM, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsSem = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}
	if((semctl(dsSem, S_MUTEX, SETVAL, 1)) == -1){
		perror("setvalSem");
		exit(1);
	}
	if((alphabet = (int*)shmat(dsShm, NULL, 0)) == (int*)-1){
		perror("shmat");
		exit(1);
	}

	//critical-region 1: set alphabet with 0
	if((WAIT(dsSem, S_MUTEX)) == -1){
		perror("WAIT");
		exit(1);
	}
	memset(alphabet, 0, ALPHABET*sizeof(int));
	//for(int i=0; i<ALPHABET; i++) printf("%c: %d\n", 'a'+i, alphabet[i]);
	if((SIGNAL(dsSem, S_MUTEX)) == -1){
		perror("SIGNAL");
		exit(1);
	}

	for(int i=0; i<nSon; i++){
		if((fork()) == 0){
			//printf("I'm Son_%d\n", i+1);
			Son(dsSem, dsShm, argv[i+1]);
		}
	}
	if(getpid() == pidFather){
		for(int i=0; i<nSon; i++) wait(NULL);
		//printf("I'm Father\n");

		float tot = 0;
		/*calculate rates*/
		if((WAIT(dsSem, S_MUTEX)) == -1){
			perror("WAIT");
			exit(1);
		}
		for(int i=0; i<ALPHABET; i++){ 
			tot+=alphabet[i];
			//printf("%c: %d\n", 'a'+i, alphabet[i]);
		}
		for(int i=0; i<ALPHABET; i++){
			printf("%c: %f%%\n", 'a'+i, (alphabet[i]/tot)*100);
		}
		if((SIGNAL(dsSem, S_MUTEX)) == -1){
			perror("SIGNAL");
			exit(1);
		}

		if((shmctl(dsShm, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
	}

}
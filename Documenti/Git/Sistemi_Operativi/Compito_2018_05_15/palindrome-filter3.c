#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define STD_OUT 2
#define BUF_SIZE 4096
#define S_WRITE_RP 0 // start 1: P signal after read, R down before write
#define S_READ_RP 1 // start 0: R signal after write, P down before read
#define S_WRITE_WP 2 // start 1: P down before write, W signal after read
#define S_READ_WP 3 // start 0: W down before read, P up after write

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}
int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

int isPalindrome(char *word){
	word[strlen(word)-1] = '\0';
	for(int i=0; i<strlen(word)/2; i++) if(word[i] != word[strlen(word)-i-1]) return 0;
	return 1;
}

void processR(int dsSem, int dsShmRP, char *file){
	char *seg; //for shared-memory
	if((seg = (char*)shmat(dsShmRP, NULL, 0)) == (char*) -1){
		if((shmctl(dsShmRP, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
		perror("shmat_R");
		exit(1);
	}

	int fd;
	if((fd = open(file, O_RDWR)) == -1){
		perror("open");
		exit(1);
	}
	struct stat info;
	if((lstat(file, &info)) == -1){
		perror("lstat");
		exit(1);
	}

	char *p; //for map
	if((p = (char*)mmap(0, info.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	close(fd);

	char tmp[BUF_SIZE];
	int i=0;
	while(p[i] != '\0'){
		int j=0;
		while(p[j+i] != '\n') j++;
		memcpy(tmp, p+i, j+1);
		tmp[j+1] = '\0';
		//printf("tmp: %s", tmp);
		i+=(j+1);

		//critical_region 1
		if((WAIT(dsSem, S_WRITE_RP)) == -1){
			perror("WAIT_S_WRITE_RP");
			exit(1);
		}
		strncpy(seg, tmp, BUF_SIZE);
		//printf("R:%s", seg);
		if((SIGNAL(dsSem, S_READ_RP)) == -1){
			perror("SIGNAL_S_READ_RP");
			exit(1);
		}
	}

	if((munmap(p, BUF_SIZE)) == -1){
		perror("munmap");
		exit(1);
	}

	//synchro-msg
	snprintf(tmp, BUF_SIZE, "-1");
	if((WAIT(dsSem, S_WRITE_RP)) == -1){
		perror("WAIT_S_WRITE_RP");
		exit(1);
	}
	strncpy(seg, tmp, BUF_SIZE);
	//printf("seg:%s", seg);
	if((SIGNAL(dsSem, S_READ_RP)) == -1){
		perror("SIGNAL_S_READ_RP");
		exit(1);
	}

	exit(0);
}

void processP(int dsSem, int dsShmRP, int dsShmWP){
	char *segRP, *segWP;
	if((segRP = (char*)shmat(dsShmRP, NULL, 0)) == (char*) -1){
		if((shmctl(dsShmRP, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
		perror("shmat_1_P");
		exit(1);
	}
	if((segWP = (char*)shmat(dsShmWP, NULL, 0)) == (char*) -1){
		if((shmctl(dsShmWP, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
		perror("shmat_2_P");
		exit(1);
	}

	char tmp[BUF_SIZE];
	int end = 0;
	//critical_region 2
	while(!end){
		if((WAIT(dsSem, S_READ_RP)) == -1){
			perror("WAIT_S_READ_RP");
			exit(1);
		}
		strncpy(tmp, segRP, BUF_SIZE);
		if((strncmp(tmp, "-1", BUF_SIZE)) == 0){
			end = 1;
			continue;
		}
		//printf("P:%s", tmp);
		if(isPalindrome(tmp)){
			//critical region 3
			if((WAIT(dsSem, S_WRITE_WP)) == -1){
				perror("WAIT_S_WRITE_WP");
				exit(1);
			}
			strncpy(segWP, tmp, BUF_SIZE);
			if((SIGNAL(dsSem, S_READ_WP)) == -1){
				perror("SIGNAL_S_READ_WP");
				exit(1);
			}
		}
		if((SIGNAL(dsSem, S_WRITE_RP)) == -1){
			perror("SIGNAL_S_WRITE_RP");
			exit(1);
		}
	}

	//synchro-msg
	snprintf(tmp, BUF_SIZE, "-1");
	if((WAIT(dsSem, S_WRITE_WP)) == -1){
		perror("WAIT_S_WRITE_WP");
		exit(1);
	}
	strncpy(segWP, tmp, BUF_SIZE);
	if((SIGNAL(dsSem, S_READ_WP)) == -1){
		perror("SIGNAL_S_READ_WP");
		exit(1);
	}
}

void processW(int dsSem, int dsShmWP, int fd){
	char *seg;
	if((seg = (char*)shmat(dsShmWP, NULL, 0)) == (char*) -1){
		if((shmctl(dsShmWP, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
		perror("shmat_W");
		exit(1);
	}

	int end = 0;
	while(!end){
		//critical region 4
		if((WAIT(dsSem, S_READ_WP)) == -1){
			perror("WAIT_S_READ_WP");
			exit(1);
		}
		if((strncmp(seg, "-1", BUF_SIZE)) == 0){
			end = 1;
			continue;
		}
		seg[strlen(seg)] = '\n';
		if((write(fd, seg, strlen(seg))) == -1){
			perror("write");
			exit(1);
		};
		if((SIGNAL(dsSem, S_WRITE_WP)) == -1){
			perror("SIGNAL_S_WRITE_WP");
			exit(1);
		}
	}
	close(fd);
	//printf("W - exit\n");
	exit(0);
}

int main(int argc, char **argv){

	int fd;

	if(argc < 2){
		printf("Usage <input file> [output file]: %s\n", argv[0]);
		exit(0);
	}
	else if(argc == 3){
		if((fd = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0660)) == -1){
			perror("open");
			exit(1);
		}
	}
	else fd = STD_OUT;
	
	int dsShmRP, dsShmWP, dsSem;

	if((dsShmRP = shmget(IPC_PRIVATE, BUF_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsShmWP = shmget(IPC_PRIVATE, BUF_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsSem = semget(IPC_PRIVATE, 4, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}
	if((semctl(dsSem, S_WRITE_RP, SETVAL, 1)) == -1){
		perror("SETVAL_S_WRITE_RP");
		exit(1);
	}
	if((semctl(dsSem, S_READ_RP, SETVAL, 0)) == -1){
		perror("SETVAL_S_READ_RP");
		exit(1);
	}
	if((semctl(dsSem, S_WRITE_WP, SETVAL, 1)) == -1){
		perror("SETVAL_S_WRITE_WP");
		exit(1);
	}
	if((semctl(dsSem, S_READ_WP, SETVAL, 1)) == -1){
		perror("SETVAL_S_READ_WP");
		exit(1);
	}

	if((fork() != 0)){ //father
		if((fork() == 0)){ //son_2: R
			printf("I'm R\n");
			processR(dsSem, dsShmRP, argv[1]);
		}
		else{
			sleep(1);
			printf("I'm P\n");

			processP(dsSem, dsShmRP, dsShmWP);

			if((semctl(dsSem, 0, IPC_RMID, NULL)) == -1){
				perror("semctl");
				exit(1);
			}
			if((shmctl(dsShmRP, IPC_RMID, NULL)) == -1){
				perror("shmctl");
				exit(1);
			}
			if((shmctl(dsShmWP, IPC_RMID, NULL)) == -1){
				perror("shmctl");
				exit(1);
			}
		}
	}
	else{ //son_1: W
		printf("I'm W\n");
		processW(dsSem, dsShmWP, fd);
	}
}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>

#define S_WRPATH 0 //initial 1 --> SCANNERn: wait(before write); STATER: signal(after read)
#define S_RDPATH 1 //initial 0 --> SCANNERn: signal(after write); STATER: wait(before read)
#define S_WRSIZE 2 //initial 1 --> FATHER: signal(after read); STATER: wait(before write)
#define S_RDSIZE 3 //initial 0 --> FATHER: wait(before read); STATER: signal(after write);
#define MAX_PATH_LEN 1024

typedef struct{
	char buf[MAX_PATH_LEN];
	int id;
} shmData;

int WAIT(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
	return semop(sem_des, operazioni, 1);
}

int SIGNAL(int sem_des, int num_semaforo){
	struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
	return semop(sem_des, operazioni, 1);
}

void ScanDir(int dsSem, int dsShm, char *path, int id){
	shmData *p; //for shared memory
	if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmData_ScanDir");
		exit(1);
	}

	struct stat info;
	if((lstat(path, &info)) == -1){
		perror("lstat");
		exit(1);
	}
	if(S_ISDIR(info.st_mode)){
		DIR *fdDir;
		struct dirent *pointerDir;
		if((fdDir = opendir(path)) == NULL){
			perror("opendir");
			exit(1);
		}
		while((pointerDir = readdir(fdDir)) != NULL){
			if((chdir(path)) == -1){
				perror("chdir");
				exit(1);
			}
			char tmp[MAX_PATH_LEN];
			strncpy(tmp, path, MAX_PATH_LEN);
			strncat(tmp, "/", MAX_PATH_LEN);
			strncat(tmp, pointerDir->d_name, MAX_PATH_LEN);
			//printf("tmp: %s\n", tmp);
			if((lstat(tmp, &info)) == -1){
				perror("lstat");
				exit(1);
			}
			if(S_ISDIR(info.st_mode)){
				if(((strncmp(pointerDir->d_name, ".", MAX_PATH_LEN) == 0) || (strncmp(pointerDir->d_name, "..", MAX_PATH_LEN) == 0))) continue;
				ScanDir(dsSem, dsShm, tmp, id);
			}
			else{
				//send path
				//critical region 1: write
				if((WAIT(dsSem, S_WRPATH)) == -1){
					perror("WAIT_S_WRPATH");
					exit(1);
				}
				p->id=id;
				strncpy(p->buf, tmp, MAX_PATH_LEN);
		//		printf("p->buf: %s\n", p->buf);
				if((SIGNAL(dsSem, S_RDPATH)) == -1){
					perror("SIGNAL_S_RDPATH");
					exit(1);
				}
			}
		}
		chdir("..");
	}
	else{
		//send path
		//critical region 1: write
		if((WAIT(dsSem, S_WRPATH)) == -1){
			perror("WAIT_S_WRPATH");
			exit(1);
		}
		p->id=id;
		strncpy(p->buf, path, MAX_PATH_LEN);
		if((SIGNAL(dsSem, S_RDPATH)) == -1){
			perror("SIGNAL_S_RDPATH");
			exit(1);
		}
	}
}

void Scanner(int dsSem, int dsShm, char *path, int id){
	shmData *p; //for shared memory
	if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmData_Scanner");
		exit(1);
	}

	ScanDir(dsSem, dsShm, path, id);
	//synchro-msg
	if((WAIT(dsSem, S_WRPATH)) == -1){
		perror("WAIT_S_WRPATH");
		exit(1);
	}
	p->id=0; 
	if((SIGNAL(dsSem, S_RDPATH)) == -1){
		perror("SIGNAL_S_RDPATH");
		exit(1);
	}

	printf("Scanner_%d - I'm exiting\n", id);
	exit(0);
}

void Stater(int dsSem, int dsShm, int nScan){
	shmData *p; //for shared memory
	int count=0;
	if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
		perror("shmData_Stater");
		exit(1);
	}
	char path[MAX_PATH_LEN];
	struct stat info;
	blkcnt_t blocks;

	while(1){
		if((WAIT(dsSem, S_RDPATH)) == -1){
			perror("WAIT_S_RDPATH");
			exit(1);
		}
		int id=p->id;
		if(id == 0){
			count++;
			if(count == nScan){
				printf("Stater - I'm exiting\n");
				break;
			}
			if((SIGNAL(dsSem, S_WRPATH)) == -1){
				perror("SIGNAL_WRPATH_1");
				exit(1);
			}
		}
		else{
			strncpy(path, p->buf, MAX_PATH_LEN);

			/*critical region 2: write size*/
			if((lstat(path, &info)) == -1){
				perror("lstat_Stater");
				exit(1);
			}
			blocks=info.st_blocks;
			//printf("blocks: %ld\n", blocks);
			if((WAIT(dsSem, S_WRSIZE)) == -1){
				perror("WAIT_S_WRSIZE_2");
				exit(1);
			}
			p->id=id;
			sprintf(p->buf, "%ld", blocks);
			//printf("p->buf: %s\n", p->buf);
			if((SIGNAL(dsSem, S_RDSIZE)) == -1){
				perror("SIGNAL_S_RDSIZE");
				exit(1);
			}
			if((SIGNAL(dsSem, S_WRPATH)) == -1){
				perror("SIGNAL_WRPATH_2");
				exit(1);
			}
		}
	}

	//synchro msg for father
	if((WAIT(dsSem, S_WRSIZE)) == -1){
		perror("WAIT_S_WRSIZE");
		exit(1);
	}
	p->id=0;
	if((SIGNAL(dsSem, S_RDSIZE)) == -1){
		perror("SIGNAL_S_RDSIZE");
		exit(1);
	}
}

int main(int argc, char **argv){

	int dsSem, dsShm;
	int nScan=argc-1;
	int id[nScan];
	for(int i=1; i<=nScan; i++){
		id[i-1]=i;
	//	printf("id[%d]:%d\n",i-1, i);
	} 
		

	if(argc<2){
		printf("Usage <path> [<path>...]:%s\n", argv[0]);
		exit(0);
	}

	if((dsShm = shmget(IPC_PRIVATE, sizeof(shmData), IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("shmget");
		exit(1);
	}
	if((dsSem = semget(IPC_PRIVATE, 4, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("semget");
		exit(1);
	}
	if((semctl(dsSem, S_WRPATH, SETVAL, 1)) == -1){
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
		perror("setVal_S_WRPATH");
		exit(1);
	}
	if((semctl(dsSem, S_RDPATH, SETVAL, 0)) == -1){
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
		perror("setVal_S_RDPATH");
		exit(1);
	}
	if((semctl(dsSem, S_WRSIZE, SETVAL, 1)) == -1){
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
		perror("setVal_S_WRSIZE");
		exit(1);
	}
	if((semctl(dsSem, S_RDSIZE, SETVAL, 0)) == -1){
		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
		perror("setVal_S_RDSIZE");
		exit(1);
	}
	pid_t pidFather=getpid();

	if((fork()) != 0){ //father
		for(int i=0; i<nScan; i++){
			if((fork()) == 0){
				printf("I'm Scanner_%d\n", i+1);
				Scanner(dsSem, dsShm, argv[i+1], id[i]);
			}
		}
	}
	else{
		printf("I'm Stater\n");
		Stater(dsSem, dsShm, nScan);
	}
	if((getpid()) == pidFather){
		sleep(2);
		printf("I'm Father\n");
		//Father(dsSem, dsShm, nScan);
		shmData *p; //for shared memory
		if((p = (shmData*)shmat(dsShm, NULL, 0)) == (shmData*)-1){
			perror("shmData_Stater");
			exit(1);
		}
		blkcnt_t blocks[nScan];
		for(int i=0; i<nScan; i++) blocks[i]=0l;
		int id;
		blkcnt_t val;
		while(1){
			/*critical region 3: read size*/
			if((WAIT(dsSem, S_RDSIZE)) == -1){
				perror("WAIT_S_RDSIZE");
				exit(1);
			}
			id=p->id;
			if(id == 0) break;
			val=atol(p->buf);
			blocks[id-1]+=val;
			//printf("val:%ld\n", val);
			if((SIGNAL(dsSem, S_WRSIZE)) == -1){
				perror("SIGNAL_S_WRSIZE");
				exit(1);
			}
		}
		for(int i=0; i<nScan; i++) printf("%ld\t%s\n", blocks[i], argv[i+1]);

		if((semctl(dsSem, 0, IPC_RMID, 0)) == -1){
			perror("semctl");
			exit(1);
		}
		if((shmctl(dsShm, IPC_RMID, NULL)) == -1){
			perror("shmctl");
			exit(1);
		}
	}

	
}

				
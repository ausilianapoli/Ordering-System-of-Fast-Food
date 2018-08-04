#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/wait.h>

#define ALPHABET 26

typedef struct{
	long type;
	int alphabet[ALPHABET];
} message;

void Son(int dsMsg, char *file, int id){
	message msg;
	struct stat info;
	int fd;
	char *p; //for map
	int letters[ALPHABET];

	if((fd = open(file, O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	if((lstat(file, &info)) == -1){
		perror("lstat");
		exit(1);
	}
	if((p = (char*)mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	close(fd);

	
	//for(int i=0; i<ALPHABET; i++) letters[i] = 0;
	memset(letters, 0, ALPHABET*sizeof(int));
	for(int i=0; i<strlen(p); i++){
		if(p[i] >= 65 && p[i] <= 90) letters[p[i]-65]++;
		else if(p[i] >= 97 && p[i] <= 122) letters[p[i]-97]++;
	}
	
	//for(int i=0; i<ALPHABET; i++) msg.alphabet[i] = letters[i];
	memcpy(msg.alphabet, letters, ALPHABET*sizeof(int));
	msg.type = 1;
	printf("\nprocesso T-%d su file '%s':\n", id, file);
	for(int i=0; i<ALPHABET; i++) printf("%c: %d  ", 'a'+i, msg.alphabet[i]);
	if((msgsnd(dsMsg, &msg, sizeof(msg)-sizeof(long), 0)) == -1){
		perror("msgsnd");
		exit(1);
	}

	if((munmap(p, info.st_size)) == -1){
		perror("munmap");
		exit(1);
	}

	exit(0);
}

int main(int argc, char **argv){

	int dsMsg;
	message msg;
	pid_t pidFather = getpid();
	int nSon = argc-1;
	int total[ALPHABET];
	memset(total, 0, ALPHABET*sizeof(int));
	//for(int i=0; i<ALPHABET; i++) total[i] = 0;

	if(argc<2){
		printf("Usage <file pathname> [<file pathname> ...]: %s\n", argv[0]);
		exit(0);
	}

	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("msgget");
		exit(1);
	}

	for(int i=0; i<nSon; i++){
		if((fork()) == 0){
			//printf("I'm Son_%d\n", i+1);
			Son(dsMsg, argv[i+1], i+1);
		} 
	}

	if((getpid() == pidFather)){
		for(int i=0; i<nSon; i++) wait(NULL);
		//printf("I'm Father\n");

		for(int i=0; i<nSon; i++){
			if((msgrcv(dsMsg, &msg, ALPHABET*sizeof(int), 0, 0)) == -1){
				perror("msgrcv");
				exit(1);
			}
			for(int i=0; i<ALPHABET; i++){
				total[i]+=msg.alphabet[i];
			}
			//for(int i=0; i<ALPHABET; i++) printf("\n%c: %d\n", 'a'+i, total[i]);
		}

		int exaeqo = 0;
		int max = 0;
		for(int i=1; i<ALPHABET; i++){
			if(total[i] >= total[max]){
				if(total[i] == total[max]) exaeqo = 1;
				max = i;
			}
		}
		printf("\nprocesso padre P:\n");
		for(int i=0; i<ALPHABET; i++) printf("%c: %d  ", 'a'+i, total[i]);
		if(!exaeqo) printf("\nLettera piu' utilizzata: %c\n", max+'a'); 
		if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
			perror("msgctl");
			exit(1);
		}
	}
}
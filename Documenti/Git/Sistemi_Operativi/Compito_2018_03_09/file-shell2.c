#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>

#define BUF_SIZE 1024

typedef struct{
	long type;
	char text[BUF_SIZE];
	char file[BUF_SIZE];
	char character;
} message;

int numFiles(char *path){
	printf("numFiles\n");
	DIR *dir;
	if((dir = opendir(path)) == NULL){
		perror("opendir");
		exit(1);
	}
	struct dirent *pointerDir;
	struct stat info;
	char file[BUF_SIZE];
	int count=0;
	//printf("path: %s\n", path);
	if((pointerDir = readdir(dir)) == NULL){
		perror("readdir");
		exit(1);
	}
	while((pointerDir = readdir(dir)) != NULL){
		//printf("numFiles - loop\n");
		strncpy(file, path, BUF_SIZE);
		strncat(file, "/", BUF_SIZE);
		strncat(file, pointerDir->d_name, BUF_SIZE);
		//printf("file: %s\n", file);
		if((lstat(file, &info)) == -1){
			perror("lstat");
			exit(1);
		}
		if(S_ISREG(info.st_mode)) count++;
	}
	return count;
}

int totalSize(char *path){
	printf("totalSize\n");
	DIR *dir;
	if((dir = opendir(path)) == NULL){
		perror("opendir");
		exit(1);
	}
	struct dirent *pointerDir;
	struct stat info;
	char file[BUF_SIZE];
	off_t count=0l;
	//printf("path: %s\n", path);
	if((pointerDir = readdir(dir)) == NULL){
		perror("readdir");
		exit(1);
	}
	while((pointerDir = readdir(dir)) != NULL){
		//printf("numFiles - loop\n");
		strncpy(file, path, BUF_SIZE);
		strncat(file, "/", BUF_SIZE);
		strncat(file, pointerDir->d_name, BUF_SIZE);
		//printf("file: %s\n", file);
		if((lstat(file, &info)) == -1){
			perror("lstat");
			exit(1);
		}
		if(S_ISREG(info.st_mode)) count+=info.st_size;
	}
	return (int)count;
}

int searchChar(char *dir, char *file, char c){
	//printf("file: %s\n", file);
	strncat(dir, "/", BUF_SIZE);
	strncat(dir, file, BUF_SIZE);
	//printf("dir:%s\n", dir);

	int fd;
	if((fd = open(dir, O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	struct stat info;
	if((lstat(dir, &info)) == -1){
		perror("lstat");
		exit(1);
	}

	char *p; //map
	if((p = (char*)mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	close(fd);

	int count=0;
	for(int i=0; i<strlen(p); i++){
		if(p[i] == c) count++;
	}

	if((munmap(p, info.st_size)) == -1){
		perror("munmap");
		exit(1);
	}

	return count;
}

void processD(int dsMsg, char *dir, int id){
	message msg;
	int result;
	
	while(1){
		if((msgrcv(dsMsg, &msg, sizeof(msg)-sizeof(long), (long)id, 0)) == -1){
			perror("msgrcv1_D");
			exit(1);
		}
		//printf("msg.text: %s\n", msg.text);
		if((strncmp(msg.text, "a", 1)) == 0){
			//num-files
			result=numFiles(dir);
			//printf("result: %d\n", result);
			snprintf(msg.text, BUF_SIZE, "%d", result);
			if((msgsnd(dsMsg, &msg, strlen(msg.text), 0)) == -1){
				perror("msgsndP_a");
				exit(1);
			}
		}
		else if((strncmp(msg.text, "b", 1)) == 0){
			//total-size
			result = totalSize(dir);
			//printf("result: %d\n", result);
			snprintf(msg.text, BUF_SIZE, "%d", result);
			if((msgsnd(dsMsg, &msg, strlen(msg.text), 0)) == -1){
				perror("msgsndP_b");
				exit(1);
			}
		}
		else{
			//search-char
			char file[BUF_SIZE];
			strncpy(file, msg.file, BUF_SIZE);
			//printf("D - file %s\n", msg.file);
			//rcv char
			char c = msg.character;
			result = searchChar(dir, file, c);
			//printf("result: %d\n", result);
			snprintf(msg.text, BUF_SIZE, "%d", result);
			if((msgsnd(dsMsg, &msg, strlen(msg.text), 0)) == -1){
				perror("msgsndP_c");
				exit(1);
			}
		}
	}
}

int main(int argc, char **argv){

	int dsMsg;
	struct stat info;
	message msg;

	if(argc<2){
		printf("Usage <pathname dir> [<pathname dir> ...]: %s\n", argv[0]);
		exit(0);
	}
	else{
		for(int i=1; i<argc; i++){
			if((lstat(argv[i], &info)) == -1){
				perror("lstat");
				exit(1);
			}
			if(!S_ISDIR(info.st_mode)){
				printf("A parameter isn't a directory\n");
				exit(0);
			}
		}
	}

	if((dsMsg = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600)) == -1){
		perror("msgget");
		exit(1);
	}

	pid_t pidFather = getpid();
	int nD = argc-1;
	for(int i=0; i<nD; i++){
		if((fork()) == 0){
		//	printf("I'm D_%d\n", i+1);
			processD(dsMsg, argv[i+1], i+1);
		}
	}
	if((getpid() == pidFather)){
		sleep(1);
	//	printf("I'm processP\n");

		char c, g;
		int d;
		char nameFile[BUF_SIZE];
		do{
			printf("To exit press x\n");
			printf("For:\n - num-files -> press a;\n - total-size -> press b;\n - search-char -> press c\n");
			scanf(" %c", &c);
			if(c == 'a'){
				//num-files
				printf("\tnum-files\n");
				printf("\tSelect the dir with a number from 1 to %d\n", nD);
				scanf(" %d", &d);
				if(d < 1 || d > nD){
					printf("\t\tBad number. Try again!\n");
				}
				else{
					msg.type=d;
					snprintf(msg.text, BUF_SIZE, "%c", c);
					//printf("msg.text: %s\n", msg.text);
					if((msgsnd(dsMsg, &msg, strlen(msg.text), 0)) == -1){
						perror("msgsndP_a");
						exit(1);
					}
					if((msgrcv(dsMsg, &msg, BUF_SIZE, 0, 0)) == -1){
						perror("msgrcv_P");
						exit(1);
					}
					printf("%s file\n", msg.text);
				}
			}
			else if(c == 'b'){
				//total-size
				printf("\ttotal-size\n");
				printf("\tSelect the dir with a number from 1 to %d\n", nD);
				scanf(" %d", &d);
				if(d < 1 || d > nD){
					printf("\t\tBad number. Try again!\n");
				}
				else{
					msg.type=d;
					snprintf(msg.text, BUF_SIZE, "%c", c);
					if((msgsnd(dsMsg, &msg, strlen(msg.text), 0)) == -1){
						perror("msgsndP_b");
						exit(1);
					}
					if((msgrcv(dsMsg, &msg, BUF_SIZE, 0, 0)) == -1){
						perror("msgrcv_P");
						exit(1);
					}
					printf("%s byte\n", msg.text);
				}
			}
			else if(c == 'c'){
				//search-char
				printf("\tsearch-char\n");
				printf("\tSelect the dir with a number from 1 to %d\n", nD);
				scanf(" %d", &d);
				printf("\tWrite the name of the file\n");
				scanf(" %s", nameFile);
				printf("\tWrite the char to search in the file\n");
				scanf(" %c", &g);
				if(d < 1 || d > nD){
					printf("\t\tBad number. Try again!\n");
				}
				else{
					msg.type=d;
					snprintf(msg.text, BUF_SIZE, "%c", c);
					//send the action
					strncpy(msg.file, nameFile, BUF_SIZE);
					//send the character
					msg.character=g;
					if((msgsnd(dsMsg, &msg, sizeof(msg)-sizeof(long), 0)) == -1){
						perror("msgsndP_c");
						exit(1);
					}

					if((msgrcv(dsMsg, &msg, sizeof(msg)-sizeof(long), 0, 0)) == -1){
						perror("msgrcv_P");
						exit(1);
					}
					printf("%s\n", msg.text);
				}
			}
			else if(c != 'x'){
				printf("\tBad character. Try again!\n");
			}
		}
		while(c != 'x');

		if((kill(0, SIGINT)) == -1){
			perror("kill");
			exit(1);
		}

		if((msgctl(dsMsg, IPC_RMID, NULL)) == -1){
			perror("msgctl");
			exit(1);
		}
		exit(0);
	}
}
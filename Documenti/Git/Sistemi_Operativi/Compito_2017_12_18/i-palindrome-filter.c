#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUF_SIZE 4096

int isPalindrome(char *word){
	for(int i=0; i<(strlen(word))/2; i++){
		if(word[i] != word[strlen(word)-1-i]){
			if(word[i]==(char)tolower(word[strlen(word)-1-i]) || word[i]==(char)toupper(word[strlen(word)-1-i])){
				continue;
			}
			else return 0;
		}
	}
	return 1;
}

void ProcessR(char *path, char *fifo){
	//printf("R\n");
	int fd;
	int dsFifoPR;
	if((fd = open(path, O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	struct stat info;
	if((fstat(fd, &info)) == -1){
		perror("fstat");
		exit(1);
	}
	//map the file pointed by fd
	char *p; //address of the map
	if((p = (char*)mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	close(fd);
	//printf("p: %s\n", p);

	//printf("fifo%s\n", fifo);
	if((dsFifoPR=open(fifo, O_RDWR)) == -1){
		unlink(fifo);
		perror("openFifoPR");
		exit(1);
	}

	//use the FIFO with P
	long len;
	if((len = write(dsFifoPR, p, BUF_SIZE)) == -1){
		perror("write");
		exit(1);
	}
	//printf("R - len: %ld\n", len);

	if((munmap(p, BUF_SIZE)) == -1){
		perror("munmap");
		exit(1);
	}
}

void ProcessP(char *fifoPR, char *fifoPW){
	//read from FIFO_PR
	int dsFifoPR;
	if((dsFifoPR = open(fifoPR, O_RDWR)) == -1){
		perror("openFifoPR");
		unlink(fifoPR);
		exit(1);
	}
	char buf[BUF_SIZE];
	long len;
	if((len = read(dsFifoPR, buf, BUF_SIZE)) == -1){
		perror("read");
		exit(1);
	}
	//printf("P - len:%ld\n", len);
	buf[strlen(buf)]='\0';
	//printf("P - buf%s\n", buf);

	//analyse file
	//memset(buf, '\0', BUF_SIZE);
	char tmp[BUF_SIZE];
	char bufPalindrome[BUF_SIZE];
	int pos=0; //store the position in the bufPalindrome
	int i=0;
	while(buf[i] != '\0'){ //i<(len-1)
		//printf("buf[i]:%c\t", buf[i]);
		int j=0;
		//printf("i: %d j:%d\n",i, j);
		while(buf[j+i] != '\n') j++; //obtain string without \n
		//printf("i: %d j:%d\n",i, j);
		memcpy(tmp, buf+i, j);
		//printf("tmp1: %s\n", tmp);
		if((isPalindrome(tmp)) == 1){
			//printf("tmp2: %s\n", tmp);
			memcpy(bufPalindrome+pos, tmp, strlen(tmp));
			pos=strlen(bufPalindrome);
			bufPalindrome[pos++]='\n';
			//printf("bufPalindrome: %s\n", bufPalindrome);
			//printf("pos: %d\n", pos);
		}
		i+=(j+1);
		memset(tmp, '\0', BUF_SIZE);
		//printf("loop\n");
	}

	//write on FIFO_PW
	int dsFifoPW;
	if((dsFifoPW = open(fifoPW, O_RDWR)) == -1){
		perror("openFifoPW");
		unlink(fifoPW);
		exit(1);
	}
	if((write(dsFifoPW, bufPalindrome, BUF_SIZE)) == -1){
		perror("write");
		exit(1);
	}
}

void ProcessW(char *fifo){
	//read from FIFO_PW
	//printf("W started\n");
	int dsFifo;
	if((dsFifo = open(fifo, O_RDWR)) == -1){
		perror("openFifo");
		unlink(fifo);
		exit(1);
	}
	
	char buf[BUF_SIZE];
	int len;
	if((len=read(dsFifo, buf, BUF_SIZE)) == -1){
		perror("read");
		exit(1);
	}
	printf("Palindrome_buf:\n%s\n", buf);
}

int main(int argc, char **argv){
	int fd; //file descriptor tmp for the file in input/stdin
	char buf[BUF_SIZE];
	char dir[BUF_SIZE]; //I create the file in current working directory
	char fifoPR[BUF_SIZE]; //pathname of fifo
	char fifoPW[BUF_SIZE]; //pathname of fifo
	strncpy(fifoPR, "/tmp/fifoPR", BUF_SIZE);
	strncpy(fifoPW, "/tmp/fifoPW", BUF_SIZE);
	//int dsFifoPR, dsFifoPW;

	if(argc<2){
		printf("Write the word to create the file. Send 'quit' to terminate\n");
		int i=0;
		while(1){
			if((fgets(buf, BUF_SIZE, stdin)) == NULL){
				perror("fgets");
				exit(1);
			}
			//buf[strlen(buf)]='\0';
			//printf("buf: %s\n", buf);
			int len=strlen(buf);
			buf[len+1]='\0';
			//printf("len:%d\n", len);
			if((strncmp(buf, "quit\n", BUF_SIZE)) == 0) break;

			if((getcwd(dir, BUF_SIZE)) == (char*)-1){
				perror("getcwd");
				exit(1);
			}
			strcat(dir, "/sample.txt");

			if((fd = open(dir, O_RDWR | O_APPEND |O_CREAT, 0666)) == -1){
				perror("open");
				exit(1);
			}

			if((write(fd, buf, len)) == -1){
				perror("write");
				exit(1);
			}
			memset(buf, '\0', BUF_SIZE); //to clean the buf
		}
	}
	else{
		strncpy(dir, argv[1], BUF_SIZE);
	}
	close(fd);

	//create and open fifo
	if((mkfifo(fifoPR, 0600)) == -1){
		perror("mkfifoPR");
		exit(1);
	}
	if((mkfifo(fifoPW, 0600)) == -1){
		perror("mkfifoPW");
		exit(1);
	}
	/*
	if((dsFifoPW=open(fifoPW, O_RDWR)) == -1){
		perror("openFifoPR");
		exit(1);
	}*/

	if((fork() != 0)){//father - P
		if((fork() == 0)){//son_2 - W
			printf("I'm W\n");
			ProcessW(fifoPW);
		}
		else{//father - P
			//sleep(1);
			printf("I'm P\n");
			ProcessP(fifoPR, fifoPW);
			if((unlink(fifoPR)) == -1){
				perror("unlinkFifoPR");
				exit(1);
			}
			if((unlink(fifoPW)) == -1){
				perror("unlinkFifoPW");
				exit(1);
			}
		}
	}
	else{//son_1 - R
		printf("I'm R\n");
		ProcessR(dir, fifoPR); //path, fifoPR
	}

}

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/time.h>
#include<time.h>
#define MAX_SIZE 2000000
int main(int argc, char* argv[]){
        struct timespec start,end;
        clock_gettime(CLOCK_REALTIME, &start);
	int fd=open(argv[1],O_RDWR); 
	clock_gettime(CLOCK_REALTIME, &end);
        printf("First Open %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
        if(fd==-1)
		printf("Could not open file\n");
	//char buf[1024];
	char buf[MAX_SIZE];
        clock_gettime(CLOCK_REALTIME, &start);
	int noBytes=read(fd,buf,MAX_SIZE);
	clock_gettime(CLOCK_REALTIME, &end);
        printf("First Read %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
//	printf("buf read is %s\n",buf);
        char buf2[MAX_SIZE]="This is what we wish to write in my out temp file\n";
        int len=strlen(buf2);
        lseek(fd,0,SEEK_END);
        clock_gettime(CLOCK_REALTIME, &start);
        noBytes=write(fd,buf2,len);
	clock_gettime(CLOCK_REALTIME, &end);
        printf("First Write %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
        if(noBytes==-1){
		printf("Error in writing");
		return -1;
	}
        clock_gettime(CLOCK_REALTIME, &start);
        int fd1=open(argv[1],O_RDWR);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("Second Open %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));

        lseek(fd1,0,0);
        char buf1[MAX_SIZE];
        clock_gettime(CLOCK_REALTIME, &start);
          noBytes=read(fd1,buf1,MAX_SIZE);
	clock_gettime(CLOCK_REALTIME, &end);
        printf("Second Read %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
  //       printf("buf read second time is %s\n",buf1);
        
     char buf3[MAX_SIZE]="Something that i want to write again\n";
         len=strlen(buf3);
         lseek(fd1,0,SEEK_END);
        clock_gettime(CLOCK_REALTIME, &start);
         noBytes=write(fd1,buf3,len);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("Second Write %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
         if(noBytes==-1){
                 printf("Error in writing");
                 return -1;
         }
  /*       lseek(fd,0,0);
         char buf4[1024];
           noBytes=read(fd,buf4,1024);
          printf("buf read third time is %s\n",buf4);
*/
	clock_gettime(CLOCK_REALTIME, &start);
	close(fd);
	 clock_gettime(CLOCK_REALTIME, &end);
	printf("On Close %ld\n",((end.tv_sec*1000000000+end.tv_nsec)-(start.tv_sec*1000000000+start.tv_nsec)));
	close(fd1);
        return 0;

}
	

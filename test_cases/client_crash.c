#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
int main(){
	
	int fd=open("client_crash.txt",O_RDWR);
        if(fd==-1)
		printf("Could not open file\n");
	char buf[1024];
	int noBytes=read(fd,buf,1024);
//	printf("buf read is %s\n",buf);
        char buf2[1024]="This is to check if client crash can work\n";
        int len=strlen(buf2);
        lseek(fd,0,SEEK_END);
        noBytes=write(fd,buf2,len);
        if(noBytes==-1){
		printf("Error in writing");
		return -1;
	}
        lseek(fd,0,0);
        char buf1[1024];
          noBytes=read(fd,buf1,1024);
  //       printf("buf read second time is %s\n",buf1);
        
     char buf3[1024]="When i am back up i should get log file and restart\n";
         len=strlen(buf3);
         lseek(fd,0,SEEK_END);
         noBytes=write(fd,buf3,len);
         if(noBytes==-1){
                 printf("Error in writing");
                 return -1;
         }
         lseek(fd,0,0);
         char buf4[1024];
           noBytes=read(fd,buf4,1024);
          printf("buf read third time is %s\n",buf4);

          sleep(30); 
 
        return 0;

}
	

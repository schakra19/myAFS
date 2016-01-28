#include "examplefs.hh"
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <grpc++/grpc++.h>
#include "helloworld.grpc.pb.h"
#include <sys/stat.h>
using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
using helloworld::HelloFlushRequest;
using helloworld::HelloStat;
using helloworld::HelloOpendir;
using helloworld::HelloDir;
using helloworld::HelloDirent;
using helloworld::HelloMkdirReply;
using helloworld::HelloMknodRequest;

std::string port("10.0.1.112:50055");
std::string homeLogDir("/home/ubuntu/CS739/fuse_log");

ExampleFS* ExampleFS::_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

ExampleFS* ExampleFS::Instance() {
	if(_instance == NULL) {
		_instance = new ExampleFS();
	}
	return _instance;
}

ExampleFS::ExampleFS() {

}

ExampleFS::~ExampleFS() {

}

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& user) {
    HelloRequest request;
    request.set_name(user);
    HelloReply reply;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &reply);

    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

 std::string myOpen(const std::string& user) {
    HelloRequest request;
    request.set_name(user);
    HelloReply reply;

    ClientContext context;

    Status status = stub_->myOpen(&context, request, &reply);

    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

std::string myClose(const std::string& path, const std::string& content)  {
      HelloFlushRequest request;
      request.set_path(path);
      request.set_content(content);
      HelloReply reply;
  
      ClientContext context;
  
      Status status = stub_->myClose(&context, request, &reply);
  
      if (status.ok()) {
        return reply.message();
      } else {
        return "RPC failed";
      }
    }

uint64_t myOpendir(const std::string& user) {
    HelloRequest request;
    request.set_name(user);
    HelloOpendir reply;

    ClientContext context;

    Status status = stub_->myOpendir(&context, request, &reply);

    if (status.ok()) {
      return reply.dir();
    } else {
      return -1;
    }
  }

helloworld::HelloDirent myReaddir(uint64_t user) {
     HelloDir request;
     request.set_fh(user);
     HelloDirent reply;
 
     ClientContext context;
 
     Status status = stub_->myReaddir(&context, request, &reply);
 
     if (status.ok()) {
       return reply;
     } else {
       //return "RPC Failed";
     }
   }


  helloworld::HelloStat myGetattr(const std::string& user) {
    HelloRequest request;
    request.set_name(user);
    HelloStat reply;

    ClientContext context;

    Status status = stub_->myGetattr(&context, request, &reply);

    if (status.ok()) {
      return reply;
    } else {
//      return;
    }
  }
std::string myReleasedir(uint64_t user) {
      
      HelloDir request;
      request.set_fh(user);
      HelloReply reply;
      
      ClientContext context;
 
      Status status = stub_->myReleasedir(&context, request, &reply);
 
      if (status.ok()) {
        return reply.message();
      } else {
        return "RPC Failed";
      }
 }
helloworld::HelloMkdirReply myMkdir(const std::string& path, uint32_t mode) {
 
       HelloDirent request;
       request.set_ino(mode);
       request.set_name(path);
       HelloMkdirReply reply;
 
       ClientContext context;
 
       Status status = stub_->myMkdir(&context, request, &reply);
 
       if (status.ok()) {
         return reply;
       } else {
         //return NULL;
       }
  }
helloworld::HelloMkdirReply myRmdir(const std::string& path) {
 
        HelloRequest request;
        request.set_name(path);
        HelloMkdirReply reply;
 
        ClientContext context;
 
        Status status = stub_->myRmdir(&context, request, &reply);
 
        if (status.ok()) {
          return reply;
        } else {
          //return NULL;
        }
   }
helloworld::HelloMkdirReply myUnlink(const std::string& path) {
 
          HelloRequest request;
          request.set_name(path);
          HelloMkdirReply reply;
 
          ClientContext context;
 
          Status status = stub_->myUnlink(&context, request, &reply);
 
          if (status.ok()) {
            return reply;
          } else {
            //return NULL;
          }
     }
 helloworld::HelloMkdirReply myMknod(const std::string& path, uint32_t mode, uint32_t dev) {
 
       HelloMknodRequest request;
       request.set_mode(mode);
       request.set_path(path);
       request.set_dev(dev);
       HelloMkdirReply reply;
 
       ClientContext context;
 
       Status status = stub_->myMknod(&context, request, &reply);
 
       if (status.ok()) {
         return reply;
       } else {
         //return NULL;
       }
  }



 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


void ExampleFS::AbsPath(char dest[PATH_MAX], const char *path) {
	strcpy(dest, _root);
	strncat(dest, path, PATH_MAX);
	//printf("translated path: %s to %s\n", path, dest);
}

void ExampleFS::setRootDir(const char *path) {
	printf("setting FS root to: %s\n", path);
	_root = path;
}

void ExampleFS::setCacheDir(const char *path) {
     printf("setting Cache dir to:  %s\n",path);
	_cacheDir=path;
}

int ExampleFS::Getattr(const char *path, struct stat *statbuf) {

	std::string t(path);


        std::string tempPath(_cacheDir);
        //std::cout<<tempPath<<std::endl;
        std::string dirPath = tempPath+t;
	char tempDir[1024];
        strcpy(tempDir, dirPath.c_str());
        //printf("file path in get attr%s\n",tempDir);
	struct stat fInfo;
        int b;
        std::fstream f;
        b = stat(tempDir, &fInfo);
        if(b==0){
            printf("File exists\n");
            printf("getattr(%s)\n",tempDir);
	    return RETURN_ERRNO(lstat(tempDir,statbuf));
	}


	printf("getattr(%s)\n",path);
	GreeterClient greeter(
        //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
        grpc::CreateChannel(port, grpc::InsecureCredentials()));
        helloworld::HelloStat reply = greeter.myGetattr(t);
//	printf("in getattr b val is %d\n",reply.flag());
	statbuf->st_dev=reply.dev();
        statbuf->st_ino=reply.ino();
        statbuf->st_mode=reply.mode();
        statbuf->st_nlink=reply.nlink();
        statbuf->st_uid=reply.uid();
        statbuf->st_gid=reply.gid();
        statbuf->st_rdev=reply.rdev();
        statbuf->st_size=reply.size();
        statbuf->st_atime=reply.atime();
        statbuf->st_mtime=reply.mtime();
        statbuf->st_ctime=reply.ctime();
        statbuf->st_blksize=reply.blksize();
        statbuf->st_blocks=reply.blocks();


/*	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	printf("getattr(%s)\n", fullPath);
	//return RETURN_ERRNO(lstat(fullPath, statbuf));
	int b=lstat(fullPath,statbuf);
	printf("Value of b in original is %d\n",b);
	printf("Error no is %d",RETURN_ERRNO(b));
	return RETURN_ERRNO(b);*/
	int r=reply.flag();
	int err=reply.err();
	if(r==0)
		return 0;
	else
		return -err;
}



int ExampleFS::Readlink(const char *path, char *link, size_t size) {
	printf("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(readlink(fullPath, link, size));
}

int ExampleFS::Mknod(const char *path, mode_t mode, dev_t dev) {
/*	printf("mknod(path=%s, mode=%d)\n", path, mode);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	
	//handles creating FIFOs, regular files, etc...
	return RETURN_ERRNO(mknod(fullPath, mode, dev));*/

	printf("mknod(path=%s, mode=%d)\n", path, mode);
 
         std::string t(path);
          GreeterClient greeter(
          grpc::CreateChannel(port, grpc::InsecureCredentials()));
          helloworld::HelloMkdirReply reply = greeter.myMknod(t,(int)mode,(int)dev);
          if(reply.ret()==0)
                  return 0;
          else
                  return -reply.err();

}

int ExampleFS::Mkdir(const char *path, mode_t mode) {
	printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
        std::string t(path);
	GreeterClient greeter(
        //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
        grpc::CreateChannel(port, grpc::InsecureCredentials()));
        helloworld::HelloMkdirReply reply = greeter.myMkdir(t,(int)mode);
	if(reply.ret()==0)
		return 0;
	else
		return -reply.err();
/*	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(mkdir(fullPath, mode));*/

}

int ExampleFS::Unlink(const char *path) {

	printf("unlink(path=%s\n)", path);
         std::string t(path);
          GreeterClient greeter(
          grpc::CreateChannel(port, grpc::InsecureCredentials()));
          helloworld::HelloMkdirReply reply = greeter.myUnlink(t);
          if(reply.ret()==0)
                  return 0;
          else
                  return -reply.err();
 


/*	printf("unlink(path=%s\n)", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(unlink(fullPath));*/
}

int ExampleFS::Rmdir(const char *path) {
	printf("rmkdir(path=%s\n)", path);
       std::string t(path);
       GreeterClient greeter(
     //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
     grpc::CreateChannel(port, grpc::InsecureCredentials()));
     helloworld::HelloMkdirReply reply = greeter.myRmdir(t);
	 if(reply.ret()==0)
                 return 0;
         else
                 return -reply.err();
	
	/*char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(rmdir(fullPath));*/
}

int ExampleFS::Symlink(const char *path, const char *link) {
	printf("symlink(path=%s, link=%s)\n", path, link);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(symlink(fullPath, link));
}

int ExampleFS::Rename(const char *path, const char *newpath) {
	printf("rename(path=%s, newPath=%s)\n", path, newpath);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(rename(fullPath, newpath));
}

int ExampleFS::Link(const char *path, const char *newpath) {
	printf("link(path=%s, newPath=%s)\n", path, newpath);
	char fullPath[PATH_MAX];
	char fullNewPath[PATH_MAX];
	AbsPath(fullPath, path);
	AbsPath(fullNewPath, newpath);
	return RETURN_ERRNO(link(fullPath, fullNewPath));
}

int ExampleFS::Chmod(const char *path, mode_t mode) {
	printf("chmod(path=%s, mode=%d)\n", path, mode);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(chmod(fullPath, mode));
}

int ExampleFS::Chown(const char *path, uid_t uid, gid_t gid) {
	printf("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(chown(fullPath, uid, gid));
}

int ExampleFS::Truncate(const char *path, off_t newSize) {
	printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(truncate(fullPath, newSize));
}

int ExampleFS::Utime(const char *path, struct utimbuf *ubuf) {
	printf("utime(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(utime(fullPath, ubuf));
}

int ExampleFS::Open(const char *path, struct fuse_file_info *fileInfo) {
 
    
     std::string t(path);
 
     std::string tempPath(_cacheDir);
     std::string dirPath = tempPath+t;
     char tempDir[1024];
     strcpy(tempDir, dirPath.c_str());
     struct stat fInfo;
     int b;
     std::fstream f;







 //std::string existPath("/home/schakra/fuse_log");
 std::string existPath(homeLogDir);
     std::string finalExistPath=existPath+path;
     char existDir[1024];
 
     strcpy(existDir, finalExistPath.c_str());
     struct stat logInfo;
     int r;
     r=stat(existDir,&logInfo);
     if(r==0){
	printf("LOG FILE EXISTS!!!! RECOVERING FROM CRASH\n");
        unlink(existDir);
	goto rpc;       
     }
   

    b = stat(tempDir, &fInfo);
    if(b==0){
//	printf("File exists\n");
        fileInfo->fh = open(tempDir, fileInfo->flags);
        return 0;
    }
rpc:
    {
  //     printf("File does not exist, do a grpc call\n");
    GreeterClient greeter(
    //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
    grpc::CreateChannel(port, grpc::InsecureCredentials()));
    std::string reply = greeter.myOpen(t);
    //std::cout << "Greeter received: " << reply << std::endl;
    std::fstream flog;
    //std::string logPath("/home/schakra/fuse_log");
    std::string logPath(homeLogDir);
    std::string finalLogPath=logPath+path;
    char logDir[1024];

    strcpy(logDir, finalLogPath.c_str());

    flog.open(finalLogPath,fstream::out);
    flog<<reply;
    flog.close();
    int fd=open(logDir,fileInfo->flags);
    fsync(fd);
    close(fd);
    f.open(tempDir,fstream::out);
    f<<reply;
    f.close();
    fileInfo->fh = open(tempDir, fileInfo->flags);
    return 0;
    }
}

int ExampleFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
     
     std::string t(path);
    std::string tempPath(_cacheDir);
     std::string dirPath = tempPath+t;
     std::cout<<dirPath<<std::endl;
      char tempDir[1024];
     strcpy(tempDir, dirPath.c_str());
     //printf("File handle in read  is %ld\n",fileInfo->fh);
     printf("read(path=%s, size=%d, offset=%d)\n", tempDir, (int)size, (int)offset);
      int pr=pread(fileInfo->fh, buf, size, offset);   
      //printf("Buf is %s\n",buf);
      return pr;
}

int ExampleFS::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
	printf("write(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
	int pw=pwrite(fileInfo->fh, buf, size, offset);
        return pw;
}

int ExampleFS::Statfs(const char *path, struct statvfs *statInfo) {
	printf("statfs(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(statvfs(fullPath, statInfo));
}

int ExampleFS::Flush(const char *path, struct fuse_file_info *fileInfo) {
	
int len;
     len=strlen(path);
     char temp[len];
     int i;
     for(i=0; i<len; i++)
        temp[i]=path[i+1];
     temp[len]='\0';
     std::string t(temp);
    char buf[4096]={'\0'};
    int pr=pread(fileInfo->fh, buf, 4096, 0);
    if(pr<0)
	return -1;
    std::string content(buf);   

std::string t1(path);
 GreeterClient greeter1(
    grpc::CreateChannel(port, grpc::InsecureCredentials()));
     std::string reply = greeter1.myClose(t1,content);
    std::string tempPath(_cacheDir);
    std::string t2(path);
    std::string fullPath=tempPath+t2;
    char tempDir[1024];
    strcpy(tempDir, fullPath.c_str());
    

    std::string existPath(homeLogDir);
     std::string finalExistPath=existPath+path;
     char existDir[1024];
 
      strcpy(existDir, finalExistPath.c_str());
    int r=unlink(existDir);
    if(r<0){
//	printf("Unlink failed\n");
	return -1;
    }
    int ret=unlink(tempDir);
    if(ret<0){
//	printf("Unlink failed\n");
	return -1;
    }	
    

	return 0;
}

int ExampleFS::Release(const char *path, struct fuse_file_info *fileInfo) {
	int len;
     len=strlen(path);
     char temp[len];
     int i;
     for(i=0; i<len; i++)
        temp[i]=path[i+1];
     temp[len]='\0';
     std::string t(temp);
    char buf[4096]={'\0'};
    int pr=pread(fileInfo->fh, buf, 4096, 0);
    if(pr<0)
        return -1;
    std::string content(buf);

std::string t1(path);
 GreeterClient greeter1(
    grpc::CreateChannel(port, grpc::InsecureCredentials()));
     std::string reply = greeter1.myClose(t1,content);
    std::string tempPath(_cacheDir);
    std::string t2(path);
    std::string fullPath=tempPath+t2;
    char tempDir[1024];
    strcpy(tempDir, fullPath.c_str());


    std::string existPath(homeLogDir);
     std::string finalExistPath=existPath+path;
     char existDir[1024];

      strcpy(existDir, finalExistPath.c_str());
                                                       
	int r=unlink(existDir);
    if(r<0){
  //      printf("Unlink failed\n");
        return -1;
    }
    int ret=unlink(tempDir);
    if(ret<0){
    //    printf("Unlink failed\n");
        return -1;
    }


        return 0;



//	printf("release(path=%s)\n", path);
//	return 0;
}

int ExampleFS::Fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	
	printf("fsync(path=%s, datasync=%d\n", path, datasync);
	if(datasync) {
		//sync data only
		return RETURN_ERRNO(fdatasync(fi->fh));
	} else {
		//sync data + file metadata
		return RETURN_ERRNO(fsync(fi->fh));
	}
}

int ExampleFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
	printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
		path, name, value, (int)size, flags);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
}

int ExampleFS::Getxattr(const char *path, const char *name, char *value, size_t size) {
	printf("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
	//char fullPath[PATH_MAX];
	//AbsPath(fullPath, path);
	//return RETURN_ERRNO(getxattr(fullPath, name, value, size));
	return 0;
}

int ExampleFS::Listxattr(const char *path, char *list, size_t size) {
	printf("listxattr(path=%s, size=%d)\n", path, (int)size);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(llistxattr(fullPath, list, size));
}

int ExampleFS::Removexattr(const char *path, const char *name) {
	printf("removexattry(path=%s, name=%s)\n", path, name);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(lremovexattr(fullPath, name));
}

int ExampleFS::Opendir(const char *path, struct fuse_file_info *fileInfo) {
	
	printf("opendir(path=%s)\n", path);
        std::string t(path);
        GreeterClient greeter(
     	//grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
     	grpc::CreateChannel(port, grpc::InsecureCredentials()));
      	uint64_t reply = greeter.myOpendir(t);

/*	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	DIR *dir = opendir(fullPath);
	fileInfo->fh = (uint64_t)dir;
        printf("Fileinfo->fh is %ld\n",fileInfo->fh);
        
	return NULL == dir ? -errno : 0;*/
	fileInfo->fh=reply;
      //  printf("Fileinfo->fh is %ld\n",fileInfo->fh);
        return NULL == reply ? -errno :0;
}

int ExampleFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
	printf("readdir(path=%s, offset=%d)\n", path, (int)offset);
       // printf("Fileinfo->fh in readdir is %ld\n",fileInfo->fh);
	GreeterClient greeter(
        //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
        grpc::CreateChannel(port, grpc::InsecureCredentials()));
        helloworld::HelloDirent reply = greeter.myReaddir(fileInfo->fh);
	std::cout<<reply.ino()<<std::endl;

/*	DIR *dir = (DIR*)fileInfo->fh;
	struct dirent *de = readdir(dir);
        printf("de is %s",de->d_name);
	if(NULL == de) {
		return -errno;
	} else {
		do {
			if(filler(buf, de->d_name, NULL, 0) != 0) {
				return -ENOMEM;
			}
			printf("de is %s",de->d_name);

		} while(NULL != (de = readdir(dir)));
	}*/

        struct dirent *de = (dirent*)malloc(sizeof(dirent));
	de->d_ino=reply.ino();
        char t[256];
	 strcpy(t, reply.name().c_str());
        strcpy(de->d_name,t);
        printf("de is %s",de->d_name);
       /* if(NULL == de) {
                return -errno;
        } else {*/
	
              while(1) {
                        if(filler(buf, de->d_name, NULL, 0) != 0) {
                                return -ENOMEM;
                        }
                        printf("de is %s",de->d_name);
			reply = greeter.myReaddir(fileInfo->fh);
			de->d_ino=reply.ino();
		        strcpy(t, reply.name().c_str());
		         strcpy(de->d_name,t);
	//		printf("de is %s",de->d_name);
	//		printf("de.ino is %ld",de->d_ino);
			if(strcmp(de->d_name,"Error")==0)
				break;

                } 
        

	return 0;
}

int ExampleFS::Releasedir(const char *path, struct fuse_file_info *fileInfo) {
	printf("releasedir(path=%s)\n", path);
	GreeterClient greeter(
        //grpc::CreateChannel("localhost:50053", grpc::InsecureCredentials()));
        grpc::CreateChannel(port, grpc::InsecureCredentials()));
        std::string reply = greeter.myReleasedir(fileInfo->fh);

	//closedir((DIR*)fileInfo->fh);
	return 0;
}

int ExampleFS::Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
	return 0;
}

int ExampleFS::Init(struct fuse_conn_info *conn) {
	return 0;
}

int ExampleFS::Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
	printf("truncate(path=%s, offset=%d)\n", path, (int)offset);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
}



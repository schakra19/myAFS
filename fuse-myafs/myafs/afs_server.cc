/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include<fstream>
#include <grpc++/grpc++.h>
 #include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include<errno.h>
#include "helloworld.grpc.pb.h"
#include<fcntl.h>
#include<unistd.h>
using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
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
//std::string home("/home/ubuntu/CS739/dir1");
std::string home;


// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
  Status myOpen(ServerContext* context, const HelloRequest* request, HelloReply* reply) override {
//	reply->set_message("Hello");
	ifstream f ;
        std::string temp;
        std::cout << request->name();
        //std::string dirPath("/home/schakra/dir1");
        std::string dirPath(home);
        std::string fullPath=dirPath+request->name();
        f.open(fullPath,fstream::out);
        f.seekg(0,std::ios::end);
        size_t size= f.tellg();
        std::string buffer(size,' ');
        f.seekg(0);
        f.read(&buffer[0],size);
        reply->set_message(buffer);
        return Status::OK;
}
Status myClose(ServerContext* context, const HelloFlushRequest* request, HelloReply* reply) override {
        reply->set_message(request->content());
          //ifstream f ;
          std::string temp;
          std::cout << request->path();
          //std::string dirPath("/home/schakra/dir1");
          std::string dirPath(home);
          std::string fullPath=dirPath+request->path();
          std::ofstream ofs;
          ofs.open(fullPath, std::ofstream::out | std::ofstream::trunc);
 	  ofs << request->content();
          ofs.close();
         /* f.open(fullPath,fstream::out);
          f.seekg(0,std::ios::end);
          size_t size= f.tellg();
          std::string buffer(size,' ');
          f.seekg(0);
          f.read(&buffer[0],size);
          reply->set_message(buffer);*/
          return Status::OK;
  }

 Status myOpendir(ServerContext* context, const HelloRequest* request, HelloOpendir* reply) override {
         //reply->set_message(request->content());
           //ifstream f ;
           //std::string dirPath("/home/schakra/dir1");
           std::string dirPath(home);
           std::string fullPath=dirPath+request->name();
           char tempDir[1024];
           strcpy(tempDir, fullPath.c_str());
           
           DIR *dir = opendir(tempDir);
           //struct dirent *de = readdir(dir);

	   uint64_t d= (uint64_t)dir;
           reply->set_dir(d);
           return Status::OK;
   }

Status myReaddir(ServerContext* context, const HelloDir* request, HelloDirent* reply) override {
 
  		DIR *dir = (DIR*)request->fh();
         struct dirent *de = readdir(dir);
	if(de==NULL){
		reply->set_ino(-1);
		reply->set_name("Error");
       }
	else{
		std::string t(de->d_name);
		reply->set_name(t);
		reply->set_ino(de->d_ino);
	}
            return Status::OK;
    }


 
Status myGetattr(ServerContext* context, const HelloRequest* request, HelloStat* reply) override {
          //reply->set_message(request->content());
            //ifstream f ;
            //std::string dirPath("/home/schakra/dir1");
            std::string dirPath(home);
            std::string fullPath=dirPath+request->name();
	     char tempDir[1024];
            strcpy(tempDir, fullPath.c_str());
	    struct stat *statbuf= (struct stat*)malloc(sizeof(struct stat));
		int b=lstat(tempDir,statbuf);
		printf("%d",errno);
		reply->set_dev(statbuf->st_dev);
		reply->set_ino(statbuf->st_ino);
		reply->set_mode(statbuf->st_mode);
		reply->set_nlink(statbuf->st_nlink);
		reply->set_uid(statbuf->st_uid);
		reply->set_gid(statbuf->st_gid);
		reply->set_rdev(statbuf->st_rdev);
		reply->set_size(statbuf->st_size);
		reply->set_atime(statbuf->st_atime);
		reply->set_mtime(statbuf->st_mtime);
		reply->set_ctime(statbuf->st_ctime);
		reply->set_blksize(statbuf->st_blksize);
		reply->set_blocks(statbuf->st_blocks);
		reply->set_flag(b);
		reply->set_err(errno);

return Status::OK;
    }


Status myReleasedir(ServerContext* context, const HelloDir* request, HelloReply* reply) override {
 
                 DIR *dir = (DIR*)request->fh();
		 closedir(dir);
		 reply->set_message("Success");
		return Status::OK;

}


Status myMkdir(ServerContext* context, const HelloDirent* request, HelloMkdirReply* reply) override {

	//std::string dirPath("/home/schakra/dir1");
	std::string dirPath(home);
        std::string fullPath=dirPath+request->name();
        char tempDir[1024];
        strcpy(tempDir, fullPath.c_str());
	int b=mkdir(tempDir,request->ino());
	reply->set_ret(b);
	reply->set_err(errno);
	return Status::OK;
}

Status myRmdir(ServerContext* context, const HelloRequest* request, HelloMkdirReply* reply) override {
 
         //std::string dirPath("/home/schakra/dir1");
         std::string dirPath(home);
         std::string fullPath=dirPath+request->name();
         char tempDir[1024];
         strcpy(tempDir, fullPath.c_str());
         int b = rmdir(tempDir);
         reply->set_ret(b);
         reply->set_err(errno);
         return Status::OK;
 }

Status myUnlink(ServerContext* context, const HelloRequest* request, HelloMkdirReply* reply) override {
 
           //std::string dirPath("/home/schakra/dir1");
           std::string dirPath(home);
           std::string fullPath=dirPath+request->name();
           char tempDir[1024];
           strcpy(tempDir, fullPath.c_str());
           int b = unlink(tempDir);
           reply->set_ret(b);
           reply->set_err(errno);
           return Status::OK;
   }
 
 Status myMknod(ServerContext* context, const HelloMknodRequest* request, HelloMkdirReply* reply) override {
 
          //std::string dirPath("/home/schakra/dir1");
          std::string dirPath(home);
          std::string fullPath=dirPath+request->path();
          char tempDir[1024];
          strcpy(tempDir, fullPath.c_str());
          int b=mknod(tempDir,request->mode(),request->dev());
          reply->set_ret(b);
          reply->set_err(errno);
          return Status::OK;
  }


};

void RunServer() {
  std::string server_address("10.0.1.112:50055");
  GreeterServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
 if(argc<2)
         printf("Usage ./afs_server directory to mount\n");
  char path[1024];
  strcpy(path,argv[1]);
  std::string t(path);
  home=t;



  RunServer();

  return 0;
}

CXX = g++
CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib -lgrpc++_unsecure -lgrpc -lgpr -lprotobuf -lpthread -ldl -lfuse
CFLAGS = -Wall -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 `pkg-config fuse --cflags`
LINKFLAGS = -Wall `pkg-config fuse --libs`
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

#PROTOS_PATH = /home/schakra/CS739/fuse-2.9.4/myafs/
PROTOS_PATH = ./
vpath %.proto $(PROTOS_PATH)

all: examplefs afs_server

examplefs: helloworld.pb.o helloworld.grpc.pb.o examplefs.o wrap.o main.o
	$(CXX) $^ $(LDFLAGS) -o $@

afs_server: helloworld.pb.o helloworld.grpc.pb.o afs_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

examplefs.o: examplefs.cc examplefs.hh
	$(CXX) -std=c++11 -g $(CFLAGS) $(LDFLAGS) -c examplefs.cc -o examplefs.o

main.o: main.c wrap.hh
	gcc -g $(CFLAGS) -c main.c -o main.o

wrap.o: wrap.cc wrap.hh examplefs.hh
	$(CXX) -std=c++11 -g $(CFLAGS) $(LDFLAGS) -c wrap.cc -o wrap.o

%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
clean: 
	rm -f *.o examplfs afs_server  


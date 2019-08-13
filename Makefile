all:hello.cpp
	clear
	clear
	g++ hello.cpp test.pb.cc -o hello -ggdb -lpthread \
	-I/home/dengmuyang/protobuf_build/include \
	-L/home/dengmuyang/protobuf_build/lib/ -l:libprotobuf.a

all:main.cpp dist_protobuf_mgr.cpp dist_protobuf_template.cpp
	clear
	clear
	g++ main.cpp dist_protobuf_mgr.cpp dist_protobuf_template.cpp ../test.pb.cc ../RedisTool/redis_tool.cpp -o main -std=c++17 -ggdb \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -lprotobuf_7 -levent -lhiredis \
	-I../ \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include \
	-I../RedisTool 

main2:main2.cpp dist_protobuf_mgr.cpp dist_protobuf_template.cpp
	clear
	clear
	g++ main2.cpp dist_protobuf_mgr.cpp dist_protobuf_template.cpp ../test.pb.cc ../RedisTool/redis_tool.cpp -o main2 -std=c++17 -ggdb \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -lprotobuf_7 -levent -lhiredis \
	-I../ \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include \
	-I../RedisTool 

main:main.cpp
	clear
	clear
	g++ main.cpp ../test.pb.cc -o main -std=c++17 -ggdb \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -lprotobuf_7 \
	-I../ \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include
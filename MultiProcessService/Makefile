all:main.cpp
	clear
	clear
	g++ main.cpp ../dmy_common/timer.cpp service_group.cpp ../RedisTool/redis_tool.cpp \
	-o main -std=c++17 -ldl -ggdb \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include \
	-I../RedisTool/ -I../ \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -levent -lhiredis -lprotobuf_7 \
	-I../DistProtobuf -rdynamic
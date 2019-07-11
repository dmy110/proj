all:main.cpp
	clear
	clear
	g++ main.cpp service_main.cpp timer.cpp service_group.cpp service_base.cpp ../RedisTool/redis_tool.o \
	-o main -std=c++17 -ldl -ggdb \
	-I../RedisTool/ -I../ \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -levent -lhiredis
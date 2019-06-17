all:main.cpp distmemory.cpp
	clear
	clear
	g++ main.cpp distmemory.cpp -o main -I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -levent -lhiredis \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include/hiredis \
	-std=c++17 -lpthread \
	-I/home/dengmuyang/proj/distmemory/ -ggdb
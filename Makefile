all:hello.cpp
	g++ hello.cpp -o hello -I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include \
	-L/home/dengmuyang/dolphin_server/thirdpart_dolphin/lib -levent -lhiredis \
	-I/home/dengmuyang/dolphin_server/thirdpart_dolphin/include/hiredis
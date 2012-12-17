cc = gcc
flags = -Wall

all: simplescan server


simplescan: simplescan.c
	$(cc) -o simplescan simplescan.c -lbluetooth $(flags)

server: server.c
	$(cc) -o server server.c -lbluetooth $(flags)


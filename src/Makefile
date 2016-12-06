CC       = gcc
MYCFLAGS = -O3 -Wall

all: partinfo
	make -C mouseinfo all

partinfo: partinfo.c
	$(CC) $(MYCFLAGS) $^ -o $@ -lparted

clean :
	make -C mouseinfo clean

CFLAGS= -std=c99 -Wall -Wextra -pedantic
hinfosvc: hinfosvc.c
	gcc $(CFLAGS) hinfosvc.c -o hinfosvc

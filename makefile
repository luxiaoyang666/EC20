CC = gcc

comport: comport.c ComportOpen.o ComportInit.o
		$(CC) comport.c ComportOpen.o ComportInit.o -o comport

ComportOpen.o: ComportOpen.c
		$(CC) -c ComportOpen.c 

ComportInit.o: ComportInit.c
		$(CC) -c ComportInit.c

clear:
		rm -rf *.o comport


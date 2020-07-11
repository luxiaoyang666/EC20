(CC) = gcc

comport: comport.c comport_main.o
	$(CC) comport.c comport_main.o -o comport -Werror -Wall

comport_main.o: comport_main.c
	$(CC) -c comport_main.c

clear:
	rm *.o comport

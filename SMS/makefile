(CC) = gcc

sms: comport.o main.c PDU.o
	$(CC) main.c comport.o PDU.o -o sms -Werror -Wall

comport.o: comport.c
	$(CC) -c comport.c

PUD.o: PUD.c
	$(CC) -c PDU.c

clear:
	rm *.o sms

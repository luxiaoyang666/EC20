(CC) = gcc

sms: sms_main.c comport.o PDU.o sms.o 
	$(CC) sms_main.c comport.o PDU.o sms.o -o sms -Wall -Werror

comport.o: comport.c
	$(CC) -c comport.c

PUD.o: PUD.c
	$(CC) -c PDU.c

sms.o: sms.c
	$(CC) -c sms.c

clear:
	rm *.o sms

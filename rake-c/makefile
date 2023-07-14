C99 = cc -std=c99
CFLAGS = -Wall -Werror -pedantic

rake-c:rake-c.o strsplit.o data_structure.o
	$(C99) $(CFLAGS) -o rake-c rake-c.o strsplit.o data_structure.o

rake-c.o:rake-c.c client.h 
	$(C99) $(CFLAGS) -c rake-c.c client.h

strsplit.o:strsplit.c
	$(C99) $(CFLAGS) -c strsplit.c

data_structure.o:data_structure.c client.h
	$(C99) $(CFLAGS) -c data_structure.c client.h
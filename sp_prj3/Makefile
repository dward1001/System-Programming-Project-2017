#-----------------------------------------------------

SOURCES = 20141550.c list_func.c assembler.c loader.c
OBJECTS = $(SOURCES:.c=.o) -lm
MYPROGRAM = 20141550.out

CC = gcc
CFLAG = -Wall -g
#-----------------------------------------------------

all: $(SOURCES) $(MYPROGRAM)

$(MYPROGRAM) : $(OBJECTS)
		$(CC) $(CFLAG) -o $@ $(OBJECTS)

clean :
		rm -f *.o $(MYPROGRAM) *.lst *.out

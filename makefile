# Makefile para compilar o programa Comissao

CC = gcc
CFLAGS = -Wall -O2
TARGET = comissao

TESTE = teste.txt

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o lib.o

main.o: main.c lib.o
	$(CC) $(CFLAGS) -c main.c 

lib.o: lib.h
	$(CC) $(CFLAGS) -c lib.c

clean:
	rm -f *.o $(TARGET) *.txt

teste: $(TARGET) $(TESTE) 
	./$(TARGET) < $(TESTE)

$(TESTE): gera_teste.py
	python3 gera_teste.py > $(TESTE) 
# Makefile para compilar o programa Comissao

CC = gcc
CFLAGS = -Wall -O2
TARGET = comissao

TESTE = teste.txt

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o $(TARGET)

teste: $(TARGET) $(TESTE) 
	./$(TARGET) < $(TESTE)

$(TESTE): gera_teste.py
	python3 gera_teste.py > $(TESTE) 
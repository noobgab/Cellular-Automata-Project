OBJECTS = main.o Thread.o

main: ${OBJECTS} func.h
	gcc ${OBJECTS} -o main -lpthread
	
main.o: main.c func.h
	gcc -c main.c
	
Thread.o: Thread.c func.h
	gcc -c Thread.c
	
clean:
	rm -f *.o
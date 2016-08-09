main: Lab2IO.c Lab2IO.h timer.h
	gcc -Wall main.c Lab2IO.c -o main -lpthread
	
clean:
	rm main
	
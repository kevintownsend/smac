all: main

main:
	g++ -o main main.cpp

run: main
	main

vim:
	vim -p makefile main.cpp

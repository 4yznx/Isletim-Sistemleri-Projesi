#/**
 
#* Makefile dosyasi
 
#* @group -> 19
 
#* @author B211210572 -> Yazan Alsolyman
#* @author B211210584 -> Arif Muhammed
#* @author B211210575 -> Ahmed Mohamed
#* @author G211210564 -> Muhammed Elzabi
#* @author G211210036 -> Abdullah Bünyamin Adsever

#*/

all: derle calistir

derle:
	gcc -g -I./include ./src/main.c ./src/handler.c -o ./bin/program
	gcc -g -I./include ./src/increment.c -o ./bin/increment

calistir:
	./bin/program
/**
 * @file  shell.h
 * @author B211210572 -> Yazan Alsolyman
 * @author B211210584 -> Arif Muhammed
 * @author B211210575 -> Ahmed Mohamed
 * @author G211210564 -> Muhammed Elzabi
 * @author G211210036 -> Abdullah Bünyamin Adsever
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Global değişken
extern int arkaPlandaCalisanProsesSayisi;

// Sinyal handler
void handler(int signum);

#define KOMUT_BOYUTU 1024

#endif
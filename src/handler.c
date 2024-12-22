/**
 * @file  handler.c
 * @author B211210572 -> Yazan Alsolyman
 * @author B211210584 -> Arif Muhammed
 * @author B211210575 -> Ahmed Mohamed
 * @author G211210564 -> Muhammed Elzabi
 * @author G211210036 -> Abdullah Bünyamin Adsever
 */

#include "shell.h"

int arkaPlandaCalisanProsesSayisi = 0;

void handler(int signum)
{
    int wStatus;
    int pid;
    while((pid = waitpid(-1, &wStatus, WNOHANG)) > 0)
    {
        // Process'in geri dönüş değerini alır
        int ret = WEXITSTATUS(wStatus);
        printf("[%d] retval: %d\n", pid, ret);
        fflush(stdout);
        // Arkaplan'da calisan process durdurulduğuna göre değeri bir azaltılabilir.
        arkaPlandaCalisanProsesSayisi--;
    }
    return;
}
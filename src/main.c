/**
 * @file  main.c
 * @author B211210572 -> Yazan Alsolyman
 * @author B211210584 -> Arif Muhammed
 * @author B211210575 -> Ahmed Mohamed
 * @author G211210564 -> Muhammed Elzabi
 * @author G211210036 -> Abdullah Bünyamin Adsever
 */

#include "shell.h"

int main()
{
    // Kullanıcıdan girdi alınır
    char girdi[KOMUT_BOYUTU];
    char* komut_parcasi;
    char* komut_parcalar[3][16];
    char giriseYonlendirilenDosya[64];
    int giriseYonlendirmefd;
    int girisYonlendir;
    char cikisaYonlendirilenDosya[64];
    int cikisaYonlendirmefd;
    int cikisYonlendir;
    int pipeVarMi;
    int pipeSayisi;
    int hata;
    int arkaPlandaCalistir;
    int wStatus;

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;

    sigaction(SIGCHLD, &act, NULL);

    int program_devam = 1;
    while(program_devam)
    {
        // Komut parse edilir
        girisYonlendir = 0;
        cikisYonlendir = 0;
        pipeVarMi = 0;
        pipeSayisi = 0;
        hata = 0;
        arkaPlandaCalistir = 0;
        printf("\n> ");
        fflush(stdout); // prompt'ın hemen yazılmasından emin olmak için
        if (fgets(girdi, KOMUT_BOYUTU, stdin) == NULL) {
            program_devam = 0;
            break;
        }
        
        // Alınan girdinin sonundaki alt satır karakteri kaldırılır
        for(int i = 0; i < strlen(girdi); i++)
        {
            if(girdi[i] == '\n')
            {
                girdi[i] = '\0';
            }
        }

        // Eğer girilen komut quit ise, programdan çık
        if(strcmp(girdi, "quit") == 0)
        {
            program_devam = 0;
            while(arkaPlandaCalisanProsesSayisi > 0);
            break;
        }

        // Komutları ; ile ayır
        char *komutlar[100];
        int komutSayisi = 0;

        komutlar[komutSayisi] = strtok(girdi, ";");
        while (komutlar[komutSayisi] != NULL) {
            komutSayisi++;
            komutlar[komutSayisi] = strtok(NULL, ";");
        }

        // Her komutu sırayla işleme
        for (int k = 0; k < komutSayisi; k++) {
            char *komut = komutlar[k];
            int i = 0;
            int komutNo = 0;

            // Komut parse edilir
            for(komut_parcasi = strtok(komut, " "); komut_parcasi != NULL; komut_parcasi = strtok(NULL, " "))
            {
                if(strcmp(komut_parcasi, "<") == 0)
                {
                    // < işaretinden sonraki yazıyı girişe yönlendirilecek olan dosya olarak kabul edilir
                    strcpy(giriseYonlendirilenDosya, strtok(NULL, " "));
                    // Girişe yönlendirilecek olan dosya okuma modunda açılır
                    giriseYonlendirmefd = open(giriseYonlendirilenDosya, O_RDONLY);
                    if(giriseYonlendirmefd >= 0)
                    {
                        girisYonlendir = 1;
                    }
                    else
                    {
                        printf("%s giriş dosyası bulunamadı\n", giriseYonlendirilenDosya);
                        hata = 1;
                    }
                    continue;
                }
                if(strcmp(komut_parcasi, ">") == 0)
                {
                    // > işaretinden sonraki yazıyı çıkışa yönlendirilecek olan dosya olarak kabul edilir
                    strcpy(cikisaYonlendirilenDosya, strtok(NULL, " "));
                    // Çıkışa yönlendirilecek olan dosya yazma ve yoksa oluşturma modunda açılır
                    cikisaYonlendirmefd = open(cikisaYonlendirilenDosya, O_WRONLY | O_CREAT, 0666);
                    if(cikisaYonlendirmefd >= 0)
                        cikisYonlendir = 1;
                    continue;
                }
                if(strcmp(komut_parcasi, "&") == 0)
                {
                    // Arka planda çalıştırma
                    arkaPlandaCalistir = 1;
                    continue;
                }
                if(strcmp(komut_parcasi, "|") == 0)
                {
                    // Boru işlemi
                    komut_parcalar[komutNo][i] = '\0';
                    pipeVarMi = 1;
                    pipeSayisi++;
                    komutNo++;
                    i = 0;
                    continue;
                }

                komut_parcalar[komutNo][i] = komut_parcasi;
                i++;
            }
            komut_parcalar[komutNo][i] = '\0';

            // Çocuk proses oluşturulur
            if(pipeVarMi == 0)
            {
                int forkDonus = fork();
                if(forkDonus < 0)
                {
                    fprintf(stderr, "fork yapılamadı\n");
                }
                else if(forkDonus > 0)    // Ebeveyn proses
                {
                    // Çocuğun dönmesini bekler
                    if(arkaPlandaCalistir == 0)
                    {
                        waitpid(forkDonus, &wStatus, 0);
                    }
                    else if(arkaPlandaCalistir == 1)
                    {
                        arkaPlandaCalisanProsesSayisi++;
                        arkaPlandaCalistir = 0;
                    }
                }
                else if(forkDonus == 0)    // Çocuk proses
                {
                    if(komut_parcalar[0][0] != NULL && hata != 1)
                    {
                        if(girisYonlendir == 1)
                            dup2(giriseYonlendirmefd, 0);
                        if(cikisYonlendir == 1)
                            dup2(cikisaYonlendirmefd, 1);
                        execvp(komut_parcalar[0][0], komut_parcalar[0]);
                        perror("exec_hata");
                    }
                    exit(1);
                }
            }
            else if(pipeVarMi == 1)
            {
                // Pipe işlemi yapılacaksa
                int fdler[2*pipeSayisi];
                for(int i = 0; i < pipeSayisi; i++)
                {
                    pipe(&fdler[i*2]);
                }

                int komutSayisiPipe = pipeSayisi + 1;
                if(komutSayisiPipe == 2)
                {
                    int forkDonus = fork();
                    if(forkDonus == 0)
                    {
                        dup2(fdler[1], 1);
                        close(fdler[0]);
                        execvp(komut_parcalar[0][0], komut_parcalar[0]);
                    }
                    else
                    {
                        int forkDonus2 = fork();
                        if(forkDonus2 == 0)
                        {
                            dup2(fdler[0], 0);
                            close(fdler[1]);
                            execvp(komut_parcalar[1][0], komut_parcalar[1]);
                        }
                    }
                    close(fdler[0]);
                    close(fdler[1]);
                    wait(NULL);
                    wait(NULL);
                }
                else if(komutSayisiPipe == 3)
                {
                    int forkDonus = fork();
                    if(forkDonus == 0)
                    {
                        dup2(fdler[1], 1);
                        for(int i = 0; i < pipeSayisi*2; i++)
                        {
                            close(fdler[i]);
                        }
                        execvp(komut_parcalar[0][0], komut_parcalar[0]);
                    }
                    else
                    {
                        int forkDonus2 = fork();
                        if(forkDonus2 == 0)
                        {
                            dup2(fdler[0], 0);
                            dup2(fdler[3], 1);
                            for(int i = 0; i < pipeSayisi*2; i++)
                            {
                                close(fdler[i]);
                            }
                            execvp(komut_parcalar[1][0], komut_parcalar[1]);
                        }
                        else
                        {
                            int forkDonus3 = fork();
                            if(forkDonus3 == 0)
                            {
                                dup2(fdler[2], 0);
                                for(int i = 0; i < pipeSayisi*2; i++)
                                {
                                    close(fdler[i]);
                                }
                                execvp(komut_parcalar[2][0], komut_parcalar[2]);
                            }
                        }
                    }
                    for(int i = 0; i < pipeSayisi*2; i++)
                    {
                        close(fdler[i]);
                    }
                    for(int i = 0; i < komutSayisiPipe; i++)
                    {
                        wait(NULL);
                    }
                }
            }
        }
    }
}
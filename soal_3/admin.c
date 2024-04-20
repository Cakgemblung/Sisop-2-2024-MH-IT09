#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_PROSES 1024

char nama_file_log[50];
pid_t proses_pengguna[MAX_PROSES];
int jumlah_proses = 0;
int mode_monitor = 0;
int mode_kontrol = 0;

char* ambil_waktu_sekarang() {
    time_t sekarang;
    struct tm* lokal;
    static char buffer[20];

    sekarang = time(NULL);
    lokal = localtime(&sekarang);
    strftime(buffer, sizeof(buffer), "%d:%m:%Y-%H:%M:%S", lokal);

    return buffer;
}

void log_proses(pid_t pid, char* nama, char* status) {
    FILE* fp = fopen(nama_file_log, "a");
    if (fp == NULL) {
        perror("Gagal membuka file log");
        return;
    }

    fprintf(fp, "[%s]_%d-%s-%s\n", ambil_waktu_sekarang(), pid, nama, status);
    fclose(fp);
}

void monitor_pengguna(char* pengguna) {
    sprintf(nama_file_log, "%s.log", pengguna);
    mode_monitor = 1;

    while (mode_monitor) {
        char perintah[100];
        sprintf(perintah, "ps -u %s", pengguna);

        FILE* pipa = popen(perintah, "r");
        if (pipa == NULL) {
            perror("Gagal membuka pipa");
            return;
        }

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipa) != NULL) {
            char* token = strtok(buffer, " ");
            pid_t pid = atoi(token);
            token = strtok(NULL, "\n");
            char* nama = token;

            if (pid != 0 && nama != NULL) {
                int ditemukan = 0;
                for (int i = 0; i < jumlah_proses; i++) {
                    if (proses_pengguna[i] == pid) {
                        ditemukan = 1;
                        break;
                    }
                }

                if (!ditemukan) {
                    proses_pengguna[jumlah_proses++] = pid;
                    log_proses(pid, nama, "JALAN");
                }
            }
        }

        pclose(pipa);
        sleep(1);
    }
}

void hentikan_monitor(char* pengguna) {
    mode_monitor = 0;
    printf("Monitoring dihentikan untuk pengguna: %s\n", pengguna);
}

void kontrol_pengguna(char* pengguna) {
    mode_kontrol = 1;

    while (mode_kontrol) {
        for (int i = 0; i < jumlah_proses; i++) {
            pid_t pid = proses_pengguna[i];
            if (kill(pid, SIGSTOP) == -1) {
                log_proses(pid, "Tidak Diketahui", "GAGAL");
                proses_pengguna[i] = proses_pengguna[--jumlah_proses];
            } else {
                log_proses(pid, "Tidak Diketahui", "GAGAL");
            }
        }

        sleep(1);
    }
}

void hentikan_kontrol(char* pengguna) {
    mode_kontrol = 0;
    printf("Kontrol dihentikan untuk pengguna: %s\n", pengguna);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Penggunaan: %s <opsi> <pengguna>\n", argv[0]);
        return 1;
    }

    char* opsi = argv[1];
    char* pengguna = argv[2];

    if (strcmp(opsi, "-m") == 0) {
        monitor_pengguna(pengguna);
    } else if (strcmp(opsi, "-s") == 0) {
        hentikan_monitor(pengguna);
    } else if (strcmp(opsi, "-c") == 0) {
        kontrol_pengguna(pengguna);
    } else if (strcmp(opsi, "-a") == 0) {
        hentikan_kontrol(pengguna);
    } else {
        fprintf(stderr, "Opsi tidak valid\n");
        return 1;
    }

    return 0;
}

# Sisop-2-2024-MH-IT09

| Nama          | NRP          |
| ------------- | ------------ |
| Kevin Anugerah Faza | 5027231027 |
| Muhammad Hildan Adiwena | 5027231077 |
| Nayyara Ashila | 5027231083 |

## Soal 1
Berikut adalah code yang kami buat untuk mengerjakan soal 1
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#define MAXBUFF_SIZE 1024

void replace_string(char *str, const char *old, const char *new) {
    char buffer[MAXBUFF_SIZE];
    char *p;

    while((p = strstr(str, old)) != NULL) {
        strncpy(buffer, str, p-str);
        buffer[p-str] = '\0';
        sprintf(buffer+(p-str), "%s%s", new, p+strlen(old));
        strcpy(str, buffer);
    }
}
```
Saya mulai mendeklarasikan library yang diperlukan oleh code ini. Kemudian dilanjutkan oleh fungsi `replace_string` yang digunakan untuk melakukan operasi pergantian kata sesuai dengan apa yang diminta nantinya. Fungsi ini menggunakan `strstr` untuk menemukan kemunculan pertama dari `old` dan `str` dan jika ditemukan fungsi mengganti `old` dengan `new`. Hasil akhir disimpan dalam `str`.
```
void procfile(const char *dirpath, const char *filename) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, filename);

    FILE *fp = fopen(filepath, "r");
    if(fp == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[MAXBUFF_SIZE];
    char temp[MAXBUFF_SIZE] = "";
    while(fgets(buffer, MAXBUFF_SIZE, fp) != NULL) {
        if(strstr(buffer, "m4LwAr3") || strstr(buffer, "5pYw4R3") || strstr(buffer, "R4nS0mWaR3")) {
            replace_string(buffer, "m4LwAr3", "[MALWARE]");
            replace_string(buffer, "5pYw4R3", "[SPYWARE]");
            replace_string(buffer, "R4nS0mWaR3", "[RANSOMWARE]");
            strcat(temp, buffer);

            char logpath[1024];
            snprintf(logpath, sizeof(logpath), "%s/virus.log", dirpath);
            FILE *log_fp = fopen(logpath, "a");
            if(log_fp != NULL) {
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                fprintf(log_fp, "[%02d-%02d-%04d][%02d:%02d:%02d] Suspicious string at %s successfully replaced!\n",
                        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, filepath);
                fclose(log_fp);
            }
        } else {
            strcat(temp, buffer);
        }
    }
    fclose(fp);

    fp = fopen(filepath, "w");
    if(fp != NULL) {
        fputs(temp, fp);
        fclose(fp);
    } else {
        perror("Error opening file for writing");
    }
}
```
Selanjutnya saya membuat fungsi `procfile` yang digunakan untuk mengganti string tertentu dan mencatat perubahannya dalam file `virus.log` dengan format: `[dd-mm-YYYY][HH:MM:SS] Suspicious string at <file_name> successfully replaced!` sesuai dengan yang diminta oleh soal. 
```
void procdir(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if(dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_REG) {
            if(strcmp(entry->d_name, "virus.c") != 0) {
                procfile(dirpath, entry->d_name);
            }
        }
    }

    closedir(dir);
}
```
fungsi `procdir` ini digunakan untuk mengakses dan memproses semua file dalam direktori dengan memanggil `procfile`.
```
int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if(pid < 0) {
        perror("Error forking");
        return 1;
    } else if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    pid_t sid = setsid();
    if(sid < 0) {
        perror("Error creating new session");
        return 1;
    }

    if(chdir("/") < 0) {
        perror("Error changing directory");
        return 1;
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1) {
        procdir(argv[1]);
        sleep(15);
    }

    return 0;
}
```
Fungsi `main` disini berguna untuk mengakses dan menjalankan seluruh fungsi yang telah dibuat sebelumnya. Ia akan memeriksa jumlah argumen dimana jika tidak ada 2 argumen maka akan muncul pesan error. Kemudian setting `sleep` diatur 15 detik sesuai dengan apa yang diminta oleh soal.

Berikut ini adalah hasil output dari code kami.

isi file sebelum dijalankan
![image](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/47d99551-47c5-4b3a-a407-37e8e9cad024)

isi file setelah dijalankan
![image](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/dc676f05-9ce8-4b49-8df1-3dc21d02898a)


Isi virus.log
![Screenshot 2024-04-27 013030](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/c905aeb7-b2b7-4281-bd84-da50bf11d335)


## Soal 4
Berikut ini adalah code dari kami untuk mengerjakan soal nomor 4
```
void openapps(int argc, char *argv[]) {
    FILE *pid_file = fopen("pids.txt", "a");
    if (pid_file == NULL) {
        printf("Could not open pid file\n");
        return;
    }

    for (int i = 2; i < argc; i += 2) {
        char *app = argv[i];
        int num_windows = atoi(argv[i + 1]);

        for (int j = 0; j < num_windows; j++) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {app, NULL};
                execvp(args[0], args);
                exit(0);
            } else {
                fprintf(pid_file, "%d\n", pid);
            }
        }
    }

    fclose(pid_file);
}
```
Fungsi `openapps` akan digunakan untuk membuka aplikasi dengan jumlah jendela yang telah ditentukan. Fungsi akan membuat file `pids.txt` yang digunakan untuk mencatat pid dari setiap aplikasi yang dijalankan oleh fungsi ini.
```
void killaps() {
    FILE *pid_file = fopen("pids.txt", "r");
    if (pid_file == NULL) {
        printf("Could not open pid file\n");
        return;
    }

    pid_t pid;
    while (fscanf(pid_file, "%d", &pid) != EOF) {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
    }

    fclose(pid_file);
}
```
Fungsi `killaps` digunakan untuk menghentikan semua aplikasi yang dijalankan oleh command `./setup -o`. Ia akan melakukan pengecekan PID pada file  `pids.txt` dan mengirim sinyal `SIGKILL` untuk menghentikan proses.
```
void openfile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }

    char pid_filename[100];
    sprintf(pid_filename, "%s_pids.txt", filename);  // Ubah format nama file di sini
    FILE *pid_file = fopen(pid_filename, "w");
    if (pid_file == NULL) {
        printf("Could not open pid file\n");
        return;
    }

    char app[100];
    int num_windows;
    while (fscanf(file, "%s %d", app, &num_windows) != EOF) {
        for (int j = 0; j < num_windows; j++) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {app, NULL};
                execvp(args[0], args);
                exit(0);
            } else {
                fprintf(pid_file, "%d\n", pid);
            }
        }
    }

    fclose(file);
    fclose(pid_file);
}
```
Fungsi `openfile` digunakan untuk menjalankan command `./setup -f file.conf` sesuai dengan yang diminta oleh soal. Fungsi akan membuka `file.conf` dan membaca isinya untuk dijalankan. PID dari aplikasi yang dijalankan akan ditaruh pada file bernama `file.conf_pids.txt`.
```
void killfile(char *filename) {
    char pid_filename[100];
    sprintf(pid_filename, "%s_pids.txt", filename);  // Ubah format nama file di sini
    FILE *pid_file = fopen(pid_filename, "r");
    if (pid_file == NULL) {
        printf("Could not open pid file\n");
        return;
    }

    pid_t pid;
    while (fscanf(pid_file, "%d", &pid) != EOF) {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
    }

    fclose(pid_file);
}
```
Pada fungsi `killfile` ini, fungsi akan membuka file `file.conf_pids.txt` untuk kemudian membaca PID dari program atau aplikasi yang dijalankan sesuai dengan `file.conf`. Konsepnya sama dengan fungsi `kilaps`.
```
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2>.....<appN> <numN>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        openapps(argc, argv);
    } else if (strcmp(argv[1], "-f") == 0 && argc == 3) {
        openfile(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 3) {
            killfile(argv[2]);
        } else {
            killaps();
        }
    } else {
        printf("Invalid option\n");
    }

    return 0;
}
```
Pada fungsi `main` ini ia akan memeriksa input argumen apakah sesuai atau tidak. Kemudian melakukan opsi yang diberikan (-o, -f, atau -k) dan memanggil fungsi yang sesuai.
Opsi `-o`: Membuka aplikasi-aplikasi berdasarkan argumen.
Opsi `-f`: Membuka aplikasi-aplikasi berdasarkan file konfigurasi.
Opsi `-k`: Menghentikan aplikasi-aplikasi berdasarkan argumen atau file konfigurasi.

Berikut adalah hasil atau output dari code kami

![image](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/21cfa219-96b4-4038-8d95-cca304e70431)
![image](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/ce2de679-79cc-4dd0-a222-9fd15546d316)

isi `file.conf_pids.txt`

![Screenshot 2024-04-27 012415](https://github.com/Cakgemblung/Sisop-2-2024-MH-IT09/assets/144968322/0c222486-3ba1-467a-8244-681330c64734)





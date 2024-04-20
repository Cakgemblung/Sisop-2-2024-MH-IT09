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

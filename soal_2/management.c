#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void log_action(char *username, char *filename, char *action);
void encrypt_rot3(char *str);
void decrypt_rot3(char *str);
void remove_file(char *filename);
void handle_files();
void backup_files();
void restore_files();
void signal_handler(int signum);

void encrypt_rot3(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = ((str[i] - 'a' + 3) % 26) + 'a';
        } else if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = ((str[i] - 'A' + 3) % 26) + 'A';
        }
        i++;
    }
}

void decrypt_rot3(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = ((str[i] - 'a' - 3 + 26) % 26) + 'a';
        } else if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = ((str[i] - 'A' - 3 + 26) % 26) + 'A';
        }
        i++;
    }
}

void remove_file(char *filename) {
    if (remove(filename) == 0) {
        printf("File %s successfully deleted.\n", filename);
    } else {
        printf("Error deleting file %s\n", filename);
    }
}

void handle_files() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/home/nayyara/kedua/library/");
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            char *filename = entry->d_name;
            char decrypted_filename[256];

        
            strcpy(decrypted_filename, filename);
            decrypt_rot3(decrypted_filename);

            
            if (strstr(decrypted_filename, "d3Let3") != NULL) {
            
                remove_file(filename);
            } else if (strstr(decrypted_filename, "r3N4mE") != NULL) {
                
                char *file_ext = strrchr(filename, '.');
                if (file_ext != NULL) {
                    if (strcmp(file_ext, ".ts") == 0) {
                        rename(filename, "helper.ts");
                    } else if (strcmp(file_ext, ".py") == 0) {
                        rename(filename, "calculator.py");
                    } else if (strcmp(file_ext, ".go") == 0) {
                        rename(filename, "server.go");
                    } else {
                        rename(filename, "renamed.file");
                    }
                }
            }
        }
    }

    closedir(dir);
}

void backup_files() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/home/nayyara/kedua/library/"); 
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            char *filename = entry->d_name;
            char decrypted_filename[256];

            strcpy(decrypted_filename, filename);
            decrypt_rot3(decrypted_filename);

            if (strstr(decrypted_filename, "m0V3") != NULL) {
                char backup_path[256];
                sprintf(backup_path, "/home/nayyara/kedua/backup/%s", filename);
                rename(filename, backup_path);
                log_action("paul", filename, "Successfully moved to backup.");
            }
        }
    }

    closedir(dir);
}

void restore_files() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/home/nayyara/kedua/library/backup/"); 
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            char *filename = entry->d_name;
            char decrypted_filename[256];

            strcpy(decrypted_filename, filename);
            decrypt_rot3(decrypted_filename);

            char restored_path[256];
            sprintf(restored_path, "%s", filename);
            char backup_path[256];
            sprintf(backup_path, "/home/nayyara/kedua/backup/%s", filename);
            rename(backup_path, restored_path);
            log_action("paul", filename, "Successfully restored from backup.");
        }
    }

    closedir(dir);
}

void log_action(char *username, char *filename, char *action) {
    FILE *logFile = fopen("/home/nayyara/kedua/history.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

    fprintf(logFile, "[%s][%s] - %s - %s\n", username, timestamp, filename, action);
    fclose(logFile);
}

void signal_handler(int signum) {
    char *mode;
    if (signum == SIGRTMIN) {
        mode = "default";
    } else if (signum == SIGUSR1) {
        mode = "backup";
        backup_files(); 
    } else if (signum == SIGUSR2) {
        mode = "restore";
        restore_files(); 
    }

    log_action("daemon", "daemon", mode);
}

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error forking");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *download_url = "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup";
        char *zip_file = "/home/nayyara/kedua/file.zip";
        execlp("wget", "wget", "--content-disposition","-O", zip_file, download_url, NULL);
        perror("Error running wget");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Download successful.\n");
        } else {
            printf("Download failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    pid = fork();
    if (pid < 0) {
        perror("Error forking");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("unzip", "unzip", "/home/nayyara/kedua/file.zip", "-d", "/home/nayyara/kedua/", NULL);
        perror("Error running unzip");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Extraction successful.\n");
        } else {
            printf("Extraction failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    handle_files();

    signal(SIGRTMIN, signal_handler); 
    signal(SIGUSR1, signal_handler); 
    signal(SIGUSR2, signal_handler); 

    while (1) {
        // Daemon actions here
        sleep(1); // Sleep for 1 second
    }

    return 0;
}

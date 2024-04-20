#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_APPS 100

pid_t pids[MAX_APPS];
int pid_index = 0;

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

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int value[1] = {5};

int main() {
    pid_t pid;

    pid = fork();

    if (pid == 0) {
        value[0] += 15;
        printf("CHILD: value = %d", value[0]);
        return 0;
    }

    else if (pid > 0) {
        wait(NULL);
        printf("PARENT: vlue = %d", value[0]);
        return 0;
    }
}
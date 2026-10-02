#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void main(void)
{
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());
    printf("UID  : %d\n", getuid());
    printf("GID  : %d\n", getgid());

    /* Boucle infinie */
    while (1) {
        sleep(1);
    }
}
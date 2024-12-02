#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signo);

int main()
{
    struct sigaction act;
    int i = 0;
    act.sa_handler = handler;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);

    printf("SIGINT on\n");
    while (1)
    {
        sleep(1);
        printf("sllep for %d sec.\n", ++i);
    }
}

void handler(int signo)
{
    printf("handler : signo = %d\n",signo);
    exit(0);
}
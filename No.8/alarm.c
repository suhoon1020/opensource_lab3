#include <stdio.h>
#include <signal.h>

void alarm_handler(int);
int alarm_flag=0;

int main(){
    struct sigaction act;

    act.sa_handler = alarm_handler;
    sigaction(SIGALRM,&act,NULL);

    alarm(5);
    pause();
    if(alarm_flag)
        printf("5초 지남\n");
}

void alarm_handler(int sig){
    printf("알람시그널 받음\n");
    alarm_flag=1;
}
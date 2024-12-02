#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
int main(){
    pid_t pid;
    int status, exit_status;

    if((pid=fork())<0){
        perror("fork failed");
    }

    if(pid==0){
        sleep(4);
        exit(5);
    }

    if((pid=wait(&status))==-1){
        perror("wait failed");
        exit(2);
    }

    if(WIFEXITED(status)){
        exit_status = WEXITSTATUS(status);
        printf("exit status from %d was %d\n",pid,exit_status);
    }
    exit(0);
}
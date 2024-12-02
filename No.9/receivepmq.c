#include <sys/types.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 16
#define QNAME "/my_queue"
#define PRIORITY 1

char recv_data[BUFSIZE];

int main(){
    mqd_t qd;
    struct mq_attr q_attr,old_q_attr;

    int prio;
    char buf[BUFSIZE];

    q_attr.mq_maxmsg=10;
    q_attr.mq_msgsize=BUFSIZE;

    if((qd=mq_open(QNAME,O_RDWR|O_NONBLOCK,0600,NULL))==-1){
        perror("open fail");
        exit(1);
    }

    q_attr.mq_flags=0; //o_nonblock

    if(mq_setattr(qd,&q_attr,NULL)){
        perror("setattr fail");
        exit(1);
    }

    if(mq_getattr(qd,&old_q_attr)){
        perror("getattr fail");
        exit(1);
    }

    if(!(old_q_attr.mq_flags & O_NONBLOCK)){
        printf("O_NONBLOCk not set \n");
    }

    if(mq_receive(qd,recv_data,BUFSIZE,&prio)==-1){
        perror("send fail");
        exit(1);
    }

    printf("received from message queue : %s, prio : %d\n",recv_data,prio);

    if(mq_close(qd)==-1){
        perror("close fail");
        exit(1);
    }

    if(mq_unlink(QNAME)==-1){
        perror("unlink fail");
        exit(1);
    }
}
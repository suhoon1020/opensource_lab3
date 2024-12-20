#include <sys/types.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 16
#define QNAME "/my_queue"
#define PRIORITY 1

char send_data[] = "Hello world";

int main(){
    mqd_t qd;
    struct mq_attr q_attr;

    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = BUFSIZE;

    if((qd=mq_open(QNAME,O_CREAT|O_RDWR,0600,&q_attr))==-1){
        perror("오류발생\n");
        exit(1);
    }

    if(mq_send(qd,send_data,strlen(send_data),PRIORITY)==-1){
        perror("전송 실패\n");
        exit(1);
    }

    if(mq_close(qd)==-1){
        perror("닫기 실패\n");
        exit(1);
    }
    
}
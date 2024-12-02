#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_TEXT 1024

struct message {
    long message_type;
    char text[MAX_TEXT];
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [send|recv]\n", argv[0]);
        exit(1);
    }

    key_t key;
    int msgid;
    struct message msg;

    // ftok를 사용하여 고유 키 생성
    key = ftok(".", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 메시지 큐 생성 또는 가져오기
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    if (strcmp(argv[1], "send") == 0) {
        // 송신 모드
        printf("전송 시작 (exit 입력 시 종료):\n");
        int message_count = 1;

        while (1) {
            printf("You: ");
            fgets(msg.text, MAX_TEXT, stdin);
            msg.text[strcspn(msg.text, "\n")] = '\0'; // 개행 문자 제거

            // "exit" 입력 시 종료
            if (strcmp(msg.text, "exit") == 0) {
                break;
            }

            msg.message_type = message_count++;
            if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
        }
    } else if (strcmp(argv[1], "recv") == 0) {
        // 수신 모드
        printf("수신 대기 중 (exit 입력 시 종료):\n");

        while (1) {
            if (msgrcv(msgid, &msg, sizeof(msg.text), 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            // 받은 메시지 출력
            printf("Other: %s\n", msg.text);

            // "exit" 메시지 수신 시 종료
            if (strcmp(msg.text, "exit") == 0) {
                break;
            }
        }

        // 메시지 큐 삭제
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl");
            exit(1);
        }
    } else {
        fprintf(stderr, "Invalid mode. Use 'send' or 'recv'.\n");
        exit(1);
    }

    return 0;
}
#include <stdlib.h>
void exitfunc1(void);
void exitfunc2(void);

int main(){
    atexit(exitfunc1);
    atexit(exitfunc2);
    printf("메인 함수\n");
}

void exitfunc1(void){
    printf("func1\n");
}

void exitfunc2(void){
    printf("func2\n");
}
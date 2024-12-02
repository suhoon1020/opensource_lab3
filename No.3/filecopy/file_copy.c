/*
텍스트 파일 내용 복사 프로그램
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    FILE *src; // 소스파일
    FILE *dst; // 도착 파일

    char ch;
    int count = 0;

    if (argc < 3)
    {
        printf("file_copy source_file destination_file 형식으로 작성하십시오\n");
        exit(1);
    }

    // 파일 열기 함수, "r" 은 읽기 형식으로 열기
    if ((src = fopen(argv[1], "r")) == NULL)
    {
        perror("foprn:src");
        exit(1);
    }
    //"w"는 쓰기 형식
    if ((dst = fopen(argv[2], "w")) == NULL)
    {
        perror("fopen:dst");
        exit(1);
    }

    while(!feof(src)){
        ch = (char)fgetc(src); //읽은 문자를 int형으로 반환, 파일 끝 도착시 EOF 반환
        if(ch!=EOF){
            fputc((int)ch,dst);
        }
        count++; //읽은 문자 수
    }

    fclose(src);
    fclose(dst);
}
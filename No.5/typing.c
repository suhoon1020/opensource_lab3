#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


int main(void){
	int fd;
	int nread, cnt=0 ,errcnt=0;
	int phrase_len;	
	char ch, text[] = "If you need another phrase, you can give a text file.";
	struct termios init_attr, new_attr;
	time_t start_time, end_time;


	phrase_len = strlen(text);

	fd = open(ttyname(fileno(stdin)), O_RDWR);
	tcgetattr(fd, &init_attr);

	new_attr = init_attr;
	new_attr.c_lflag &= ~ICANON;
	new_attr.c_lflag &= ~ECHO;
	new_attr.c_cc[VMIN] = 1;
	new_attr.c_cc[VTIME] = 0;

	if(tcsetattr(fd, TCSANOW, &new_attr) != 0){
		fprintf(stderr, "터미널 속성 설정 불가\n");
	}
	
	// 입력하는 순간 타이머 시작해야함.
	printf("다음 문장을 그대로 입력하세요.\n");
	printf("%s\n", text);

	while((nread = read(fd, &ch, 1)) > 0){
		// 백스페이스일 때 지우고 싶음 
		if(ch == 8 || ch == 127){
			if(cnt >0){
				write(fd,"\b \b", 3);
				cnt--;
			}
			continue;
		}
		if(ch == '\n')
			break;

		//키가 눌렸을 때 cnt가 0이면 시작 시간 기록
		if(cnt == 0){
			time(&start_time);
		}

		if(ch == text[cnt++]){
			write(fd, &ch, 1);
		}
		else{
			write(fd, "*", 1);
			errcnt++;
		}
	}

	// 빠져나오는 순간 종료 시간 기록
	time(&end_time);

	double typing_time = difftime(end_time, start_time);
	
	// 오류 횟수와 평균 분당 타수 
	// 분당 타수 = (총 타수 / 걸린시간) * 60 
	printf("\n오타수 : %d\n", errcnt);
	printf("타이핑 시간 : %.2f\n", typing_time);
	printf("분당 평균 타수 : %.2f\n", (cnt / typing_time)*60);
	tcsetattr(fd, TCSANOW, &init_attr);
	close(fd);

}
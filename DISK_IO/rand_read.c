#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
//필요하면 header file 추가 가능

#define SUFFLE_NUM 10000	// 이 값은 마음대로 수정 가능
#define SECOND_TO_MICRO 1000000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
void runtime(struct timeval *begin, struct timeval *end);
off_t size;

int main(int argc, char **argv)
{
	char buf[200];
	int fd;
	struct timeval begin, end;
	int *read_order_list;
	int num_of_records;

	if((fd = open(argv[1], O_RDONLY)) < 0)
	{
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	size = lseek(fd, 0, SEEK_END);
	num_of_records = size /200;
	read_order_list = (int*)calloc(size, sizeof(int));

	// 이 함수를 실행하면 'read_order_list' 배열에 읽어야 할 레코드 번호들이 순서대로
	// 나열되어 저장됨. 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함.
	GenRecordSequence(read_order_list, num_of_records);

	// 'read_order_list'를 이용하여 레코드 파일로부터 전체 레코드를 random 하게 읽어들이고,
	// 이때 걸리는 시간을 측정하는 코드 구현
	if(lseek(fd, 0, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	gettimeofday(&begin, NULL);
	for(int i = 0; i<num_of_records; i++){
		lseek(fd, read_order_list[i] * 200, SEEK_SET);
		if(read(fd, buf, 200) == 0)
			break;
	}
	gettimeofday(&end, NULL);
	runtime(&begin, &end);
	close(fd);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}

void runtime(struct timeval *begin, struct timeval *end)
{
	end->tv_sec -= begin->tv_sec;

	if(end->tv_usec < begin->tv_usec)
	{
		end->tv_sec--;
		end->tv_usec += SECOND_TO_MICRO;
	}

	end->tv_usec -= begin->tv_usec;
	printf("%d usec", end->tv_usec);
}
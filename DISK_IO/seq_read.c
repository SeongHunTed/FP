#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
//�ʿ��ϸ� header file �߰� ����
#define SECOND_TO_MICRO 1000000

int size;
void runtime(struct timeval *begin, struct timeval *end);

int main(int argc, char **argv)
{
	// ���ڵ� ���Ϸκ��� ��ü ���ڵ带 ���������� �о���̰�, �̶�
	// �ɸ��� �ð��� �����ϴ� �ڵ� ����

	char buf[200];
	int fd;
	struct timeval begin, end;

	if((fd = open(argv[1], O_RDONLY)) < 0)
	{
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	size = lseek(fd, 0, SEEK_END) / 200;

	if(lseek(fd, 0, SEEK_SET) < 0)
	{
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	gettimeofday(&begin, NULL);
	while(1)
	{
		if(read(fd, buf, 200) == 0)
			break;
	}
	gettimeofday(&end, NULL);
	runtime(&begin, &end);
	close(fd);

	return 0;
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
	printf("%ld usec", end->tv_usec);
}
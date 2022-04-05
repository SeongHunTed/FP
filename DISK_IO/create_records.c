#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#define RECORD_SIZE 200
#define S_MODE 0644

int main(int argc, char **argv)
{
	
	// 사용자로부터 입력 받은 레코드 수 만큼의 레코드 파일을 생성하는 코드 구현
	int num = atoi(argv[1]); //records 수
	int fd; //파일디스크립터
	char* buf;
	int studentRecord[50];

	for(int i = 0; i<50; i++)
	{
		studentRecord[i] = i;
	}

	if(argc != 3)
	{
		fprintf(stderr, "Usage : %s <#records> <record_file_name>", argv[1]);
		exit(1);
	}

	if((fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_MODE)) < 0)
	{
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}
	
	buf = (char*)calloc(200, sizeof(char));

	for(int i = 0; i<num; i++)
	{
		read(fd, buf, RECORD_SIZE);
		write(fd, buf, RECORD_SIZE);
		lseek(fd, 0, SEEK_END);
	}

	exit(0);

	// 파일에 '학생 레코드' 저장할 때 주의 사항
	// 1. 레코드의 크기는 무조건 200 바이트를 준수
	// 2. 레코드 파일에서 레코드와 레코드 사이에 어떤 데이터도 들어가면 안됨
	// 3. 레코드에는 임의의 데이터를 저장해도 무방
	// 4. 만약 n개의 레코드를 저장하면 파일 크기는 정확히 200 x n 바이트가 되어야 함

	return 0;
}

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
	
	// ����ڷκ��� �Է� ���� ���ڵ� �� ��ŭ�� ���ڵ� ������ �����ϴ� �ڵ� ����
	int num = atoi(argv[1]); //records ��
	int fd; //���ϵ�ũ����
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

	// ���Ͽ� '�л� ���ڵ�' ������ �� ���� ����
	// 1. ���ڵ��� ũ��� ������ 200 ����Ʈ�� �ؼ�
	// 2. ���ڵ� ���Ͽ��� ���ڵ�� ���ڵ� ���̿� � �����͵� ���� �ȵ�
	// 3. ���ڵ忡�� ������ �����͸� �����ص� ����
	// 4. ���� n���� ���ڵ带 �����ϸ� ���� ũ��� ��Ȯ�� 200 x n ����Ʈ�� �Ǿ�� ��

	return 0;
}

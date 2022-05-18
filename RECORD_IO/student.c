#include <stdio.h>		// �ʿ��� header file �߰� ����
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "student.h"


int readRecord(FILE *fp, char* recordbuf, int rrn);
void unpack(const char *recordbuf, STUDENT *s);


int writeRecord(FILE *fp, char *recordbuf, int rrn);
void pack(char *recordbuf, const STUDENT *s);


int appendRecord(FILE *fp, STUDENT *s);


void searchRecord(FILE *fp, enum FIELD f, char *keyval); // enum tag ���� ���� -> Linux����� Ȯ��
void printRecord(const STUDENT *s);

//
// ���ڵ��� �ʵ���� enum FIELD Ÿ���� ������ ��ȯ���� �ش�.
// ���� ���, ����ڰ� ������ ��ɾ��� ���ڷ� "NAME"�̶�� �ʵ���� ����Ͽ��ٸ� 
// ���α׷� ���ο��� �̸� NAME(=1)���� ��ȯ�� �ʿ伺�� ������, �̶� �� �Լ��� �̿��Ѵ�.
//
enum FIELD getFieldID(char *fieldname); // enum tag ���� ���� -> Linux����� Ȯ��

void main(int argc, char *argv[])
{
	FILE *fp;			// ��� file processing operation�� C library�� ����� ��
	fp = fopen(argv[2], "r+");
	char recordBuf[RECORD_SIZE] = {0};
	char hearder[HEADER_SIZE]; // ��� ���ڵ�
	// char countPage[4]; // ���ڵ���� ǥ��
	int reservedArea = 0; // ����� �� �������
	int countPage = 0; // ����� �� ��ü ���ڵ� ��
	STUDENT student; // 85byte

	int i,j;

	// data file�� ���� ��
	if(access(argv[2], F_OK < 0)){							
		fp = fopen(argv[2], "w");
		countPage = 0;
		reservedArea = 0;
		// header record ����
		//	countPage�� ����
		memset(hearder, 0xFF, sizeof(hearder));
		memcpy(hearder, (char *)&countPage, sizeof(int));
		//	reservedArea ����
		memcpy(hearder + 4, (char *)&reservedArea, sizeof(int));
		fwrite((void*)hearder, sizeof(hearder), 1, fp);
	}

	switch(argv[1][1]){
		
		case 's':	// search option
			printf("Hello s");
			break;
		
		case 'a':	// append option
			if(argc < 7){
				fprintf(stderr, "Usage : %s -a <Filename> <Student ID> <Student name> <Student dept> <Student Addr> <Student Email> \n", argv[0]);
				exit(1);
			}
			// bound ����ó��
			// open ����ó��
			// sscanf(argv[3], "%s", student.id);
			// sscanf(argv[4], "%s", student.name);
			// sscanf(argv[5], "%s", student.dept);
			// sscanf(argv[6], "%s", student.addr);
			// sscanf(argv[7], "%s", student.email);

			appendRecord(fp, &student);
			fclose(fp);
			
			break;
		default:
			fprintf(stderr, "Wrong Option!\n");
			break;
	}
	exit(0);

}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

//
// �Լ� readRecord()�� �л� ���ڵ� ���Ͽ��� �־��� rrn�� �ش��ϴ� ���ڵ带 �о 
// recordbuf�� �����ϰ�, ���� unpack() �Լ��� ȣ���Ͽ� �л� Ÿ���� ������ ���ڵ���
// �� �ʵ尪�� �����Ѵ�. �����ϸ� 1�� �׷��� ������ 0�� �����Ѵ�.
// unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� record���� �� field�� �����ϴ� ���� �Ѵ�.
//
int readRecord(FILE *fp, char *recordbuf, int rrn){
	
	if(rrn == 0){
		rewind(fp);
		fseek(fp, HEADER_SIZE, SEEK_SET);
		if(fread(recordbuf, HEADER_SIZE, 1, fp) > 0) return 1;
		else return 0;
	} else {
		rewind(fp);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
		if(fread(recordbuf, RECORD_SIZE, 1, fp) > 0) return 1;
		else return 0;
	}
}

void unpack(const char *recordbuf, STUDENT *s){
	
	sprintf(recordbuf, "%[^#]%[^#]%[^#]%[^#]%[^#]", s->id, s->name, s->dept, s->addr, s->email);

}

//
// �Լ� writeRecord()�� �л� ���ڵ� ���Ͽ� �־��� rrn�� �ش��ϴ� ��ġ�� recordbuf�� 
// ����Ǿ� �ִ� ���ڵ带 �����Ѵ�. ������ pack() �Լ��� ȣ���Ͽ� recordbuf�� �����͸� ä�� �ִ´�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int writeRecord(FILE *fp, char *recordbuf, int rrn){
	if(rrn == 0){
		rewind(fp);
		fseek(fp, HEADER_SIZE, SEEK_SET);
		if(fwrite(recordbuf, HEADER_SIZE, 1, fp) > 0) return 1;
		else return 0;
	} else {
		rewind(fp);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
		if(fwrite(recordbuf, RECORD_SIZE, 1, fp) > 0) return 1;
		else return 0;
	}
}

void pack(char *recordbuf, const STUDENT *s){
	// ������ packing ����

	// int i = 0;
	// int j = 0;
	// for(i = 0; i < strlen(s->id); i++){
	// 	recordbuf[j++] = s->id[i];
	// } recordbuf[j++] = '#';
	// for(i = 0; i < strlen(s->name); i++){
	// 	recordbuf[j++] = s->name[i];
	// } recordbuf[j++] = '#';
	// for(i = 0; i < strlen(s->dept); i++){
	// 	recordbuf[j++] = s->dept[i];
	// } recordbuf[j++] = '#';
	// for(i = 0; i < strlen(s->addr); i++){
	// 	recordbuf[j++] = s->addr[i];
	// } recordbuf[j++] = '#';
	// for(i = 0; i < strlen(s->email); i++){
	// 	recordbuf[j++] = s->email[i];
	// } recordbuf[j++] = '#';

	// sprintf�� packing ����

	sprintf(recordbuf, "%s#%s#%s#%s#%s#", s->id, s->name, s->dept, s->addr, s->email);

}

//
// �Լ� appendRecord()�� �л� ���ڵ� ���Ͽ� ���ο� ���ڵ带 append�Ѵ�.
// ���ڵ� ���Ͽ� ���ڵ尡 �ϳ��� �������� �ʴ� ��� (ù ��° append)�� header ���ڵ带
// ���Ͽ� �����ϰ� ù ��° ���ڵ带 �����Ѵ�. 
// �翬�� ���ڵ带 append�� �� ������ header ���ڵ忡 ���� ������ �ڵ���� �Ѵ�.
// �Լ� appendRecord()�� ���������� writeRecord() �Լ��� ȣ���Ͽ� ���ڵ� ������ �ذ��Ѵ�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int appendRecord(FILE *fp, STUDENT *s){

	char recordbuf[RECORD_SIZE];
	char headerbuf[HEADER_SIZE];
	int count = 0;
	int reseverdArea = 0;
	
	// ����� �о�� ���� ���ڵ� ������ �����´�
	readRecord(fp, headerbuf, 0);
	memcpy(&count, headerbuf, sizeof(int));
	memcpy(&reseverdArea, headerbuf + 4, sizeof(int));

	// �Է°� packing
	pack(recordbuf, s);

	if(count == 0){
		count++;
		memcpy(headerbuf, &count, sizeof(int));
		memcpy(headerbuf + sizeof(int), &reseverdArea, sizeof(int));
		writeRecord(fp, headerbuf, 0);
		if(writeRecord(fp, recordbuf, 1) > 0) return 1;
	} else {
		count++;
		memcpy(&headerbuf, &count, sizeof(int));
		// memcpy(&headerbuf + 4, &reseverdArea, sizeof(int));
		writeRecord(fp, headerbuf, 0);
		if(writeRecord(fp, recordbuf, count) > 0) return 1;
	}

	return 0;

}

//
// �л� ���ڵ� ���Ͽ��� �˻� Ű���� �����ϴ� ���ڵ尡 �����ϴ����� sequential search ����� 
// ���� ã�Ƴ���, �̸� �����ϴ� ��� ���ڵ��� ������ ����Ѵ�. �˻� Ű�� �л� ���ڵ带 �����ϴ�
// � �ʵ嵵 �����ϴ�. ���������� readRecord() �Լ��� ȣ���Ͽ� sequential search�� �����Ѵ�.
// �˻� ����� ����� �� �ݵ�� printRecord() �Լ��� ����Ѵ�. (�ݵ�� ���Ѿ� �ϸ�, �׷���
// �ʴ� ��� ä�� ���α׷����� �ڵ������� Ʋ�� ������ �ν���)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval){ // enum tag ���� ���� -> Linux����� Ȯ��

} 

enum FIELD getFieldID(char *fieldname){ // enum tag ���� ���� -> Linux����� Ȯ��

} 

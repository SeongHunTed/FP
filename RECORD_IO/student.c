#include <stdio.h>		// 필요한 header file 추가 가능
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "student.h"


int readRecord(FILE *fp, char* recordbuf, int rrn);
void unpack(const char *recordbuf, STUDENT *s);


int writeRecord(FILE *fp, char *recordbuf, int rrn);
void pack(char *recordbuf, const STUDENT *s);


int appendRecord(FILE *fp, STUDENT *s);


void searchRecord(FILE *fp, enum FIELD f, char *keyval); // enum tag 내가 붙임 -> Linux실행시 확인
void printRecord(const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
enum FIELD getFieldID(char *fieldname); // enum tag 내가 붙임 -> Linux실행시 확인

void main(int argc, char *argv[])
{
	FILE *fp;			// 모든 file processing operation은 C library를 사용할 것
	if(access(argv[2], F_OK) < 0){
		fp = fopen(argv[2], "w+");
	} else {
		fp = fopen(argv[2], "r+");
	}
	
	STUDENT student; // 85byte

	switch(argv[1][1]){
		
		case 's':	// search option
			printf("Hello s");
			break;
		
		case 'a':	// append option
			if(argc < 8){
				fprintf(stderr, "Usage : %s -a <Filename> <Student ID> <Student name> <Student dept> <Student Addr> <Student Email> \n", argv[0]);
				exit(1);
			}
			// bound 예외처리
			// open 예외처리
			
			sscanf(argv[3], "%s", student.id);
			sscanf(argv[4], "%s", student.name);
			sscanf(argv[5], "%s", student.dept);
			sscanf(argv[6], "%s", student.addr);
			sscanf(argv[7], "%s", student.email);

			appendRecord(fp, &student);
			fclose(fp);
			
			break;
		default:
			fprintf(stderr, "Wrong Option!\n");
			break;
	}
	// exit(0);
}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

//
// 함수 readRecord()는 학생 레코드 파일에서 주어진 rrn에 해당하는 레코드를 읽어서 
// recordbuf에 저장하고, 이후 unpack() 함수를 호출하여 학생 타입의 변수에 레코드의
// 각 필드값을 저장한다. 성공하면 1을 그렇지 않으면 0을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하는 일을 한다.
//
int readRecord(FILE *fp, char *recordbuf, int rrn){
	
	if(rrn == 0){
		rewind(fp);
		fseek(fp, 0, SEEK_SET);
		if(fread(recordbuf, HEADER_SIZE, 1, fp) > 0) {
			printf("Header read in\n");
			return 1;
		}
		else return 0;
	// 레코드를 쓰는 경우 85바이트
	} else {
		printf("Real Record in\n");
		rewind(fp);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * (rrn-1), SEEK_SET);
		if(fread(recordbuf, RECORD_SIZE, 1, fp) > 0) return 1;
		else return 0;
	}

}

void unpack(const char *recordbuf, STUDENT *s){
	
	// char unpackbuf[80];
	int i;
	for(i = 0; i<strlen(s->id); i++){
		if(recordbuf[i] == '#'){
			break;
		}
		s->id[i] = recordbuf[i];
	}

}

//
// 함수 writeRecord()는 학생 레코드 파일에 주어진 rrn에 해당하는 위치에 recordbuf에 
// 저장되어 있는 레코드를 저장한다. 이전에 pack() 함수를 호출하여 recordbuf에 데이터를 채워 넣는다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writeRecord(FILE *fp, char *recordbuf, int rrn){
	// 헤더 레코드만 쓰는 경우 8바이트
	if(rrn == 0){
		rewind(fp);
		fseek(fp, 0, SEEK_SET);
		if(fwrite(recordbuf, HEADER_SIZE, 1, fp) > 0) {
			printf("Header Write in\n");
			return 1;
		}
		else return 0;
	// 레코드를 쓰는 경우 85바이트
	} else {
		printf("Real Record in\n");
		rewind(fp);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * (rrn-1), SEEK_SET);
		if(fwrite(recordbuf, RECORD_SIZE, 1, fp) > 0) return 1;
		else return 0;
	}
}

void pack(char *recordbuf, const STUDENT *s){
	// 일일히 packing 구현

	int i = 0;
	int j = 0;
	for(i = 0; i < strlen(s->id); i++){
		recordbuf[j++] = s->id[i];
	} recordbuf[j++] = '#';
	for(i = 0; i < strlen(s->name); i++){
		recordbuf[j++] = s->name[i];
	} recordbuf[j++] = '#';
	for(i = 0; i < strlen(s->dept); i++){
		recordbuf[j++] = s->dept[i];
	} recordbuf[j++] = '#';
	for(i = 0; i < strlen(s->addr); i++){
		recordbuf[j++] = s->addr[i];
	} recordbuf[j++] = '#';
	for(i = 0; i < strlen(s->email); i++){
		recordbuf[j++] = s->email[i];
	} recordbuf[j++] = '#';

	// sprintf로 packing 구현
	// sprintf(recordbuf, "%s#%s#%s#%s#%s#", s->id, s->name, s->dept, s->addr, s->email);

}

//
// 함수 appendRecord()는 학생 레코드 파일에 새로운 레코드를 append한다.
// 레코드 파일에 레코드가 하나도 존재하지 않는 경우 (첫 번째 append)는 header 레코드를
// 파일에 생성하고 첫 번째 레코드를 저장한다. 
// 당연히 레코드를 append를 할 때마다 header 레코드에 대한 수정이 뒤따라야 한다.
// 함수 appendRecord()는 내부적으로 writeRecord() 함수를 호출하여 레코드 저장을 해결한다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int appendRecord(FILE *fp, STUDENT *s){

	char recordbuf[RECORD_SIZE] = {0};
	char headerbuf[8];
	int count = 0;
	int reservedArea = 0;
	

	// 헤더 읽어 들이기
	fseek(fp, 0, SEEK_SET);
	memset(headerbuf, 0, sizeof(HEADER_SIZE));
	readRecord(fp, headerbuf, 0);
	memcpy(&count, headerbuf, sizeof(int));
	memcpy(&reservedArea, headerbuf + sizeof(int), sizeof(int));

	// 입력값 packing
	pack(recordbuf, s);
	
	if(count == 0){
		printf("In\n");
		count++;
		memset(headerbuf, 0, sizeof(HEADER_SIZE));
		memcpy(headerbuf, &count, sizeof(int));
		memcpy(headerbuf + 4, &reservedArea, sizeof(int));
		writeRecord(fp, headerbuf, 0);
		if(writeRecord(fp, recordbuf, 1) > 0) return 1;
	} else {
		count++;
		memcpy(headerbuf, &count, sizeof(int));
		memcpy(headerbuf + 4, &reservedArea, sizeof(int));
		writeRecord(fp, headerbuf, 0);
		if(writeRecord(fp, recordbuf, count) > 0) return 1;
	}

	return 0;

}

//
// 학생 레코드 파일에서 검색 키값을 만족하는 레코드가 존재하는지를 sequential search 기법을 
// 통해 찾아내고, 이를 만족하는 모든 레코드의 내용을 출력한다. 검색 키는 학생 레코드를 구성하는
// 어떤 필드도 가능하다. 내부적으로 readRecord() 함수를 호출하여 sequential search를 수행한다.
// 검색 결과를 출력할 때 반드시 printRecord() 함수를 사용한다. (반드시 지켜야 하며, 그렇지
// 않는 경우 채점 프로그램에서 자동적으로 틀린 것으로 인식함)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval){ // enum tag 내가 붙임 -> Linux실행시 확인

} 

enum FIELD getFieldID(char *fieldname){ // enum tag 내가 붙임 -> Linux실행시 확인

} 

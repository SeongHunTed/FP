#include <stdio.h>		// 필요한 header file 추가 가능
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "student.h"

//
// 함수 readRecord()는 학생 레코드 파일에서 주어진 rrn에 해당하는 레코드를 읽어서 
// recordbuf에 저장하고, 이후 unpack() 함수를 호출하여 학생 타입의 변수에 레코드의
// 각 필드값을 저장한다. 성공하면 1을 그렇지 않으면 0을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하는 일을 한다.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// 함수 writeRecord()는 학생 레코드 파일에 주어진 rrn에 해당하는 위치에 recordbuf에 
// 저장되어 있는 레코드를 저장한다. 이전에 pack() 함수를 호출하여 recordbuf에 데이터를 채워 넣는다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// 함수 appendRecord()는 학생 레코드 파일에 새로운 레코드를 append한다.
// 레코드 파일에 레코드가 하나도 존재하지 않는 경우 (첫 번째 append)는 header 레코드를
// 파일에 생성하고 첫 번째 레코드를 저장한다. 
// 당연히 레코드를 append를 할 때마다 header 레코드에 대한 수정이 뒤따라야 한다.
// 함수 appendRecord()는 내부적으로 writeRecord() 함수를 호출하여 레코드 저장을 해결한다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// 학생 레코드 파일에서 검색 키값을 만족하는 레코드가 존재하는지를 sequential search 기법을 
// 통해 찾아내고, 이를 만족하는 모든 레코드의 내용을 출력한다. 검색 키는 학생 레코드를 구성하는
// 어떤 필드도 가능하다. 내부적으로 readRecord() 함수를 호출하여 sequential search를 수행한다.
// 검색 결과를 출력할 때 반드시 printRecord() 함수를 사용한다. (반드시 지켜야 하며, 그렇지
// 않는 경우 채점 프로그램에서 자동적으로 틀린 것으로 인식함)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
enum FIELD getFieldID(char *fieldname);

//
// 학생 레코드 파일에서 "학번" 키값을 만족하는 레코드를 찾아서 이것을 삭제한다.
// 참고로, 검색 조건은 반드시 학번(ID)만을 사용한다. 레코드의 검색은 searchRecord() 함수와
// 유사한 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int deleteRecord(FILE *fp, enum FIELD f, char *keyval);

//
// 학생 레코드 파일에 새로운 레코드를 추가한다. 과제 설명서에서 언급한 대로, 삭제 레코드가
// 존재하면 반드시 삭제 레코드들 중 하나에 새로운 레코드를 저장한다. 삭제 레코드 리스트 관리와
// 삭제 레코드의 선택은 교재 방식을 따른다. 새로운 레코드의 추가는 appendRecord() 함수와 비슷한
// 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

void main(int argc, char *argv[])
{
	FILE *fp;			// 모든 file processing operation은 C library를 사용할 것
	if(access(argv[2], F_OK) < 0){
		fp = fopen(argv[2], "w+");
	} else {
		fp = fopen(argv[2], "r+");
	}

	STUDENT s; // 85byte

	switch(argv[1][1]){
		
		case 's':	// search option

			if(argc < 3){
				fprintf(stderr, "Usage : %s -s <Filename> <field_name=filed_value>", argv[0]);
				break;
			}
			
			// keyvalue 분류
			char keyvalue[30] = {0};
			int i = 0;
			for(i = 0; i<strlen(argv[3]); i++){
				if(argv[3][i] == '='){
					i++;
					int a = i;
					for(int j = 0; j<strlen(argv[3])-a; j++){
						keyvalue[j] = argv[3][i++];
					}
					keyvalue[++i] = '\0';
					break;
				}
			}

			// field_name 분류
			char enumbuf[35] = {0};
			sscanf(argv[3], "%s", enumbuf);
			enum FIELD f = getFieldID(enumbuf);

			searchRecord(fp, f, keyvalue);
			break;
		
		case 'a':	// append option
			if(argc < 8){
				fprintf(stderr, "Usage : %s -a <Filename> <Student ID> <Student name> <Student dept> <Student Addr> <Student Email> \n", argv[0]);
				break;
			}
			// bound 예외처리
			// open 예외처리
			
			sscanf(argv[3], "%s", s.id);
			sscanf(argv[4], "%s", s.name);
			sscanf(argv[5], "%s", s.dept);
			sscanf(argv[6], "%s", s.addr);
			sscanf(argv[7], "%s", s.email);

			appendRecord(fp, s.id, s.name, s.dept, s.addr, s.email);

			break;
		
		case 'd':
			if(argc < 3){
				printf("Usage : -d <Filename> <ID=0000000>");
				break;
			}
			// keyvalue 분류
			memset(keyvalue, (char)0xFF, sizeof(keyvalue));
			for(i = 0; i < 8; i++){
				keyvalue[i] = argv[3][3+i];
			}
			keyvalue[8] = '\0';

			// field_name 분류
			memset(enumbuf, (char)0xFF, sizeof(enumbuf));
			sscanf(argv[3], "%s", enumbuf);
			f = getFieldID(enumbuf);
			deleteRecord(fp, f, keyvalue);
			break;

		case 'i':
			if(argc < 8){
				fprintf(stderr, "Usage : %s -a <Filename> <Student ID> <Student name> <Student dept> <Student Addr> <Student Email> \n", argv[0]);
				break;
			}

			sscanf(argv[3], "%s", s.id);
			sscanf(argv[4], "%s", s.name);
			sscanf(argv[5], "%s", s.dept);
			sscanf(argv[6], "%s", s.addr);
			sscanf(argv[7], "%s", s.email);

			insertRecord(fp, s.id, s.name, s.dept, s.addr, s.email);
			break;

		default:
			fprintf(stderr, "Wrong Option!\n");
			break;
	}
	fclose(fp);
	fp = NULL;
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn)
{
	char recordbuf[RECORD_SIZE] = {0};
	pack(recordbuf, s);

	rewind(fp);
	fseek(fp, HEADER_SIZE + RECORD_SIZE * (rrn-1), SEEK_SET);
	if(fwrite(recordbuf, RECORD_SIZE, 1, fp) > 0) return 1;
	else return 0;

}
void pack(char *recordbuf, const STUDENT *s)
{
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
	recordbuf[j] = '\0';
}

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email)
{
	STUDENT s;
	sscanf(id, "%s", s.id);
	sscanf(name, "%s", s.name);
	sscanf(dept, "%s", s.dept);
	sscanf(addr, "%s", s.addr);
	sscanf(email, "%s", s.email);
	
	char headerbuf[8] = {0};
	int count = 0;
	int reservedArea = -1;


	// 헤더 읽어 들이기
	fseek(fp, 0, SEEK_SET);
	fread(&count, sizeof(int), 1, fp);
	fread(&reservedArea, sizeof(int), 1, fp);

	if(count == 0){
		count++;
		memset(headerbuf, 0, sizeof(HEADER_SIZE));
		memcpy(headerbuf, &count, sizeof(int));
		// memcpy(headerbuf + 4, &reservedArea, sizeof(int));
		fseek(fp, 0, SEEK_SET);
		fwrite(headerbuf, HEADER_SIZE, 1, fp);
		fseek(fp, 4, SEEK_SET);
		fwrite(&reservedArea, sizeof(int), 1, fp);
		if(writeRecord(fp, &s, 1) > 0) return 1;
	} else {
		count++;
		memset(headerbuf, 0, sizeof(HEADER_SIZE));
		memcpy(headerbuf, &count, sizeof(int));
		memcpy(headerbuf + 4, &reservedArea, sizeof(int));
		fseek(fp, 0, SEEK_SET);
		fwrite(headerbuf, HEADER_SIZE, 1, fp);
		if(writeRecord(fp, &s, count) > 0) return 1;
	}

	return 0;
}

int readRecord(FILE *fp, STUDENT *s, int rrn)
{	
	rewind(fp);
	char recordbuf[RECORD_SIZE] ={0};

	fseek(fp, HEADER_SIZE + (RECORD_SIZE * rrn), SEEK_SET);
	if(fread(recordbuf, RECORD_SIZE, 1, fp) > 0){
		unpack(recordbuf, s);
		return 1;
	} else return 0;

}


void unpack(const char *recordbuf, STUDENT *s)
{
	int j = 0;
	int i = 0;
	
	for(i = 0; i<9; i++){
		if(recordbuf[j] == '#') break;
		s->id[i] = recordbuf[j];
		j++;
	}
	s->id[j] = '\0';
	j++;


	for(i = 0; i<11; i++){
		if(recordbuf[j] == '#') break;
		s->name[i] = recordbuf[j];
		j++;
	}
	s->name[j] = '\0';
	j++;


	for(i = 0; i < 13; i++){
		if(recordbuf[j] == '#') break;
		s->dept[i] = recordbuf[j];
		j++;
	}
	s->dept[j] = '\0';
	j++;


	for(i = 0; i < 31; i++){
		if(recordbuf[j] == '#') break;
		s->addr[i] = recordbuf[j];
		j++;
	}
	s->addr[j] = '\0';
	j++;

	for(i = 0; i < 21; i++){
		if(recordbuf[j] == '#') break;
		s->email[i] = recordbuf[j];
		j++;
	}
	s->email[++i] = '\0';

}

void searchRecord(FILE *fp, enum FIELD f, char *keyval)
{	
	rewind(fp);
	fseek(fp, (off_t)0 , SEEK_END);
	int rrn = ftell(fp) / RECORD_SIZE;
	STUDENT s;
	// memset(&s.id, 0 , sizeof(s.id));
	memset(&s.name, 0, sizeof(s.name));
	memset(&s.dept, 0, sizeof(s.dept));
	memset(&s.addr, 0, sizeof(s.addr));
	memset(&s.email, 0, sizeof(s.email));

	char recordbuf[RECORD_SIZE];
	char idbuf[8] = {0};
	char namebuf[10];
	char deptbuf[12];
	char addrbuf[30];
	char emailbuf[20];

	memset(idbuf, 0, sizeof(idbuf));
	memset(namebuf, 0, sizeof(namebuf));
	memset(deptbuf, 0, sizeof(deptbuf));
	memset(addrbuf, 0, sizeof(addrbuf));
	memset(emailbuf, 0, sizeof(emailbuf));

	
 	if(f == ID){
		for(int i = 0; i<rrn; i++){
			fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(recordbuf, RECORD_SIZE, 1, fp);

			char idbuf[8] = {0};
			int j = 0;
			for(j = 0; i<sizeof(idbuf); j++){
				if(recordbuf[j] == '#') break;
				idbuf[j] = recordbuf[j];
			}
			idbuf[j] = '\0';
			if(strcmp(keyval, idbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == NAME){
		for(int i = 0; i<rrn; i++){

			int count = 0; // 딜리미터 카운트

			// 딜리미터 카운트를 위해 record를 가져옴
			fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(recordbuf, RECORD_SIZE, 1, fp);

			// 딜리미터 카운트 부분
			int k = 0;
			for(k = 0; k < RECORD_SIZE; k++){
				if(count == 1) break;
				if(recordbuf[k] == '#') count++;
			}

			// 해당 정보를 가져옴
			fseek(fp, k + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(namebuf, 10, 1, fp);
			char newnamebuf[10] = {0};
			int j = 0;
			for(j = 0; i<sizeof(namebuf); j++){
				if(namebuf[j] == '#') break;
				newnamebuf[j] = namebuf[j];
			}
			newnamebuf[j] = '\0';
			if(strcmp(keyval, newnamebuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == DEPT){
		for(int i = 0; i<rrn; i++){
			int count = 0;
			int k = 0;
			fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(recordbuf, RECORD_SIZE, 1, fp);
			for(k = 0; k < RECORD_SIZE; k++){
				if(count == 2) break;
				if(recordbuf[k] == '#') count++;
			}
			fseek(fp, k + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(deptbuf, 12, 1, fp);
			char newbuf[12] ={0};
			int j = 0;
			for(j = 0; i < 12; j++){
				if(deptbuf[j] == '#') break;
				newbuf[j] = deptbuf[j];
			}
			newbuf[j] = '\0';
			if(strcmp(keyval, newbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == ADDR){
		for(int i = 0; i<rrn; i++){
			int count = 0;
			int k = 0;
			fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(recordbuf, RECORD_SIZE, 1, fp);
			for(k = 0; k < RECORD_SIZE; k++){
				if(count == 3) break;
				if(recordbuf[k] == '#') count++;
			}
			fseek(fp, k + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(addrbuf, 30, 1, fp);
			char newbuf[30] = {0};
			int j = 0;
			for(j = 0; i<sizeof(addrbuf); j++){
				if(addrbuf[j] == '#') break;
				newbuf[j] = addrbuf[j];
			}
			newbuf[j] = '\0';
			if(strcmp(keyval, newbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == EMAIL){
		for(int i = 0; i<rrn; i++){
			int count = 0;
			fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(recordbuf, RECORD_SIZE, 1, fp);
			int k = 0;
			for(k = 0; k < RECORD_SIZE; k++){
				if(count == 4) break;
				if(recordbuf[k] == '#') count++;
			}

			fseek(fp, k + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(emailbuf, 20, 1, fp);
			char newbuf[20] = {0};
			int j = 0;
			for(j = 0; i<sizeof(emailbuf); j++){
				if(emailbuf[j] == '#') break;
				newbuf[j] = emailbuf[j];
			}
			newbuf[j] = '\0';
			if(strcmp(keyval, newbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}


}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}


enum FIELD getFieldID(char *fieldname)
{	
	char enumcheck[6] = {0};

	int i = 0;
	for(i = 0; i < strlen(fieldname); i++){
		if(fieldname[i] == '='){
			break;
		}
		enumcheck[i] = fieldname[i];
	}
	enumcheck[i] = '\0';

	if(strcmp(enumcheck, "ID") == 0) return ID;
	else if(strcmp(enumcheck, "NAME") == 0) return NAME;
	else if(strcmp(enumcheck, "DEPT") == 0) return DEPT;
	else if(strcmp(enumcheck, "ADDR") == 0) return ADDR;
	else if(strcmp(enumcheck, "EMAIL") == 0) return EMAIL;
	else return 0;
}

int deleteRecord(FILE *fp, enum FIELD f, char *keyval){

	rewind(fp);
	char deletebuf[RECORD_SIZE] = {0};
	char idbuf[9] = {0};
	char symbol = '*';
	int reservedArea = 0;
	int rrn = 0;

	// rrn 가져오기
	fseek(fp, 0, SEEK_SET);
	fread(&rrn, sizeof(int), 1, fp);

	// reservedArea 가져오기
	fseek(fp, 4, SEEK_SET);
	fread(&reservedArea, sizeof(int), 1, fp);

	for(int i = 0; i < rrn; i++){
		fseek(fp, HEADER_SIZE + i * RECORD_SIZE, SEEK_SET);
		fread(idbuf, sizeof(idbuf), 1, fp);
		idbuf[8] = '\0';

		if(strcmp(keyval, idbuf) == 0){
			fseek(fp, HEADER_SIZE + i * RECORD_SIZE, SEEK_SET);
			memcpy(deletebuf, &symbol, sizeof(symbol));
			fwrite(deletebuf, sizeof(deletebuf), 1, fp);
			fseek(fp, 1 + HEADER_SIZE + i * RECORD_SIZE, SEEK_SET);
			fwrite(&reservedArea, sizeof(int), 1, fp);
			fseek(fp, 4, SEEK_SET);
			fwrite(&i, sizeof(i), 1, fp);
			return 1;
		}
	}

	return 0;
}

//
// 학생 레코드 파일에 새로운 레코드를 추가한다. 과제 설명서에서 언급한 대로, 삭제 레코드가
// 존재하면 반드시 삭제 레코드들 중 하나에 새로운 레코드를 저장한다. 삭제 레코드 리스트 관리와
// 삭제 레코드의 선택은 교재 방식을 따른다. 새로운 레코드의 추가는 appendRecord() 함수와 비슷한
// 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email){
	
	rewind(fp);

	STUDENT s;
	sscanf(id, "%s", s.id);
	sscanf(name, "%s", s.name);
	sscanf(dept, "%s", s.dept);
	sscanf(addr, "%s", s.addr);
	sscanf(email, "%s", s.email);

	// 헤더와 이전 삭제 레코드 rrn
	int count = 0;
	int reservedArea = 0;
	int prevReserved = 0;

	// 가장 최근에 삭제된 레코드
	fseek(fp, 4, SEEK_SET);
	fread(&reservedArea, sizeof(int), 1, fp);

	// 삭제된 레코드가 있는경우
	if(reservedArea > -1){
		fseek(fp, 1 + HEADER_SIZE + reservedArea * RECORD_SIZE, SEEK_SET);
		fread(&prevReserved, sizeof(int), 1, fp);
		// 이전 레코드 값 헤더에 업데이트
		fseek(fp, 4, SEEK_SET);
		fwrite(&prevReserved, sizeof(int), 1, fp);
		// insert할 레코드 정보 쓰기
		writeRecord(fp, &s, reservedArea + 1);
		return 1;
	} else {			// 삭제된 레코드가 없는 경우
		// count값 가져와서 증가시키기
		fseek(fp, 0, SEEK_SET);
		fread(&count, sizeof(int), 1, fp);
		count++;
		fseek(fp, 0, SEEK_SET);
		fwrite(&count, sizeof(int), 1, fp);
		// appendRecord와 같이 수행
		writeRecord(fp, &s, count);
		return 1;
	}
	return 0;

}
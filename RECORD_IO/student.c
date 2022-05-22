#include <stdio.h>		// �ʿ��� header file �߰� ����
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "student.h"

//
// �Լ� readRecord()�� �л� ���ڵ� ���Ͽ��� �־��� rrn�� �ش��ϴ� ���ڵ带 �о 
// recordbuf�� �����ϰ�, ���� unpack() �Լ��� ȣ���Ͽ� �л� Ÿ���� ������ ���ڵ���
// �� �ʵ尪�� �����Ѵ�. �����ϸ� 1�� �׷��� ������ 0�� �����Ѵ�.
// unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� record���� �� field�� �����ϴ� ���� �Ѵ�.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// �Լ� writeRecord()�� �л� ���ڵ� ���Ͽ� �־��� rrn�� �ش��ϴ� ��ġ�� recordbuf�� 
// ����Ǿ� �ִ� ���ڵ带 �����Ѵ�. ������ pack() �Լ��� ȣ���Ͽ� recordbuf�� �����͸� ä�� �ִ´�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// �Լ� appendRecord()�� �л� ���ڵ� ���Ͽ� ���ο� ���ڵ带 append�Ѵ�.
// ���ڵ� ���Ͽ� ���ڵ尡 �ϳ��� �������� �ʴ� ��� (ù ��° append)�� header ���ڵ带
// ���Ͽ� �����ϰ� ù ��° ���ڵ带 �����Ѵ�. 
// �翬�� ���ڵ带 append�� �� ������ header ���ڵ忡 ���� ������ �ڵ���� �Ѵ�.
// �Լ� appendRecord()�� ���������� writeRecord() �Լ��� ȣ���Ͽ� ���ڵ� ������ �ذ��Ѵ�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// �л� ���ڵ� ���Ͽ��� �˻� Ű���� �����ϴ� ���ڵ尡 �����ϴ����� sequential search ����� 
// ���� ã�Ƴ���, �̸� �����ϴ� ��� ���ڵ��� ������ ����Ѵ�. �˻� Ű�� �л� ���ڵ带 �����ϴ�
// � �ʵ嵵 �����ϴ�. ���������� readRecord() �Լ��� ȣ���Ͽ� sequential search�� �����Ѵ�.
// �˻� ����� ����� �� �ݵ�� printRecord() �Լ��� ����Ѵ�. (�ݵ�� ���Ѿ� �ϸ�, �׷���
// �ʴ� ��� ä�� ���α׷����� �ڵ������� Ʋ�� ������ �ν���)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// ���ڵ��� �ʵ���� enum FIELD Ÿ���� ������ ��ȯ���� �ش�.
// ���� ���, ����ڰ� ������ ��ɾ��� ���ڷ� "NAME"�̶�� �ʵ���� ����Ͽ��ٸ� 
// ���α׷� ���ο��� �̸� NAME(=1)���� ��ȯ�� �ʿ伺�� ������, �̶� �� �Լ��� �̿��Ѵ�.
//
enum FIELD getFieldID(char *fieldname);

void main(int argc, char *argv[])
{
	FILE *fp;			// ��� file processing operation�� C library�� ����� ��
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
			
			// keyvalue �з�
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
			printf("Keyvalue : %s \n", keyvalue);

			// field_name �з�
			char enumbuf[35] = {0};
			sscanf(argv[3], "%s", enumbuf);
			enum FIELD f = getFieldID(enumbuf);
			printf("FIELD : %d\n", f);

			searchRecord(fp, f, keyvalue);
			fclose(fp);
			break;
		
		case 'a':	// append option
			if(argc < 8){
				fprintf(stderr, "Usage : %s -a <Filename> <Student ID> <Student name> <Student dept> <Student Addr> <Student Email> \n", argv[0]);
				break;
			}
			// bound ����ó��
			// open ����ó��
			
			sscanf(argv[3], "%s", s.id);
			sscanf(argv[4], "%s", s.name);
			sscanf(argv[5], "%s", s.dept);
			sscanf(argv[6], "%s", s.addr);
			sscanf(argv[7], "%s", s.email);

			appendRecord(fp, s.id, s.name, s.dept, s.addr, s.email);
			fclose(fp);
			
			break;
		default:
			fprintf(stderr, "Wrong Option!\n");
			break;
	}
	fclose(fp);
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
	int reservedArea = 0;

	// ��� �о� ���̱�
	fseek(fp, 0, SEEK_SET);
	memset(headerbuf, 0, sizeof(HEADER_SIZE));
	fread(headerbuf, HEADER_SIZE, 1, fp);
	memcpy(&count, headerbuf, sizeof(int));
	memcpy(&reservedArea, headerbuf + sizeof(int), sizeof(int));

	if(count == 0){
		printf("There was no Head\n");
		count++;
		memset(headerbuf, 0, sizeof(HEADER_SIZE));
		memcpy(headerbuf, &count, sizeof(int));
		memcpy(headerbuf + 4, &reservedArea, sizeof(int));
		fseek(fp, 0, SEEK_SET);
		fwrite(headerbuf, HEADER_SIZE, 1, fp);
		if(writeRecord(fp, &s, 1) > 0) return 1;
	} else {
		printf("DATA Existed\n");
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

	printf("%d \n", HEADER_SIZE + RECORD_SIZE * rrn);
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

//
// �л� ���ڵ� ���Ͽ��� �˻� Ű���� �����ϴ� ���ڵ尡 �����ϴ����� sequential search ����� 
// ���� ã�Ƴ���, �̸� �����ϴ� ��� ���ڵ��� ������ ����Ѵ�. �˻� Ű�� �л� ���ڵ带 �����ϴ�
// � �ʵ嵵 �����ϴ�. ���������� readRecord() �Լ��� ȣ���Ͽ� sequential search�� �����Ѵ�.
// �˻� ����� ����� �� �ݵ�� printRecord() �Լ��� ����Ѵ�. (�ݵ�� ���Ѿ� �ϸ�, �׷���
// �ʴ� ��� ä�� ���α׷����� �ڵ������� Ʋ�� ������ �ν���)
//
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


	char idbuf[8];
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
			fread(idbuf, 8, 1, fp);
			printf("IDbuf : %s \n", idbuf);
			printf("Keyvalue : %s\n", keyval);
			printf("strcmp : %d\n", strcmp(keyval, idbuf));
			if(strcmp(keyval, idbuf) == -3){
				printf("In\n");
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == NAME){
		for(int i = 0; i<rrn; i++){
			fseek(fp, 8 + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(namebuf, 10, 1, fp);
			printf("namebuf : %s\n", namebuf);
			if(strcmp(keyval, namebuf) == -3){
				readRecord(fp, &s, i);
				printf("strcmp : %d\n", strcmp(keyval, namebuf));
				printRecord(&s);
			}	
		}
	}

	if(f == DEPT){
		for(int i = 0; i<rrn; i++){
			fseek(fp, 18 + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(deptbuf, 8, 1, fp);
			if(strcmp(keyval, deptbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == ADDR){
		for(int i = 0; i<rrn; i++){
			fseek(fp, 30 + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(addrbuf, 8, 1, fp);
			if(strcmp(keyval, addrbuf) == 0){
				readRecord(fp, &s, i);
				printRecord(&s);
			}	
		}
	}

	if(f == EMAIL){
		for(int i = 0; i<rrn; i++){
			fseek(fp, 60 + HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
			fread(emailbuf, 8, 1, fp);
			if(strcmp(keyval, emailbuf) == 0){
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
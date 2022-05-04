#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTbl addrmaptbl;
extern FILE *devicefp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl();

//
// flash memory�� ó�� ����� �� �ʿ��� �ʱ�ȭ �۾�, ���� ��� address mapping table�� ����
// �ʱ�ȭ ���� �۾��� �����Ѵ�
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	//
	// �߰������� �ʿ��� �۾��� ������ �����ϸ� �ǰ� ������ ���ص� ������
	//



	return;
}

//
// file system�� ���� FTL�� �����ϴ� write interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
//
void ftl_write(int lsn, char *sectorbuf)
{
// #ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
// 	print_addrmaptbl();
// #endif

	//
	// block mapping ������� overwrite�� �߻��ϸ� �̸� �ذ��ϱ� ���� �ݵ�� �ϳ��� empty block��
	// �ʿ��ϸ�, �ʱⰪ�� flash memory���� �� ������ block number�� �����
	// overwrite�� �ذ��ϰ� �� �� �翬�� reserved_empty_blk�� overwrite�� ���߽�Ų (invalid) block�� �Ǿ�� ��
	// ���� reserved_empty_blk�� �����Ǿ� �ִ� ���� �ƴ϶� ��Ȳ�� ���� ��� �ٲ� �� ����
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;

	int pbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int ppn = pbn * PAGES_PER_BLOCK + offset - 1;

	char pagebuf[PAGE_SIZE]; // Sectorũ���� ����

	// int cur_page = ((pbn * PAGES_PER_BLOCK + offset - 1) * PAGE_SIZE);			//cur page addr
	// int first_page = (pbn * PAGE_SIZE * PAGES_PER_BLOCK);					//first page
	// printf("First page %d \n", first_page );
	int check = addrmaptbl.pbn[pbn];

	SpareData sd = {lsn, ""};

	memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); // page�� sector��ŭ �־���
 	memmove(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE)); // page�� spare ��ŭ �־� page�ϼ�

		
	if(check == -1){
		dd_write(ppn, pagebuf);
	} else {
		while(1){

		}
	}


	return;
}

//
// file system�� ���� FTL�� �����ϴ� read interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
// 
void ftl_read(int lsn, char *sectorbuf)
{
// #ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
// 	print_addrmaptbl();
// #endif

	int pbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int ppn = pbn * PAGES_PER_BLOCK + offset - 1;
	if(addrmaptbl.pbn[pbn] == -1) return;

	char pagebuf[PAGE_SIZE];
	dd_read(ppn, pagebuf);
	memcpy(sectorbuf, pagebuf, sizeof(SECTOR_SIZE));

	return;
}

// //
// // for debugging
// //
// void print_block(int pbn)
// {
// 	char *pagebuf;
// 	SpareData *sdata;
// 	int i;
	
// 	pagebuf = (char *)malloc(PAGE_SIZE);
// 	sdata = (SpareData *)malloc(SPARE_SIZE);

// 	printf("Physical Block Number: %d\n", pbn);

// 	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
// 	{
// 		fread((void*)i, (unsigned long*)pagebuf, 1, devicefp);
// 		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
// 		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
// 	}

// 	free(pagebuf);
// 	free(sdata);

// 	return;
// }

// //
// // for debugging
// //
// void print_addrmaptbl()
// {
// 	int i;

// 	printf("Address Mapping Table: \n");
// 	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
// 	{
// 		if(addrmaptbl.pbn[i] >= 0)
// 		{
// 			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
// 		}
// 	}
// }

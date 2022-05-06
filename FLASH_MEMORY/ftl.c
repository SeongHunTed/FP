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
	int lbn;
	char check_buf[PAGE_SIZE];
	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	for(i = 0; i < BLOCKS_PER_DEVICE; i++){
		if(dd_read(PAGES_PER_BLOCK * i ,check_buf) == 1){
			memcpy(&lbn, check_buf+SECTOR_SIZE, 4);
			if(lbn != 0xffffffff){
				addrmaptbl.pbn[lbn] = i;
				printf("%d\n", addrmaptbl.pbn[lbn]);
			}
		} else {
			fprintf(stderr, "Read File Error!\n");
			exit(0);
		}
	}
	return;
}

//
// file system�� ���� FTL�� �����ϴ� write interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl();
#endif
	//
	// block mapping ������� overwrite�� �߻��ϸ� �̸� �ذ��ϱ� ���� �ݵ�� �ϳ��� empty block��
	// �ʿ��ϸ�, �ʱⰪ�� flash memory���� �� ������ block number�� �����
	// overwrite�� �ذ��ϰ� �� �� �翬�� reserved_empty_blk�� overwrite�� ���߽�Ų (invalid) block�� �Ǿ�� ��
	// ���� reserved_empty_blk�� �����Ǿ� �ִ� ���� �ƴ϶� ��Ȳ�� ���� ��� �ٲ� �� ����
	//

	int reserved_empty_blk = DATABLKS_PER_DEVICE; // ������� �� ������

	int lbn = lsn / PAGES_PER_BLOCK;								// �� �� = 0
	int offset = lsn % PAGES_PER_BLOCK;								// offset = 1
	int ppn = ((DATABLKS_PER_DEVICE) -1 - lbn) * PAGES_PER_BLOCK + offset;   // 15-1-0 * 4 + 1 = 57
	// int ppn = pbn * PAGES_PER_BLOCK + offset;					// physical page number
	
	char pagebuf[PAGE_SIZE];

	int check = addrmaptbl.pbn[lbn];

	SpareData sd = {lsn, ""};
		
	if(check == -1){		
		printf("Block is Empty!\n");	
		memset(pagebuf, (char)0xFF, PAGE_SIZE);									// ���� ����ִ� ���
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 							// page�� sector��ŭ �־���
		memcpy(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));		
		dd_write(ppn, pagebuf);
		addrmaptbl.pbn[lbn] = DATABLKS_PER_DEVICE - lbn -1;						// ���ּ� -> �����ּ� ���� �����Ͽ� ����
	} else {
		printf("Block overwritting\n");					// overwrite ��� data update

		int empty_blk_ppn = reserved_empty_blk * PAGES_PER_BLOCK; //60 -> 61 -> 62 -> 63
		int empty_check;
		
		for(int i = 0; i < PAGES_PER_BLOCK; i++){
			dd_read(empty_blk_ppn, pagebuf);
			memcpy(&empty_check, pagebuf+SECTOR_SIZE, 4);

			if(empty_check != -1){
				// ���� reserved block
				// ���� ��� üũ -> addrmaptbl
				// ���ο� ����� �� �ű�
				// �����ּұ��� ����
				// ��������
				while(1){
					srand(time(NULL));
					reserved_empty_blk = rand() % DATABLKS_PER_DEVICE;			// �������� ��� ����
					if(addrmaptbl.pbn[reserved_empty_blk] != -1) break;			// �������� ���� �� ���� ����� ��
				}
				break;
			}
			empty_blk_ppn++;
		}

		ppn = addrmaptbl.pbn[lbn] * PAGES_PER_BLOCK;
		empty_blk_ppn = reserved_empty_blk * PAGES_PER_BLOCK;
		printf("ppn : %d\n", ppn);
		printf("empty block : %d\n", reserved_empty_blk);

		for (int i = 0; i < PAGES_PER_BLOCK; i++){  // �� ���� ������ ����
			// if(++ppn % PAGES_PER_BLOCK == 0){
			// 	ppn = ppn - PAGES_PER_BLOCK;
			// 	empty_blk_ppn = empty_blk_ppn - PAGES_PER_BLOCK;
			// }
			printf("read & write %d to %d\n", ppn, empty_blk_ppn);
			dd_read(ppn++, pagebuf);
			dd_write(empty_blk_ppn++, pagebuf);
		}

		memset(pagebuf, (char)0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page�� sector��ŭ �־���
		memcpy(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));	

		dd_erase(addrmaptbl.pbn[lbn]);	// �ش� �� �ʱ�ȭ

		empty_blk_ppn = reserved_empty_blk * PAGES_PER_BLOCK + offset;
		dd_write(empty_blk_ppn, pagebuf);

		addrmaptbl.pbn[lbn] = reserved_empty_blk;
		
	}
	return;
}

//
// file system�� ���� FTL�� �����ϴ� read interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
// 
void ftl_read(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl();
#endif
	int lbn = lsn / PAGES_PER_BLOCK;					// 0
	int offset = lsn % PAGES_PER_BLOCK;					// 1 % 32 = 1
	int pbn = addrmaptbl.pbn[lbn];						// 1022
	
	int ppn = ((DATABLKS_PER_DEVICE) -1 - lbn) * PAGES_PER_BLOCK + offset;			// (1022 - lbn ) *pn + offset

	if(addrmaptbl.pbn[lbn] < 0) return;

	char pagebuf[PAGE_SIZE];
	dd_read(ppn, pagebuf);
	memcpy(pagebuf, sectorbuf, sizeof(SECTOR_SIZE));

	print_block(pbn);

	return;
}

//
// for debugging
//
void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(pagebuf);
	free(sdata);

	return;
}

//
// for debugging
//
void print_addrmaptbl()
{
	int i;

	printf("Address Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(addrmaptbl.pbn[i] >= 0)
		{
			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
		}
	}
}

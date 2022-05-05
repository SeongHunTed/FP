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
	int reserved_empty_blk = DATABLKS_PER_DEVICE;
	int empty_blk_ppn = reserved_empty_blk / PAGE_SIZE;

	int pbn = lsn / PAGES_PER_BLOCK;							// �� ��
	int offset = lsn % PAGES_PER_BLOCK;							// offset

	int ppn = pbn * PAGES_PER_BLOCK + offset;					// physical page number
	
	char pagebuf[PAGE_SIZE];

	int check = addrmaptbl.pbn[pbn];

	SpareData sd = {lsn, ""};
	
	memset(pagebuf, (char)0xFF, PAGE_SIZE);
	// memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page�� sector��ŭ �־���
	// memcpy(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));		// page�� spare ��ŭ �־� page�ϼ�

		
	if(check == -1){											// ���� ����ִ� ���
		// memset(pagebuf, (char)0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page�� sector��ŭ �־���
		memmove(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));		
		dd_write(ppn, pagebuf);
		addrmaptbl.pbn[pbn] = pbn;								// ���ּ� == �����ּ� �ϴ� ��ġȭ ����
	} else {
		empty_blk_ppn += offset;
		for (int i = 1; i < PAGES_PER_BLOCK; i++){  // �� ���� ������ ����
			if(++ppn % PAGES_PER_BLOCK == 0){
				ppn = ppn - PAGES_PER_BLOCK;
			}
			dd_read(ppn, pagebuf);
			dd_write(empty_blk_ppn++, pagebuf);
		}

		dd_erase(pbn);	// �ش� �� �ʱ�ȭ

		// ������ ������ ����
		for(int i = 0; i < PAGES_PER_BLOCK; i++){
			++empty_blk_ppn;
			++ppn;
			if(empty_blk_ppn % PAGES_PER_BLOCK == 0 && ppn % PAGES_PER_BLOCK == 0){
				empty_blk_ppn = empty_blk_ppn - PAGES_PER_BLOCK;
				ppn = ppn -PAGES_PER_BLOCK;
			}
			dd_read(empty_blk_ppn, pagebuf);
			dd_write(ppn, pagebuf);
		}

		dd_erase(reserved_empty_blk);

		// memset(pagebuf, (char)0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page�� sector��ŭ �־���
		memmove(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));	
		dd_write(ppn, pagebuf);
		addrmaptbl.pbn[pbn] = pbn;
		
		reserved_empty_blk = rand() % BLOCKS_PER_DEVICE;
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

	int lbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int ppn = pbn * PAGES_PER_BLOCK + offset;

	if(addrmaptbl.pbn[pbn] == -1) return;

	char pagebuf[PAGE_SIZE];
	dd_read(ppn, pagebuf);
	memcpy(sectorbuf, pagebuf, sizeof(SECTOR_SIZE));
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

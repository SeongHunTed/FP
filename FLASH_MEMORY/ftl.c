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
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
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
// file system을 위한 FTL이 제공하는 write interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_addrmaptbl();
#endif
	//
	// block mapping 기법에서 overwrite가 발생하면 이를 해결하기 위해 반드시 하나의 empty block이
	// 필요하며, 초기값은 flash memory에서 맨 마지막 block number를 사용함
	// overwrite를 해결하고 난 후 당연히 reserved_empty_blk는 overwrite를 유발시킨 (invalid) block이 되어야 함
	// 따라서 reserved_empty_blk는 고정되어 있는 것이 아니라 상황에 따라 계속 바뀔 수 있음
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;
	int empty_blk_ppn = reserved_empty_blk / PAGE_SIZE;

	int pbn = lsn / PAGES_PER_BLOCK;							// 논리 블럭
	int offset = lsn % PAGES_PER_BLOCK;							// offset

	int ppn = pbn * PAGES_PER_BLOCK + offset;					// physical page number
	
	char pagebuf[PAGE_SIZE];

	int check = addrmaptbl.pbn[pbn];

	SpareData sd = {lsn, ""};
	
	memset(pagebuf, (char)0xFF, PAGE_SIZE);
	// memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page에 sector만큼 넣어줌
	// memcpy(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));		// page에 spare 만큼 넣어 page완성

		
	if(check == -1){											// 블럭이 비어있는 경우
		// memset(pagebuf, (char)0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page에 sector만큼 넣어줌
		memmove(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));		
		dd_write(ppn, pagebuf);
		addrmaptbl.pbn[pbn] = pbn;								// 논리주소 == 물리주소 일단 일치화 기준
	} else {
		empty_blk_ppn += offset;
		for (int i = 1; i < PAGES_PER_BLOCK; i++){  // 빈 블럭에 데이터 복사
			if(++ppn % PAGES_PER_BLOCK == 0){
				ppn = ppn - PAGES_PER_BLOCK;
			}
			dd_read(ppn, pagebuf);
			dd_write(empty_blk_ppn++, pagebuf);
		}

		dd_erase(pbn);	// 해당 블럭 초기화

		// 복사한 데이터 쓰기
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
		memcpy(pagebuf, sectorbuf, sizeof(PAGE_SIZE)); 				// page에 sector만큼 넣어줌
		memmove(pagebuf + SECTOR_SIZE, &sd, sizeof(SPARE_SIZE));	
		dd_write(ppn, pagebuf);
		addrmaptbl.pbn[pbn] = pbn;
		
		reserved_empty_blk = rand() % BLOCKS_PER_DEVICE;
	}


	return;
}

//
// file system을 위한 FTL이 제공하는 read interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
// 
void ftl_read(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
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

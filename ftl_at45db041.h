#ifndef _FTL_AT45DB041_H
#define _FTL_AT45DB041_H

/*---------------INCLUDES-----------------------*/

typedef enum FTL_INFO
{
	CTRL_SYNC_ftl,
	GET_SECTOR_COUNT_ftl,
	GET_SECTOR_SIZE_ftl,
	GET_BLOCK_SIZE_ftl
}FTL_INFO;

void FTL_mapLogicalToPhysical(void);
void FTL_mkFTL(void);
void FTL_initTable(void);
uint32_t FTL_remapTable(uint32_t sector);
int FTL_writeSingleBlock(uint32_t sector,uint8_t* buffer);
int FTL_writeMultipleBlock(uint32_t sector,uint8_t* buffer, uint32_t count);
int FTL_readSingleBlock(uint32_t sector,uint8_t* buffer);
int FTL_readMultipleBlock(uint32_t sector,uint8_t* buffer, uint32_t count);

#endif
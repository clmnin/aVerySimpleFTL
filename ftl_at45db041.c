/*
    |       |                   |                   |
    |  FTL  |   Main Memory     |   Extra Memory    |
    |       |                   |                   |
*/

// TODO: We might have to have something to count the number of times it was erased

/*------------------INCLUDES-----------------------*/
#include <integer.h>
#include "ftl_at45db041.h"
/* other includes like the driver 
#include "at45/at45_fat.h   */

#define NUMBER_OF_SECTORS           1500
#define NUMBER_OF_EXTRA_SECTORS     500
#define MAIN_MEM_START_OFFSET       20
#define EXTRA_MEM_REGISTER			(18<<9)&(0x00FFE00)
#define EXTRA_MEM_START_OFFSET      1521
#define NO_OF_SECTORS_ftl			2048
#define ERASE_PAGE_256_ftl			256

/*The FTL memory
*/
typedef struct
{
    uint32_t mm_page[NUMBER_OF_SECTORS];
    uint32_t extra_page[NUMBER_OF_EXTRA_SECTORS];
    uint16_t next_extra_page;
}ftl_mem;

ftl_mem ftl;

void FTL_mapLogicalToPhysical(void)
{
    unsigned char buffer[3000];
    int i;
    at45_read(0, buffer, 3000); // 3K as each Main Memory page is stored in 2 bytes in the FTL table
    // now we need to move 2 bytes each into the struct of NUMBER_OF_SECTORS
	for(i=0; i<3000; i = i+2)
	{
		ftl.mm_page[i/2] = (buffer[i]<<8&0xff00)|(buffer[i+1]&0xff);
	}

    at45_read(EXTRA_MEM_REGISTER,buffer,256);
    ftl.next_extra_page = (buffer[0]<<8&0xff00)|(buffer[1]&0xff);

    // by this step we have the FTL table and the next extra page in RAM
}

void FTL_mkFTL(void)
{
	uint8_t buffer[3000], write_result;
	unsigned char buff[3000];
	int i;
	for(i=0; i<3000; i = i+2)
	{
		buffer[i] = ((i/2+EXTRA_MEM_START_OFFSET) & 0xFF00)>>8;
		buffer[i+1] = (i/2+EXTRA_MEM_START_OFFSET) & 0xFF;
	}
	write_result = at45_write_ftl(0,buffer,3000);
	buffer[0] = 0x5;
	buffer[1] = 0xF1;
	write_result = at45_write(EXTRA_MEM_REGISTER,buffer,2);
	at45_read(EXTRA_MEM_REGISTER, buff,2);

}

void FTL_initTable(void)
{
    //read from the FTL table into RAM
    FTL_mapLogicalToPhysical();
}

uint32_t FTL_remapTable(uint32_t sector)
{
    // we map the ftl table into another memory which exist in the extra space
    ftl_mem.mm_page[sector] = ftl_mem.next_extra_page;
    next_extra_page += 1;
    if(ftl_mem.next_extra_page > NO_OF_SECTORS_ftl)
    {
        // handle this
    }
    //write this into the ftl table

    // the next step is really bad, modify it for minimum erase
    at45_write(0,ftl_mem.mm_page, 1500); // this will update the whole memory, bad move

    // even this step is bad
    at45_write(FTL_EXTRA_MM_TABLE, &ftl_mem.next_extra_page, 1);

    return ftl_mem.next_extra_page;
}

int FTL_writeSingleBlock(uint32_t sector,uint8_t* buffer)
{
    uint32_t physical_page_number = 0;
    int write_status;

    physical_page_number = ftl_mem.mm_page[sector];
    write_status =  flash_WriteSingleBlock(physical_page_number, buffer);
    if( write_status == 0)
    {
        return 0; // that is the write operation was successful
    }
    else
    {
        // the write operation has failed,
        // maybe the page has worn out.
        sector = FTL_remapTable(sector); // remap the table
        // TODO: What if we have reached the limit of our extra sector
        physical_page_number = ftl_mem.mm_page[sector];
        write_status =  flash_WriteSingleBlock(physical_page_number, buffer);
        return write_status; // we will get a return 0 if the write was successful
    }
}

int FTL_writeMultipleBlock(uint32_t sector,uint8_t* buffer, uint32_t count)
{
    uint32_t physical_page_number = 0;
    int write_status;

    physical_page_number = ftl_mem.mm_page[sector];
    write_status =  flash_WriteMultipleBlock(physical_page_number, buffer, count);
    if( write_status == 0)
    {
        return 0; // that is the write operation was successful
    }
    else
    {
        // the write operation has failed,
        // maybe the page has worn out.
        sector = FTL_remapTable(sector);
         // remap the table
        // TODO: What if we have reached the limit of our extra sector
        physical_page_number = ftl_mem.mm_page[sector];
        write_status =  flash_WriteSingleBlock(physical_page_number, buffer);
        return write_status; // we will get a return 0 if the write was successful
    }
}

int FTL_readSingleBlock(uint32_t sector,uint8_t* buffer)
{
    uint32_t physical_page_number = 0;

    physical_page_number = ftl_mem.mm_page[sector];
    return flash_ReadSingleBlock(physical_page_number, buffer);
    // we shouldn't have any issue with reading, unless the hardware is not connected
}

int FTL_readMultipleBlock(uint32_t sector,uint8_t* buffer, uint32_t count)
{
    uint32_t physical_page_number = 0;

    physical_page_number = ftl_mem.mm_page[sector];
    return flash_ReadMultipleBlock(physical_page_number, buffer, count);
    // we shouldn't have any issue with reading, unless the hardware is not connected
}

int FTL_getMemoryInfo(FTL_INFO ctrl)
{
	switch(ctrl)
	{
	case GET_SECTOR_COUNT_ftl:
		return NUMBER_OF_SECTORS/2;
		break;
	case GET_SECTOR_SIZE_ftl:
		return ERASE_PAGE_256_ftl*2;
		break;
	case GET_BLOCK_SIZE_ftl:
		return 1;/*in no.of Sectors */
		break;
	default:
		break;
	}
	return 1;
}


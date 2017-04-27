/*
    |       |                   |                   |
    |  FTL  |   Main Memory    |   Extra Memory   |
    |       |                   |                   |
*/

// TODO: We might have to have something to count the number of times it was erased

/*The main memory FTL
*/
typedef struct
{
    uint16_t mm_page[128];
}ftl_mem;

/*The extra memory FTL
*/
typedef struct
{
    uint16_t next_extra_page;
}ftl_ext;

/*The main FTL structure
*/
typedef struct
{
    ftl_mem page_address;
    ftl_ext page_extra;
}ftl;

uint32_t FTL_mapLogicalToPhysical(uint32_t sector)
{
    uint16_t ftl_page_number = sector / 128;
    uint16_t ftl_mem_in_page = ft_page % 128;
    uint16_t ftl_maped_sector = 0;
    unsigned char * buffer;

    // read from that page which has our physical address
    flash_ReadSingleBlock(ftl_page_number, buffer); 
    // now at location ft_mem_in_page we have the phyical address 
    // corresponding to the logical address we received
    ftl.page_address.mm_page[sector] =  buffer[ftl_mem_in_page*2] // will this copy the next 16 bytes into mm_page?
}

uint32_t FTL_remapTable(uint32_t sector)
{
}

int FTL_writeSingleBLock(uint32_t sector,uint8_t* buffer)
{
    uint32_t phy_page_number = 0;
    int write_status;

    physical_page_number = FTL_mapLogicalToPhysical(sector);
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
        physical_page_number = FTL_mapLogicalToPhysical(sector);
        write_status =  flash_WriteSingleBlock(physical_page_number, buffer);
        return write_status; // we will get a return 0 if the write was successful
    }
}

int FTL_writeMultipleBLock(uint32_t sector,uint8_t* buffer, uint32_t count)
{
    uint32_t phy_page_number = 0;
    int write_status;

    physical_page_number = FTL_mapLogicalToPhysical(sector);
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
        physical_page_number = FTL_mapLogicalToPhysical(sector);
        write_status =  flash_WriteSingleBlock(physical_page_number, buffer);
        return write_status; // we will get a return 0 if the write was successful
    }
}

int FTL_readSingleBLock(uint32_t sector,uint8_t* buffer)
{
    uint32_t phy_page_number = 0;

    physical_page_number = FTL_mapLogicalToPhysical(sector);
    return flash_ReadSingleBlock(physical_page_number, buffer);
    // we shouldn't have any issue with reading, unless the hardware is not connected
}

int FTL_readMultipleBLock(uint32_t sector,uint8_t* buffer, uint32_t count)
{
    uint32_t phy_page_number = 0;

    physical_page_number = FTL_mapLogicalToPhysical(sector);
    return flash_ReadMultipleBlock(physical_page_number, buffer, count);
    // we shouldn't have any issue with reading, unless the hardware is not connected
}


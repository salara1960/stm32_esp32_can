#include "w25.h"


#ifdef SET_W25FLASH

/*
const uint16_t PageSize = 0x0100; // 256
const uint16_t SectorSize = 0x1000; // 4096
const uint16_t Block32KSize = 0x8000; // 32768
const uint32_t BlockSize = 0x00010000; // 65536
*/

//------------------------------------------------------------------------------------------
const char *TAGW25 = "W25";
w25qxx_t w25qxx;
const char *all_chipID[] = {
	"???",//0
	"W25Q10",//1
	"W25Q20",//2
	"W25Q40",//3
	"W25Q80",//4
	"W25Q16",//5
	"W25Q32",//6
	"W25Q64",//7
	"W25Q128",//8
	"W25Q256",//9
	"W25Q512"//10
};
const uint32_t all_chipBLK[] = {
	0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
};
const uint8_t max_param_type = 6;
const char *typeNames[] = {"BIT8","BIT16","BIT32","BIT64","BITX", "???"};
//------------------------------------------------------------------------------------------
uint8_t W25qxx_Spi(uint8_t Data)
{
uint8_t ret;

    HAL_SPI_TransmitReceive(portFLASH, &Data, &ret, 1, 10);//HAL_MAX_DELAY);

    return ret;
}
//------------------------------------------------------------------------------------------
uint32_t W25qxx_ReadID(void)
{
uint32_t Temp[3] = {0};

    W25_SELECT();//set to 0

    W25qxx_Spi(0x9F);
    Temp[0] = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    Temp[1] = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    Temp[2] = W25qxx_Spi(W25QXX_DUMMY_BYTE);

    W25_UNSELECT();//set to 1

    return ((Temp[0] << 16) | (Temp[1] << 8) | Temp[2]);
}
//------------------------------------------------------------------------------------------
void W25qxx_ReadUniqID(void)
{
    W25_SELECT();

    W25qxx_Spi(0x4B);

    for (uint8_t i = 0; i < 4; i++) W25qxx_Spi(W25QXX_DUMMY_BYTE);
    for (uint8_t i = 0; i < 8; i++) w25qxx.UniqID[i] = W25qxx_Spi(W25QXX_DUMMY_BYTE);

    W25_UNSELECT();
}
//------------------------------------------------------------------------------------------
void W25qxx_WriteEnable(void)
{
    W25_SELECT();

    W25qxx_Spi(0x06);

    W25_UNSELECT();

    W25qxx_Delay(1);
}
//------------------------------------------------------------------------------------------
void W25qxx_WriteDisable(void)
{
    W25_SELECT();

    W25qxx_Spi(0x04);

    W25_UNSELECT();

    W25qxx_Delay(1);
}
//------------------------------------------------------------------------------------------
uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusReg)
{
uint8_t status = 0;

    W25_SELECT();

    switch (SelectStatusReg) {
        case 1:
            W25qxx_Spi(0x05);
            status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
            w25qxx.StatusRegister1 = status;
        break;
        case 2:
            W25qxx_Spi(0x35);
            status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
            w25qxx.StatusRegister2 = status;
        break;
        default : {
            W25qxx_Spi(0x15);
            status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
            w25qxx.StatusRegister3 = status;
        }
    }

    W25_UNSELECT();

    return status;
}
//------------------------------------------------------------------------------------------
void W25qxx_WriteStatusRegister(uint8_t SelectStatusReg, uint8_t Data)
{
    W25_SELECT();

    switch (SelectStatusReg) {
        case 1 :
            W25qxx_Spi(0x01);
            w25qxx.StatusRegister1 = Data;
        break;
        case 2 :
            W25qxx_Spi(0x31);
            w25qxx.StatusRegister2 = Data;
        break;
        default : {
            W25qxx_Spi(0x11);
            w25qxx.StatusRegister3 = Data;
        }
    }

    W25qxx_Spi(Data);

    W25_UNSELECT();
}
//------------------------------------------------------------------------------------------
void W25qxx_WaitForWriteEnd(void)
{
    W25qxx_Delay(1);

    W25_SELECT();

    W25qxx_Spi(0x05);
    do
    {
        w25qxx.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
        W25qxx_Delay(1);
    } while ((w25qxx.StatusRegister1 & 0x01) == 0x01);

    W25_UNSELECT();
}
//------------------------------------------------------------------------------------------
bool W25qxx_Init(bool prn)
{
    w25qxx.Lock = 1;
    bool ret = false;

    W25_UNSELECT();

    uint32_t id = W25qxx_ReadID() & 0xffff;

    if (prn) Report(TAGW25, true, "w25qxx Init Begin... Chip ID:0x%X%s", id, eol);

    id -= 0x4010; if (id > 0x0a) id = 0;
    w25qxx.ID         = id;              //W25Q10..W25Q512
    w25qxx.BlockCount = all_chipBLK[id]; //0..1024;

    if (prn) Report(TAGW25, true, "Chip %s:\r\n", all_chipID[id]);

    if (id) {
    	w25qxx.PageSize = 256;
    	w25qxx.SectorSize = 0x1000;
    	w25qxx.SectorCount = w25qxx.BlockCount * 16;
    	w25qxx.PageCount = (w25qxx.SectorCount * w25qxx.SectorSize) / w25qxx.PageSize;
    	w25qxx.BlockSize = w25qxx.SectorSize * 16;
    	w25qxx.CapacityInKiloByte = (w25qxx.SectorCount * w25qxx.SectorSize) / 1024;
    	W25qxx_ReadUniqID();
    	W25qxx_ReadStatusRegister(1);
    	W25qxx_ReadStatusRegister(2);
    	W25qxx_ReadStatusRegister(3);
    	ret = true;

    	if (prn) Report(NULL, false,"\tPage Size:\t%u bytes%s"
                 "\tPage Count:\t%u%s"
                 "\tSector Size:\t%u bytes%s"
                 "\tSector Count:\t%u%s"
                 "\tBlock Size:\t%u bytes%s"
                 "\tBlock Count:\t%u%s"
                 "\tCapacity:\t%u KBytes%s",
                 w25qxx.PageSize, eol,
                 w25qxx.PageCount, eol,
                 w25qxx.SectorSize, eol,
                 w25qxx.SectorCount, eol,
                 w25qxx.BlockSize, eol,
                 w25qxx.BlockCount, eol,
                 w25qxx.CapacityInKiloByte, eol);
    }

    w25qxx.Lock = 0;

    return ret;
}
//------------------------------------------------------------------------------------------
void W25qxx_EraseChip(bool prn)
{
	if (!w25qxx.ID) return;

    while (w25qxx.Lock) W25qxx_Delay(1);//wait unlock device

    w25qxx.Lock = 1;//lock device

    uint32_t StartTime = HAL_GetTick();
    if (prn) Report(TAGW25, true, "Begin erase chip '%s'...", all_chipID[w25qxx.ID]);

    W25qxx_WriteEnable();

    W25_SELECT();

    W25qxx_Spi(0xC7);

    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();
    if (prn) Report(NULL, false, "%s done after %u ms!%s", __func__, HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(10);

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
void W25qxx_EraseSector(uint32_t SectorAddr)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    uint32_t StartTime = HAL_GetTick();
    Report(TAGW25, true, "%s %u Begin...", __func__, SectorAddr);
#endif

    W25qxx_WaitForWriteEnd();
    SectorAddr = SectorAddr * w25qxx.SectorSize;
    W25qxx_WriteEnable();

    W25_SELECT();
    W25qxx_Spi(0x20);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((SectorAddr & 0xFF000000) >> 24);
    W25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
    W25qxx_Spi((SectorAddr & 0xFF00) >> 8);
    W25qxx_Spi(SectorAddr & 0xFF);
    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();

#ifdef W25QXX_DEBUG
    Report(NULL, false, " done after %u ms%s", HAL_GetTick() - StartTime, eol);
#endif
    W25qxx_Delay(1);

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
void W25qxx_EraseBlock(uint32_t BlockAddr)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s %u Begin...%s", __func__, BlockAddr, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif

    W25qxx_WaitForWriteEnd();
    BlockAddr = BlockAddr * w25qxx.SectorSize * 16;
    W25qxx_WriteEnable();

    W25_SELECT();
    W25qxx_Spi(0xD8);
    if(w25qxx.ID >= W25Q256) W25qxx_Spi((BlockAddr & 0xFF000000) >> 24);
    W25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
    W25qxx_Spi((BlockAddr & 0xFF00) >> 8);
    W25qxx_Spi(BlockAddr & 0xFF);
    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s done after %u ms%s", __func__, HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(100);
#else
    W25qxx_Delay(1);
#endif

    w25qxx.Lock = 0;
}
#endif
//------------------------------------------------------------------------------------------
uint32_t W25qxx_PageToSector(uint32_t PageAddress)
{
    return ((PageAddress * w25qxx.PageSize) / w25qxx.SectorSize);
}
//------------------------------------------------------------------------------------------
uint32_t W25qxx_PageToBlock(uint32_t PageAddress)
{
    return ((PageAddress * w25qxx.PageSize) / w25qxx.BlockSize);
}
//------------------------------------------------------------------------------------------
uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress)
{
    return ((SectorAddress * w25qxx.SectorSize) / w25qxx.BlockSize);
}
//------------------------------------------------------------------------------------------
uint32_t W25qxx_SectorToPage(uint32_t SectorAddress)
{
    return (SectorAddress * w25qxx.SectorSize) / w25qxx.PageSize;
}
//------------------------------------------------------------------------------------------
uint32_t W25qxx_BlockToPage(uint32_t BlockAddress)
{
    return (BlockAddress * w25qxx.BlockSize) / w25qxx.PageSize;
}
//------------------------------------------------------------------------------------------
bool W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{

    while(w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

    if ( ((NumByteToCheck_up_to_PageSize + OffsetInByte) > w25qxx.PageSize) ||
            (!NumByteToCheck_up_to_PageSize) )
                        NumByteToCheck_up_to_PageSize = w25qxx.PageSize - OffsetInByte;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckPage:0x%X(%u), Offset:%u, Bytes:%u begin...%s",
                 Page_Address, Page_Address, OffsetInByte, NumByteToCheck_up_to_PageSize, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif

    uint8_t pBuffer[32];
    uint32_t i, WorkAddress;
    for (i = OffsetInByte; i < w25qxx.PageSize; i += sizeof(pBuffer)) {

        W25_SELECT();
        WorkAddress = (i + Page_Address * w25qxx.PageSize);
        W25qxx_Spi(0x0B);
        if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
        W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
        W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
        W25qxx_Spi(WorkAddress & 0xFF);
        W25qxx_Spi(0);
        HAL_SPI_Receive(portFLASH, pBuffer, sizeof(pBuffer), 100);
        W25_UNSELECT();

        for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
            if (pBuffer[x] != 0xFF) goto NOT_EMPTY;
        }
    }
    if ((w25qxx.PageSize + OffsetInByte) % sizeof(pBuffer) != 0) {
        i -= sizeof(pBuffer);
        for ( ; i < w25qxx.PageSize; i++) {

        	W25_SELECT();
            WorkAddress = (i + Page_Address * w25qxx.PageSize);
            W25qxx_Spi(0x0B);
            if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
            W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
            W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
            W25qxx_Spi(WorkAddress & 0xFF);
            W25qxx_Spi(0);
            HAL_SPI_Receive(portFLASH, pBuffer, 1, 100);
            W25_UNSELECT();

            if (pBuffer[0] != 0xFF) goto NOT_EMPTY;
        }
    }

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckPage is Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return true;

NOT_EMPTY:

#ifdef W25QXX_DEBUG
	Report(TAGW25, true, "w25qxx CheckPage is Not Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
	W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return false;
}
//------------------------------------------------------------------------------------------
bool W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

    if ( (NumByteToCheck_up_to_SectorSize > w25qxx.SectorSize) || (!NumByteToCheck_up_to_SectorSize) )
                NumByteToCheck_up_to_SectorSize = w25qxx.SectorSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckSector:0x%X(%u), Offset:%u, Bytes:%u begin...%s",
                 Sector_Address, Sector_Address, OffsetInByte, NumByteToCheck_up_to_SectorSize, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif

    uint8_t pBuffer[32];
    uint32_t i, WorkAddress;
    for ( i = OffsetInByte; i < w25qxx.SectorSize; i += sizeof(pBuffer)) {

    	W25_SELECT();
        WorkAddress = (i + Sector_Address * w25qxx.SectorSize);
        W25qxx_Spi(0x0B);
        if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
        W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
        W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
        W25qxx_Spi(WorkAddress & 0xFF);
        W25qxx_Spi(0);
        HAL_SPI_Receive(portFLASH, pBuffer, sizeof(pBuffer), 100);
        W25_UNSELECT();

        for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
            if (pBuffer[x] != 0xFF) goto NOT_EMPTY;
        }
    }
    if ((w25qxx.SectorSize + OffsetInByte) % sizeof(pBuffer) != 0) {
        i -= sizeof(pBuffer);
        for( ; i < w25qxx.SectorSize; i++) {

            W25_SELECT();
            WorkAddress = (i + Sector_Address * w25qxx.SectorSize);
            W25qxx_Spi(0x0B);
            if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
            W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
            W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
            W25qxx_Spi(WorkAddress & 0xFF);
            W25qxx_Spi(0);
            HAL_SPI_Receive(portFLASH, pBuffer, 1, 100);
            W25_UNSELECT();

            if (pBuffer[0] != 0xFF) goto NOT_EMPTY;
        }
    }

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckSector is Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return true;

NOT_EMPTY:

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckSector is Not Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return false;
}
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
bool W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

    if ( (NumByteToCheck_up_to_BlockSize > w25qxx.BlockSize) || !NumByteToCheck_up_to_BlockSize )
                          NumByteToCheck_up_to_BlockSize = w25qxx.BlockSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckBlock:0x%X(%u), Offset:%u, Bytes:%u begin...%s",
                 Block_Address, Block_Address, OffsetInByte, NumByteToCheck_up_to_BlockSize, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif

    uint8_t pBuffer[32];
    uint32_t i, WorkAddress;
    for (i = OffsetInByte; i < w25qxx.BlockSize; i += sizeof(pBuffer)) {

        W25_SELECT();
        WorkAddress = (i + Block_Address * w25qxx.BlockSize);
        W25qxx_Spi(0x0B);
        if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
        W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
        W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
        W25qxx_Spi(WorkAddress & 0xFF);
        W25qxx_Spi(0);
        HAL_SPI_Receive(portFLASH, pBuffer, sizeof(pBuffer), 100);
        W25_UNSELECT();

        for (uint8_t x = 0; x < sizeof(pBuffer); x++) {
            if(pBuffer[x] != 0xFF) goto NOT_EMPTY;
        }
    }
    if ((w25qxx.BlockSize + OffsetInByte) % sizeof(pBuffer) != 0) {
        i -= sizeof(pBuffer);
        for ( ; i < w25qxx.BlockSize; i++) {

            W25_SELECT();
            WorkAddress = (i + Block_Address * w25qxx.BlockSize);
            W25qxx_Spi(0x0B);
            if (w25qxx.ID >= W25Q256) W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
            W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
            W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
            W25qxx_Spi(WorkAddress & 0xFF);
            W25qxx_Spi(0);
            HAL_SPI_Receive(portFLASH, pBuffer, 1, 100);
            W25_UNSELECT();

            if (pBuffer[0] != 0xFF) goto NOT_EMPTY;
        }
    }

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "w25qxx CheckBlock is Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
    W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return true;

NOT_EMPTY:

#ifdef W25QXX_DEBUG
	Report(TAGW25, true, "w25qxx CheckBlock is Not Empty in %u ms%s", HAL_GetTick() - StartTime, eol);
	W25qxx_Delay(100);
#endif

    w25qxx.Lock = 0;

    return false;
}
#endif
//------------------------------------------------------------------------------------------
void W25qxx_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    uint32_t StartTime = HAL_GetTick();
    Report(TAGW25, true, "%s 0x%02X at address %d begin...", __func__, pBuffer, WriteAddr_inBytes);
#endif

    W25qxx_WaitForWriteEnd();
    W25qxx_WriteEnable();

    W25_SELECT();
    W25qxx_Spi(0x02);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((WriteAddr_inBytes & 0xFF000000) >> 24);
    W25qxx_Spi((WriteAddr_inBytes & 0xFF0000) >> 16);
    W25qxx_Spi((WriteAddr_inBytes & 0xFF00) >> 8);
    W25qxx_Spi(WriteAddr_inBytes & 0xFF);
    W25qxx_Spi(pBuffer);
    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s done after %d ms%s", __func__, HAL_GetTick() - StartTime, eol);
#endif

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

    if ( ((NumByteToWrite_up_to_PageSize + OffsetInByte) > w25qxx.PageSize) || !NumByteToWrite_up_to_PageSize )
                NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;
    if ( (OffsetInByte + NumByteToWrite_up_to_PageSize) > w25qxx.PageSize )
                NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s WritePage:0x%X(%u), Offset:%u ,Writes %u Bytes, begin...%s",
                 __func__, Page_Address, Page_Address, OffsetInByte, NumByteToWrite_up_to_PageSize, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif
    //
    //
    W25qxx_WaitForWriteEnd();
    W25qxx_WriteEnable();

    W25_SELECT();
    W25qxx_Spi(0x02);
    Page_Address = (Page_Address * w25qxx.PageSize) + OffsetInByte;
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
    W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
    W25qxx_Spi((Page_Address & 0xFF00) >> 8);
    W25qxx_Spi(Page_Address&0xFF);
    HAL_SPI_Transmit(portFLASH, pBuffer, NumByteToWrite_up_to_PageSize, 100);
    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();

#ifdef W25QXX_DEBUG
    StartTime = HAL_GetTick() - StartTime;
    for (uint32_t i = 0; i < NumByteToWrite_up_to_PageSize ; i++) {
    	if ( (i % 16 == 0) && (i > 2) ) {
    		Report(NULL, false, eol);
    		W25qxx_Delay(10);
    	}
    	Report(NULL, false, "0x%02X,", pBuffer[i]);
    }
    Report(NULL, false, eol);
    Report(TAGW25, true, "%s done after %u ms%s", __func__, StartTime, eol);
    W25qxx_Delay(100);
#else
    W25qxx_Delay(1);
#endif

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
    if ((NumByteToWrite_up_to_SectorSize > w25qxx.SectorSize) || !NumByteToWrite_up_to_SectorSize)
                NumByteToWrite_up_to_SectorSize = w25qxx.SectorSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s WriteSector:0x%X(%u), Offset:%u ,Write %u Bytes, begin...%s",
                 __func__, Sector_Address, Sector_Address, OffsetInByte, NumByteToWrite_up_to_SectorSize, eol);
    W25qxx_Delay(100);
#endif

    if (OffsetInByte >= w25qxx.SectorSize) {
#ifdef W25QXX_DEBUG
    	Report(TAGW25, true, "---w25qxx WriteSector Faild!%s", eol);
    	W25qxx_Delay(100);
#endif
        return;
    }

    int32_t BytesToWrite;
    uint32_t LocalOffset, StartPage;
    if ((OffsetInByte + NumByteToWrite_up_to_SectorSize) > w25qxx.SectorSize)
        BytesToWrite = w25qxx.SectorSize - OffsetInByte;
    else
        BytesToWrite = NumByteToWrite_up_to_SectorSize;
    StartPage = W25qxx_SectorToPage(Sector_Address) + (OffsetInByte / w25qxx.PageSize);
    LocalOffset = OffsetInByte % w25qxx.PageSize;

    do
    {
        W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
        StartPage++;
        BytesToWrite -= w25qxx.PageSize - LocalOffset;
        pBuffer += w25qxx.PageSize;
        LocalOffset = 0;
    } while(BytesToWrite > 0);

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s Done%s", __func__, eol);
    W25qxx_Delay(100);
#endif

}
#endif
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
void W25qxx_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
    if ((NumByteToWrite_up_to_BlockSize > w25qxx.BlockSize) || !NumByteToWrite_up_to_BlockSize)
            NumByteToWrite_up_to_BlockSize = w25qxx.BlockSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s WriteBlock:0x%X(%u), Offset:%u ,Write %u Bytes, begin...%s",
                 __func__, Block_Address, Block_Address, OffsetInByte, NumByteToWrite_up_to_BlockSize, eol);
    W25qxx_Delay(100);
#endif

    if (OffsetInByte >= w25qxx.BlockSize) {
#ifdef W25QXX_DEBUG
    	Report(TAGW25, true, "%s Faild!%s", __func__, eol);
    	W25qxx_Delay(100);
#endif
        return;
    }

    int32_t BytesToWrite;
    uint32_t LocalOffset, StartPage;
    if ((OffsetInByte + NumByteToWrite_up_to_BlockSize) > w25qxx.BlockSize)
        BytesToWrite = w25qxx.BlockSize - OffsetInByte;
    else
        BytesToWrite = NumByteToWrite_up_to_BlockSize;
    StartPage = W25qxx_BlockToPage(Block_Address) + (OffsetInByte / w25qxx.PageSize);
    LocalOffset = OffsetInByte % w25qxx.PageSize;
    do
    {
        W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
        StartPage++;
        BytesToWrite -= w25qxx.PageSize - LocalOffset;
        pBuffer += w25qxx.PageSize;
        LocalOffset = 0;
    } while(BytesToWrite > 0);

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s done%s", __func__, eol);
    W25qxx_Delay(100);
#endif

}
#endif
//------------------------------------------------------------------------------------------
void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    uint32_t StartTime = HAL_GetTick();
    Report(TAGW25, true, "%s at address %u begin...%s", __func__, Bytes_Address, eol);
#endif

    W25_SELECT();
    W25qxx_Spi(0x0B);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((Bytes_Address & 0xFF000000) >> 24);
    W25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
    W25qxx_Spi((Bytes_Address& 0xFF00) >> 8);
    W25qxx_Spi(Bytes_Address & 0xFF);
    W25qxx_Spi(0);
    *pBuffer = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    W25_UNSELECT();

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s 0x%02X done after %u ms%s", __func__, *pBuffer, HAL_GetTick() - StartTime, eol);
#endif

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
void W25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    uint32_t StartTime = HAL_GetTick();
    Report(TAGW25, true, "%s at Address:0x%X(%u), %u Bytes  begin...%s",
    			__func__, ReadAddr, ReadAddr, NumByteToRead, eol);
#endif

    W25_SELECT();
    W25qxx_Spi(0x0B);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((ReadAddr & 0xFF000000) >> 24);
    W25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
    W25qxx_Spi((ReadAddr& 0xFF00) >> 8);
    W25qxx_Spi(ReadAddr & 0xFF);
    W25qxx_Spi(0);
    HAL_SPI_Receive(portFLASH, pBuffer, NumByteToRead, 2000);
    W25_UNSELECT();

#ifdef W25QXX_DEBUG
    StartTime = HAL_GetTick() - StartTime;
    for (uint32_t i = 0; i < NumByteToRead ; i++) {
    	if ((i % 16 == 0) && (i > 2)) {
    		Report(NULL, false, eol);
    		W25qxx_Delay(10);
    	}
    	Report(NULL, false, "0x%02X,", pBuffer[i]);
    }
    Report(NULL, false, eol);
    Report(TAGW25, true, "%s done after %u ms%s", __func__, StartTime, eol);
    W25qxx_Delay(100);
#else
    W25qxx_Delay(1);
#endif

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

    if ((NumByteToRead_up_to_PageSize > w25qxx.PageSize) || !NumByteToRead_up_to_PageSize)
        NumByteToRead_up_to_PageSize = w25qxx.PageSize;
    if ((OffsetInByte + NumByteToRead_up_to_PageSize) > w25qxx.PageSize)
        NumByteToRead_up_to_PageSize = w25qxx.PageSize - OffsetInByte;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s:0x%X(%u), Offset:%u ,Read %u Bytes, begin...%s",
                 __func__, Page_Address, Page_Address, OffsetInByte, NumByteToRead_up_to_PageSize, eol);
    W25qxx_Delay(100);
    uint32_t StartTime = HAL_GetTick();
#endif

    Page_Address = Page_Address * w25qxx.PageSize + OffsetInByte;

    W25_SELECT();
    W25qxx_Spi(0x0B);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
    W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
    W25qxx_Spi((Page_Address& 0xFF00) >> 8);
    W25qxx_Spi(Page_Address & 0xFF);
    W25qxx_Spi(0);
    HAL_SPI_Receive(portFLASH, pBuffer, NumByteToRead_up_to_PageSize, 100);
    W25_UNSELECT();

#ifdef W25QXX_DEBUG
    StartTime = HAL_GetTick() - StartTime;
    for (uint32_t i = 0; i < NumByteToRead_up_to_PageSize ; i++) {
    	if ((i % 16 == 0) && (i > 2)) {
    		Report(NULL, false, eol);
    		W25qxx_Delay(10);
    	}
    	Report(NULL,false, "0x%02X,", pBuffer[i]);
    }
    Report(NULL, false, eol);
    Report(TAGW25, true, "%s done after %u ms%s", __func__, StartTime, eol);
    W25qxx_Delay(100);
#else
    W25qxx_Delay(1);
#endif

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{
    if ((NumByteToRead_up_to_SectorSize > w25qxx.SectorSize) || !NumByteToRead_up_to_SectorSize)
                NumByteToRead_up_to_SectorSize = w25qxx.SectorSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s:0x%X(%u), Offset:%u ,Read %u Bytes, begin...%s",
                 __func__, Sector_Address, Sector_Address, OffsetInByte, NumByteToRead_up_to_SectorSize, eol);
    W25qxx_Delay(100);
#endif

    if (OffsetInByte >= w25qxx.SectorSize) {
#ifdef W25QXX_DEBUG
    	Report(TAGW25, true, "---w25qxx ReadSector Faild!%s", eol);
    	W25qxx_Delay(100);
#endif
        return;
    }

    int32_t BytesToRead;
    uint32_t LocalOffset, StartPage;
    if ((OffsetInByte + NumByteToRead_up_to_SectorSize) > w25qxx.SectorSize)
        BytesToRead = w25qxx.SectorSize - OffsetInByte;
    else
        BytesToRead = NumByteToRead_up_to_SectorSize;
    StartPage = W25qxx_SectorToPage(Sector_Address) + (OffsetInByte / w25qxx.PageSize);
    LocalOffset = OffsetInByte % w25qxx.PageSize;
    do {
        W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
        StartPage++;
        BytesToRead -= w25qxx.PageSize - LocalOffset;
        pBuffer += w25qxx.PageSize;
        LocalOffset = 0;
    } while(BytesToRead > 0);

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s done%s", __func__, eol);
    W25qxx_Delay(100);
#endif

}
#endif
//------------------------------------------------------------------------------------------
#ifdef W25QXX_MAX
void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize)
{
    if ((NumByteToRead_up_to_BlockSize > w25qxx.BlockSize) || !NumByteToRead_up_to_BlockSize)
        NumByteToRead_up_to_BlockSize = w25qxx.BlockSize;

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s:0x%X(%u), Offset:%u ,Read %u Bytes, begin...%s",
                 __func__, Block_Address, Block_Address, OffsetInByte, NumByteToRead_up_to_BlockSize, eol);
    W25qxx_Delay(100);
#endif

    if (OffsetInByte >= w25qxx.BlockSize) {
#ifdef W25QXX_DEBUG
    	Report(TAGW25, true, "%s Faild!%s", __func__, eol);
    	W25qxx_Delay(100);
#endif
        return;
    }

    int32_t BytesToRead;
    uint32_t LocalOffset, StartPage;
    if ((OffsetInByte + NumByteToRead_up_to_BlockSize) > w25qxx.BlockSize)
        BytesToRead = w25qxx.BlockSize - OffsetInByte;
    else
        BytesToRead = NumByteToRead_up_to_BlockSize;
    StartPage = W25qxx_BlockToPage(Block_Address) + (OffsetInByte / w25qxx.PageSize);
    LocalOffset = OffsetInByte % w25qxx.PageSize;
    do {
        W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
        StartPage++;
        BytesToRead -= w25qxx.PageSize - LocalOffset;
        pBuffer += w25qxx.PageSize;
        LocalOffset = 0;
    } while(BytesToRead > 0);

#ifdef W25QXX_DEBUG
    Report(TAGW25, true, "%s done%s", __func__, eol);
    W25qxx_Delay(100);
#endif

}
#endif
//------------------------------------------------------------------------------------------
void W25qxx_ErasePage(uint32_t PageAddr)
{
    while (w25qxx.Lock) W25qxx_Delay(1);

    w25qxx.Lock = 1;

#ifdef W25QXX_DEBUG
    uint32_t StartTime = HAL_GetTick();
    Report(TAGW25, true, "%s %u Begin...", __func__, PageAddr);
#endif

    W25qxx_WaitForWriteEnd();
    W25qxx_WriteEnable();
    PageAddr *= w25qxx.PageSize;

    W25_SELECT();
    W25qxx_Spi(0x20);
    if (w25qxx.ID >= W25Q256) W25qxx_Spi((PageAddr & 0xFF000000) >> 24);
    W25qxx_Spi((PageAddr & 0xFF0000) >> 16);
    W25qxx_Spi((PageAddr & 0xFF00) >> 8);
    W25qxx_Spi(PageAddr & 0xFF);
    W25_UNSELECT();

    W25qxx_WaitForWriteEnd();

#ifdef W25QXX_DEBUG
    Report(NULL, false, " done after %u ms%s", HAL_GetTick() - StartTime, eol);
#endif
    W25qxx_Delay(1);

    w25qxx.Lock = 0;
}
//------------------------------------------------------------------------------------------
void prnPage(uint32_t page, bool all)
{
uint16_t prn_len = 16;
uint8_t fb[MAX_PAGE_SIZE];

	if (all) prn_len = MAX_PAGE_SIZE;

	W25qxx_ReadPage(fb, page, 0, prn_len);
	char *stx = (char *)pvPortMalloc(prn_len << 2);//vPortFree(buff);
	if (stx) {
		sprintf(stx, "page=%lu :", page);
		if (prn_len > 16) sprintf(stx+strlen(stx), "%s", eol);
		for (uint16_t i = 0; i < prn_len; i++) {
			if ((i % 32 == 0) && (i > 2)) sprintf(stx+strlen(stx), "%s", eol);
			sprintf(stx+strlen(stx), " %02X", fb[i]);
		}
		Report(TAGW25, true, "%s%s", stx, eol);
		vPortFree(stx);
	}
}
//------------------------------------------------------------------------------------------
int W25qxx_saveParam(const char *name, void *data, int type, uint8_t len)//return pageAddr or -1
{
w25_page_t pt = {0};
int ret = -1, page = -1;

	if (!w25qxx.ID || !data || !len) return ret;

	for (uint32_t i = 0; i < w25qxx.SectorSize/w25qxx.PageSize; i++) {
		W25qxx_ReadPage((uint8_t *)&pt, i, 0, sizeof(w25_page_t));
		if (!pt.busy) {
			if (!strncmp(pt.name, name, strlen(name))) {
				page = i;
				break;
			}
		} else {
			if (ret == -1) ret = i;
		}
	}

	if (page < 0) {
		if (ret == -1) return ret; else page = ret;
	} else {
#ifdef W25QXX_DEBUG
		Report(TAGW25, true, "Param name '%s' present on page #%lu%s", name, page, eol);
#endif
		//W25qxx_Delay(10);
		W25qxx_ErasePage(page);
#ifdef W25QXX_DEBUG
		Report(TAGW25, true, "_ErasePage(%u) OK%s", page, eol);
#endif
	}
	ret = page;//page number !

	if (len > (w25qxx.PageSize - sizeof(w25_page_t))) len = w25qxx.PageSize - sizeof(w25_page_t);
	uint8_t *body = (uint8_t *)data;
	//W25qxx_Delay(10);
	W25qxx_WritePage(body, page, sizeof(w25_page_t), len);//sabe body
	//
	memset(pt.name, 0, sizeof(pt.name));
	int dl = strlen(name);
	if (dl > sizeof(pt.name)) dl = sizeof(pt.name);
	strncpy(pt.name, name, dl);
	pt.len = len;
	if (type >= max_param_type) type = max_param_type - 1;
	pt.type = (uint8_t)type;
	pt.busy = 0;
	//W25qxx_Delay(10);
	W25qxx_WritePage((uint8_t *)&pt, page, 0, sizeof(w25_page_t));//save header
#ifdef W25QXX_DEBUG
	char tmp[16] = {0};
	switch (type) {
		case typeBIT8:
			sprintf(tmp, "%u", *body);
		break;
		case typeBIT16:
			sprintf(tmp, "%u", *(uint16_t *)body);
		break;
		case typeBIT32:
			sprintf(tmp, "%lu", *(uint32_t *)body);
		break;
		case typeBIT64:
			sprintf(tmp, "%lld", *(uint64_t *)body);
		break;
	}
	Report(TAGW25, true, "_saveParam(%s, %s, %s, %u) -> write to page # %d%s", name, tmp, typeNames[pt.type], pt.len, ret, eol);
#endif

	return ret;
}
//------------------------------------------------------------------------------------------
int W25qxx_readParam(const char *name, void *data, uint8_t *type, uint8_t *len)//return pageAddr or -1
{
w25_page_t pt = {0};
int ret = -1, page = -1;

	if (!w25qxx.ID || !data) return ret;

	for (uint32_t i = 0; i < w25qxx.SectorSize/w25qxx.PageSize; i++) {
		W25qxx_ReadPage((uint8_t *)&pt, i, 0, sizeof(w25_page_t));
		if (!pt.busy) {
			if (!strncmp(pt.name, name, strlen(name))) {
				page = i;
				break;
			}
		}
	}

	if (page < 0) return ret;
	W25qxx_Delay(10);
	uint8_t *buf = (uint8_t *)pvPortMalloc(pt.len);//vPortFree(buff);
	if (buf) {
		W25qxx_ReadPage(buf, page, sizeof(w25_page_t), pt.len);
		memcpy((uint8_t *)data, buf, pt.len);
		*len = pt.len;
		*type = pt.type;
		ret = page;
		vPortFree(buf);
	}

	return ret;
}
//------------------------------------------------------------------------------------------
void AboutFlashChip()
{
	if (w25qxx.ID) Report(NULL, false,"Chip '%s':%s"
			     	 	 "\tPage Size:\t%u bytes%s"
                 	 	 "\tPage Count:\t%u%s"
                 	 	 "\tSector Size:\t%u bytes%s"
                 	 	 "\tSector Count:\t%u%s"
                 	 	 "\tBlock Size:\t%u bytes%s"
                 	 	 "\tBlock Count:\t%u%s"
                 	 	 "\tCapacity:\t%u KBytes%s",
						 all_chipID[w25qxx.ID], eol,
						 w25qxx.PageSize, eol,
						 w25qxx.PageCount, eol,
						 w25qxx.SectorSize, eol,
						 w25qxx.SectorCount, eol,
						 w25qxx.BlockSize, eol,
						 w25qxx.BlockCount, eol,
						 w25qxx.CapacityInKiloByte, eol);
}
//------------------------------------------------------------------------------------------
#endif

#ifndef __W25_H__
#define __W25_H__

#include "main.h"

#ifdef SET_W25FLASH


//#define W25QXX_DEBUG
//#define W25QXX_MAX

#define MAX_PAGE_SIZE 256

#define W25QXX_DUMMY_BYTE 0xA5

#define W25qxx_Delay(delay) HAL_Delay(delay)
#define W25_SELECT()   HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_RESET);//set to 0
#define W25_UNSELECT() HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_SET);  //set to 1

typedef enum {
    W25Q10 = 1,
    W25Q20,
    W25Q40,
    W25Q80,
    W25Q16,
    W25Q32,
    W25Q64,
    W25Q128,
    W25Q256,
    W25Q512,
} W25QXX_ID_t;

typedef enum {
	typeBIT8 = 0,
	typeBIT16,
	typeBIT32,
	typeBIT64,
	typeBITX
} param_t;

#pragma pack(push,1)
typedef struct {
    W25QXX_ID_t ID;
    uint8_t UniqID[8];

    uint16_t PageSize;
    uint32_t PageCount;
    uint32_t SectorSize;
    uint32_t SectorCount;
    uint32_t BlockSize;
    uint32_t BlockCount;

    uint32_t CapacityInKiloByte;

    uint8_t StatusRegister1;
    uint8_t StatusRegister2;
    uint8_t StatusRegister3;

    uint8_t Lock;
} w25qxx_t;
#pragma pack(pop)

//-------------------------------

#pragma pack(push,1)
typedef struct {
    char name[7];
    uint8_t type;
    uint8_t busy;
    uint8_t len;
//    uint8_t body[246];
} w25_page_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t type;
    uint8_t total;
} w25_hdr_page_t;
#pragma pack(pop)

//------------------------------------------------------------------------------------------
extern const char *TAGW25;
extern w25qxx_t w25qxx;
extern const char *all_chipID[];
extern const uint32_t all_chipBLK[];
extern const uint8_t max_param_type;
extern const char *typeNames[];
//------------------------------------------------------------------------------------------

extern bool W25qxx_Init(bool prn);

extern void W25qxx_EraseChip(bool prn);
extern void W25qxx_EraseSector(uint32_t SectorAddr);
#ifdef W25QXX_MAX
	extern void W25qxx_EraseBlock(uint32_t BlockAddr);
#endif

extern uint32_t W25qxx_PageToSector(uint32_t PageAddress);
extern uint32_t W25qxx_PageToBlock(uint32_t PageAddress);
extern uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress);
extern uint32_t W25qxx_SectorToPage(uint32_t SectorAddress);
extern uint32_t W25qxx_BlockToPage(uint32_t BlockAddress);

extern bool W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
extern bool W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
#ifdef W25QXX_MAX
	extern bool W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);
#endif

extern void W25qxx_WriteByte(uint8_t pBuffer, uint32_t Bytes_Address);
extern void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
extern void W25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);

extern void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
extern void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);

#ifdef W25QXX_MAX
	extern void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
	extern void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);

	extern void W25qxx_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);
	extern void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);
#endif

extern void W25qxx_ErasePage(uint32_t PageAddr);
extern int W25qxx_saveParam(const char *name, void *data, int type, uint8_t len);//return pageAddr or -1
extern int W25qxx_readParam(const char *name, void *data, uint8_t *type, uint8_t *len);//return pageAddr or -1
extern void prnPage(uint32_t p, bool all);
extern void AboutFlashChip();
//------------------------------------------------------------------------------------------


#endif

#endif /* __W25_H__ */

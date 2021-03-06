#include "ssd1306.h"


#if defined(SET_OLED_I2C) || defined(SET_OLED_SPI)

//------------------------------------------------------------------------

// font table
const uint8_t font8x8[128][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0000 (nul)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0001
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0002
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0003
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0004
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0005
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0006
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0007
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0008
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0009
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0010
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0011
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0012
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0013
    { 0x60, 0x38, 0x7E, 0x00, 0x7E, 0x1C, 0x06, 0x00 },   //{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0014
    { 0x60, 0x38, 0x7E, 0x3C, 0x7E, 0x1C, 0x06, 0x00 },   //{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0015
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0016
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0017
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0018
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0019
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001E
    { 0x00, 0x0E, 0x11, 0x11, 0x0E, 0x00, 0x00, 0x00 },   // U+001F (gradus)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0020 (space)
    { 0x00, 0x00, 0x06, 0x5F, 0x5F, 0x06, 0x00, 0x00 },   // U+0021 (!)
    { 0x00, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x00 },   // U+0022 (")
    { 0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14, 0x00 },   // U+0023 (#)
    { 0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12, 0x00, 0x00 },   // U+0024 ($)
    { 0x46, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x62, 0x00 },   // U+0025 (%)
    { 0x30, 0x7A, 0x4F, 0x5D, 0x37, 0x7A, 0x48, 0x00 },   // U+0026 (&)
    { 0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0027 (')
    { 0x00, 0x1C, 0x3E, 0x63, 0x41, 0x00, 0x00, 0x00 },   // U+0028 (()
    { 0x00, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00, 0x00 },   // U+0029 ())
    { 0x08, 0x2A, 0x3E, 0x1C, 0x1C, 0x3E, 0x2A, 0x08 },   // U+002A (*)
    { 0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08, 0x00, 0x00 },   // U+002B (+)
    { 0x00, 0x80, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00 },   // U+002C (,)
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00 },   // U+002D (-)
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00 },   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 },   // U+002F (/)
    { 0x3E, 0x7F, 0x71, 0x59, 0x4D, 0x7F, 0x3E, 0x00 },   // U+0030 (0)
    { 0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00 },   // U+0031 (1)
    { 0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00 },   // U+0032 (2)
    { 0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00 },   // U+0033 (3)
    { 0x18, 0x1C, 0x16, 0x53, 0x7F, 0x7F, 0x50, 0x00 },   // U+0034 (4)
    { 0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00 },   // U+0035 (5)
    { 0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30, 0x00, 0x00 },   // U+0036 (6)
    { 0x03, 0x03, 0x71, 0x79, 0x0F, 0x07, 0x00, 0x00 },   // U+0037 (7)
    { 0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00 },   // U+0038 (8)
    { 0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00 },   // U+0039 (9)
    { 0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 },   // U+003A (:)
    { 0x00, 0x80, 0xE6, 0x66, 0x00, 0x00, 0x00, 0x00 },   // U+003B (;)
    { 0x08, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00, 0x00 },   // U+003C (<)
    { 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00 },   // U+003D (=)
    { 0x00, 0x41, 0x63, 0x36, 0x1C, 0x08, 0x00, 0x00 },   // U+003E (>)
    { 0x02, 0x03, 0x51, 0x59, 0x0F, 0x06, 0x00, 0x00 },   // U+003F (?)
    { 0x3E, 0x7F, 0x41, 0x5D, 0x5D, 0x1F, 0x1E, 0x00 },   // U+0040 (@)
    { 0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C, 0x00, 0x00 },   // U+0041 (A)
    { 0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00 },   // U+0042 (B)
    { 0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63, 0x22, 0x00 },   // U+0043 (C)
    { 0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C, 0x00 },   // U+0044 (D)
    { 0x41, 0x7F, 0x7F, 0x49, 0x5D, 0x41, 0x63, 0x00 },   // U+0045 (E)
    { 0x41, 0x7F, 0x7F, 0x49, 0x1D, 0x01, 0x03, 0x00 },   // U+0046 (F)
    { 0x1C, 0x3E, 0x63, 0x41, 0x51, 0x73, 0x72, 0x00 },   // U+0047 (G)
    { 0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F, 0x00, 0x00 },   // U+0048 (H)
    { 0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00, 0x00 },   // U+0049 (I)
    { 0x30, 0x70, 0x40, 0x41, 0x7F, 0x3F, 0x01, 0x00 },   // U+004A (J)
    { 0x41, 0x7F, 0x7F, 0x08, 0x1C, 0x77, 0x63, 0x00 },   // U+004B (K)
    { 0x41, 0x7F, 0x7F, 0x41, 0x40, 0x60, 0x70, 0x00 },   // U+004C (L)
    { 0x7F, 0x7F, 0x0E, 0x1C, 0x0E, 0x7F, 0x7F, 0x00 },   // U+004D (M)
    { 0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F, 0x7F, 0x00 },   // U+004E (N)
    { 0x1C, 0x3E, 0x63, 0x41, 0x63, 0x3E, 0x1C, 0x00 },   // U+004F (O)
    { 0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F, 0x06, 0x00 },   // U+0050 (P)
    { 0x1E, 0x3F, 0x21, 0x71, 0x7F, 0x5E, 0x00, 0x00 },   // U+0051 (Q)
    { 0x41, 0x7F, 0x7F, 0x09, 0x19, 0x7F, 0x66, 0x00 },   // U+0052 (R)
    { 0x26, 0x6F, 0x4D, 0x59, 0x73, 0x32, 0x00, 0x00 },   // U+0053 (S)
    { 0x03, 0x41, 0x7F, 0x7F, 0x41, 0x03, 0x00, 0x00 },   // U+0054 (T)
    { 0x7F, 0x7F, 0x40, 0x40, 0x7F, 0x7F, 0x00, 0x00 },   // U+0055 (U)
    { 0x1F, 0x3F, 0x60, 0x60, 0x3F, 0x1F, 0x00, 0x00 },   // U+0056 (V)
    { 0x7F, 0x7F, 0x30, 0x18, 0x30, 0x7F, 0x7F, 0x00 },   // U+0057 (W)
    { 0x43, 0x67, 0x3C, 0x18, 0x3C, 0x67, 0x43, 0x00 },   // U+0058 (X)
    { 0x07, 0x4F, 0x78, 0x78, 0x4F, 0x07, 0x00, 0x00 },   // U+0059 (Y)
    { 0x47, 0x63, 0x71, 0x59, 0x4D, 0x67, 0x73, 0x00 },   // U+005A (Z)
    { 0x00, 0x7F, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00 },   // U+005B ([)
    { 0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00 },   // U+005C (\)
    { 0x00, 0x41, 0x41, 0x7F, 0x7F, 0x00, 0x00, 0x00 },   // U+005D (])
    { 0x08, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x08, 0x00 },   // U+005E (^)
    { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },   // U+005F (_)
    { 0x00, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 0x00 },   // U+0060 (`)
    { 0x20, 0x74, 0x54, 0x54, 0x3C, 0x78, 0x40, 0x00 },   // U+0061 (a)
    { 0x41, 0x7F, 0x3F, 0x48, 0x48, 0x78, 0x30, 0x00 },   // U+0062 (b)
    { 0x38, 0x7C, 0x44, 0x44, 0x6C, 0x28, 0x00, 0x00 },   // U+0063 (c)
    { 0x30, 0x78, 0x48, 0x49, 0x3F, 0x7F, 0x40, 0x00 },   // U+0064 (d)
    { 0x38, 0x7C, 0x54, 0x54, 0x5C, 0x18, 0x00, 0x00 },   // U+0065 (e)
    { 0x48, 0x7E, 0x7F, 0x49, 0x03, 0x02, 0x00, 0x00 },   // U+0066 (f)
    { 0x98, 0xBC, 0xA4, 0xA4, 0xF8, 0x7C, 0x04, 0x00 },   // U+0067 (g)
    { 0x41, 0x7F, 0x7F, 0x08, 0x04, 0x7C, 0x78, 0x00 },   // U+0068 (h)
    { 0x00, 0x44, 0x7D, 0x7D, 0x40, 0x00, 0x00, 0x00 },   // U+0069 (i)
    { 0x60, 0xE0, 0x80, 0x80, 0xFD, 0x7D, 0x00, 0x00 },   // U+006A (j)
    { 0x41, 0x7F, 0x7F, 0x10, 0x38, 0x6C, 0x44, 0x00 },   // U+006B (k)
    { 0x00, 0x41, 0x7F, 0x7F, 0x40, 0x00, 0x00, 0x00 },   // U+006C (l)
    { 0x7C, 0x7C, 0x18, 0x38, 0x1C, 0x7C, 0x78, 0x00 },   // U+006D (m)
    { 0x7C, 0x7C, 0x04, 0x04, 0x7C, 0x78, 0x00, 0x00 },   // U+006E (n)
    { 0x38, 0x7C, 0x44, 0x44, 0x7C, 0x38, 0x00, 0x00 },   // U+006F (o)
    { 0x84, 0xFC, 0xF8, 0xA4, 0x24, 0x3C, 0x18, 0x00 },   // U+0070 (p)
    { 0x18, 0x3C, 0x24, 0xA4, 0xF8, 0xFC, 0x84, 0x00 },   // U+0071 (q)
    { 0x44, 0x7C, 0x78, 0x4C, 0x04, 0x1C, 0x18, 0x00 },   // U+0072 (r)
    { 0x48, 0x5C, 0x54, 0x54, 0x74, 0x24, 0x00, 0x00 },   // U+0073 (s)
    { 0x00, 0x04, 0x3E, 0x7F, 0x44, 0x24, 0x00, 0x00 },   // U+0074 (t)
    { 0x3C, 0x7C, 0x40, 0x40, 0x3C, 0x7C, 0x40, 0x00 },   // U+0075 (u)
    { 0x1C, 0x3C, 0x60, 0x60, 0x3C, 0x1C, 0x00, 0x00 },   // U+0076 (v)
    { 0x3C, 0x7C, 0x70, 0x38, 0x70, 0x7C, 0x3C, 0x00 },   // U+0077 (w)
    { 0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0x44, 0x00 },   // U+0078 (x)
    { 0x9C, 0xBC, 0xA0, 0xA0, 0xFC, 0x7C, 0x00, 0x00 },   // U+0079 (y)
    { 0x4C, 0x64, 0x74, 0x5C, 0x4C, 0x64, 0x00, 0x00 },   // U+007A (z)
    { 0x08, 0x08, 0x3E, 0x77, 0x41, 0x41, 0x00, 0x00 },   // U+007B ({)
    { 0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00 },   // U+007C (|)
    { 0x41, 0x41, 0x77, 0x3E, 0x08, 0x08, 0x00, 0x00 },   // U+007D (})
    { 0x02, 0x03, 0x01, 0x03, 0x02, 0x03, 0x01, 0x00 },   // U+007E (~)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }    // U+007F ( )
};

uint8_t invert = OLED_CMD_DISPLAY_NORMAL;
const uint32_t min_wait_ms = 350;
const uint32_t max_wait_ms = 1000;
const uint32_t sem_wait = portMAX_DELAY;

//******************************************************************************************
osStatus waitEvent()
{
#ifdef SET_MUTEX_LCD
    return osMutexWait(mutexLCD, sem_wait);
#else
	#ifdef SET_SEM_LCD
    	return osSemaphoreWait(semLCD, sem_wait);
	#else
    	return osOK;
	#endif
#endif
}
//-----------------------------------------------------------------------------------------
void doneEvent()
{
#ifdef SET_MUTEX_LCD
	osMutexRelease(mutexLCD);
#else
	#ifdef SET_SEM_LCD
		osSemaphoreRelease(semLCD);
	#endif
#endif
}
//-----------------------------------------------------------------------------------------
#ifdef SET_OLED_SPI
void spi_ssd1306_Reset()
{
	CS_OLED_DESELECT();

	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);//RESET
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);//SET
	HAL_Delay(10);
}
//-----------------------------------------------------------------------------------------
HAL_StatusTypeDef spi_ssd1306_WriteCmds(uint8_t *cmds, uint16_t sz)
{
HAL_StatusTypeDef stat = HAL_OK;

	CS_OLED_SELECT();
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);//RESET
	stat = HAL_SPI_Transmit(portOLED, cmds, sz, HAL_MAX_DELAY);
	CS_OLED_DESELECT();

	return stat;
}
//-----------------------------------------------------------------------------------------
// Send data
HAL_StatusTypeDef spi_ssd1306_WriteData(const char *buf, uint16_t sz, bool with)
{
HAL_StatusTypeDef stat = HAL_OK;

	CS_OLED_SELECT();

	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);//SET
	if (with) {
		uint16_t cnt = 0;
		stat = HAL_SPI_Transmit_DMA(portOLED, (uint8_t *)buf, sz);
		while (HAL_SPI_GetState(portOLED) != HAL_SPI_STATE_READY) {
			HAL_Delay(1);
			cnt++;
			if (!cnt) break;
		}
	} else {
		stat = HAL_SPI_Transmit(portOLED, (uint8_t *)buf, sz, HAL_MAX_DELAY);
	}

	CS_OLED_DESELECT();
	return stat;
}
#endif
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
uint8_t ssd1306_calcx(int len)
{
uint8_t ret = 0;

    if ( (len > 0) && (len < 16) ) ret = ((16 - len) >> 1) + 1;

    return ret;
}
//-----------------------------------------------------------------------------------------
void ssd1306_on(bool flag)
{
uint8_t dat[] = {OLED_CONTROL_BYTE_CMD_SINGLE, 0};
HAL_StatusTypeDef rt = HAL_OK;

    if (flag) dat[1] = OLED_CMD_DISPLAY_ON;
    	 else dat[1] = OLED_CMD_DISPLAY_OFF;

    if (waitEvent() == osOK) {
#ifdef SET_OLED_SPI
    	rt = spi_ssd1306_WriteCmds(dat, sizeof(dat));
#endif
#ifdef SET_OLED_I2C
    	rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat, sizeof(dat), min_wait_ms);
#endif
    	doneEvent();
    }

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_init()
{
uint8_t dat[] = {
	OLED_CONTROL_BYTE_CMD_STREAM,//0x00
	OLED_CMD_SET_CHARGE_PUMP,    //0x8D
	0x14,
	OLED_CMD_SET_SEGMENT_REMAP,  //0xA1
	OLED_CMD_SET_COM_SCAN_MODE,  //0xC8
	OLED_CMD_SET_COLUMN_RANGE,   //0x21
	0x00,
	0x7F,
	OLED_CMD_SET_PAGE_RANGE,     //0x22
	0x00,
	0x07,
	OLED_CMD_DISPLAY_ON,         //0xAF
	invert
};
HAL_StatusTypeDef rt = HAL_OK;

	if (waitEvent() == osOK) {
#ifdef SET_OLED_SPI
		rt = spi_ssd1306_WriteCmds(dat, sizeof(dat));
#endif
#ifdef SET_OLED_I2C
		rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat, sizeof(dat), min_wait_ms);
#endif
		doneEvent();
	}

	i2cError = rt;
	if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
#ifdef SET_SSD1306_INVERT
void ssd1306_invert()
{
uint8_t dat[] = {OLED_CONTROL_BYTE_CMD_SINGLE, 0};
HAL_StatusTypeDef rt = HAL_OK;


    if (invert == OLED_CMD_DISPLAY_INVERTED) invert = OLED_CMD_DISPLAY_NORMAL;
										else invert = OLED_CMD_DISPLAY_INVERTED;
    dat[1] = invert;

    if (waitEvent() == osOK) {
#ifdef SET_OLED_SPI
    	rt = spi_ssd1306_WriteCmds(dat, sizeof(dat));
#endif
#ifdef SET_OLED_I2C
    	rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat, sizeof(dat), min_wait_ms);
#endif
    	doneEvent();
    }

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
#endif
//-----------------------------------------------------------------------------------------
void ssd1306_clear()
{
uint8_t i, dat[] = {OLED_CONTROL_BYTE_CMD_SINGLE, 0};
HAL_StatusTypeDef rt = HAL_OK;
uint8_t zero[129] = {0};

	zero[0] = OLED_CONTROL_BYTE_DATA_STREAM;
	if ( waitEvent()== osOK) {
		for (i = 0; i < 8; i++) {
			dat[1] = 0xB0 | i;
#ifdef SET_OLED_SPI
			rt  = spi_ssd1306_WriteCmds(dat, sizeof(dat));
			rt |= spi_ssd1306_WriteData((const char *)zero, sizeof(zero), 0);
#endif
#ifdef SET_OLED_I2C
			rt  = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat,    2, min_wait_ms);
			rt |= HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, zero, 129, max_wait_ms);
#endif
		}
		doneEvent();
	}

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_clear_line(uint8_t cy)
{
cy--;
HAL_StatusTypeDef rt = HAL_OK;
uint8_t cif_zero[] = {OLED_CONTROL_BYTE_DATA_STREAM, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t first[] = {
	OLED_CONTROL_BYTE_CMD_STREAM,
	OLED_CMD_SET_COLUMN_RANGE,
	0,
	0x7f,
	OLED_CMD_SET_PAGE_RANGE,
	cy,
	7
};
#ifdef SET_OLED_I2C
uint8_t next[] = {
	OLED_CONTROL_BYTE_CMD_SINGLE,
	0xB0 | cy
};
#endif

	if ( waitEvent()== osOK) {
#ifdef SET_OLED_SPI
		rt = spi_ssd1306_WriteCmds(first, sizeof(first));
		if (rt == HAL_OK) {
			for (uint8_t i = 0; i < 16; i++) {
				rt |= spi_ssd1306_WriteCmds(cif_zero, 1);
				rt |= spi_ssd1306_WriteData((const char *)&cif_zero[1], sizeof(cif_zero) - 1, 0);
			}
		}
#endif
#ifdef SET_OLED_I2C
		if (HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, first, sizeof(first), max_wait_ms) == HAL_OK) {
			if (HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, next, sizeof(next), min_wait_ms) == HAL_OK) {
				for (uint8_t i = 0; i < 16; i++) {
					rt |= HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, cif_zero, sizeof(cif_zero), max_wait_ms);
				}
			}
		}
#endif
		doneEvent();
	}

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_pattern()
{
uint8_t i, dat[] = {OLED_CONTROL_BYTE_CMD_SINGLE, 0};
uint8_t buf[129] = {0};
HAL_StatusTypeDef rt = HAL_OK;

	buf[0] = OLED_CONTROL_BYTE_DATA_STREAM;
	for (i = 1; i < 129; i++) buf[i] = 0xFF >> (i % 8);
	if (waitEvent() == osOK) {
		for (i = 0; i < 8; i++) {
			dat[1] = 0xB0 | i;
#ifdef SET_OLED_SPI
			rt  = spi_ssd1306_WriteCmds(dat, sizeof(dat));
			rt |= spi_ssd1306_WriteData((const char *)buf, sizeof(buf), 0);
#endif
#ifdef SET_OLED_I2C
			rt  = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat,   2, min_wait_ms);
			rt |= HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, buf, 129, max_wait_ms);
#endif
		}
		doneEvent();
	}

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_contrast(uint8_t value)//0xff or 0x00
{
uint8_t dat[] = {OLED_CONTROL_BYTE_CMD_STREAM, OLED_CMD_SET_CONTRAST, value};
HAL_StatusTypeDef rt = HAL_OK;

	if (waitEvent() == osOK) {
#ifdef SET_OLED_SPI
		rt = spi_ssd1306_WriteCmds(dat, sizeof(dat));
#endif
#ifdef SET_OLED_I2C
		rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat, sizeof(dat), min_wait_ms);
#endif
		doneEvent();
	}

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_text_xy(const char *stroka, uint8_t cx, uint8_t cy)
{
HAL_StatusTypeDef rt = HAL_OK;
uint8_t i, lin = cy - 1, col = cx - 1;
int len = strlen(stroka);
uint8_t dat[] = {OLED_CONTROL_BYTE_CMD_STREAM, 0, 0x10, 0};
uint8_t cif[] = {OLED_CONTROL_BYTE_DATA_STREAM, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t first[] = {
	OLED_CONTROL_BYTE_CMD_STREAM,
	OLED_CMD_SET_COLUMN_RANGE,
	col << 3,
	0x7f,
	OLED_CMD_SET_PAGE_RANGE,
	lin,
	7
};

	if (waitEvent() == osOK) {
#ifdef SET_OLED_I2C
		rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, first, sizeof(first), min_wait_ms);
		if (rt == HAL_OK) {
			/*dat[0] = OLED_CONTROL_BYTE_CMD_STREAM;
			dat[1] = 0;
			dat[2] = 0x10;
			dat[3] = 0;*/
			for (i = 0; i < len; i++) {
				if (stroka[i] == '\n') {
					dat[3] = 0xB0 | ++lin;
					rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, dat, sizeof(dat), min_wait_ms);
				} else {
					memcpy(&cif[1], &font8x8[(uint8_t)stroka[i]][0], 8);
					rt = HAL_I2C_Master_Transmit(portSSD, OLED_I2C_ADDRESS, cif, sizeof(cif), max_wait_ms);
				}
			}
		}
#endif
#ifdef SET_OLED_SPI
		lin = cy - 1;
		rt = spi_ssd1306_WriteCmds(first, sizeof(first));
		/*dat[0] = OLED_CONTROL_BYTE_CMD_STREAM;
		dat[1] = 0;
		dat[2] = 0x10;
		dat[3] = 0;*/
		for (i = 0; i < len; i++) {
			if (stroka[i] == '\n') {
				dat[3] = 0xB0 | ++lin;
				rt |= spi_ssd1306_WriteCmds(dat, sizeof(dat));
			} else {
				memcpy(&cif[1], &font8x8[(uint8_t)stroka[i]][0], 8);
				rt |= spi_ssd1306_WriteCmds(cif, 1);
				rt |= spi_ssd1306_WriteData((const char *)&cif[1], sizeof(cif) - 1, true);
			}
		}
#endif
		doneEvent();
	}

    i2cError = rt;
    if (i2cError) errLedOn(__func__);
}
//-----------------------------------------------------------------------------------------
void ssd1306_text(const char *stroka)
{
	if (stroka) {
		if (!i2cError) {
			ssd1306_text_xy(stroka, 1, 1);
		} else {
			errLedOn(__func__);
		}
	}
}
//******************************************************************************************

#endif


#include "hdr.h"
#ifdef SET_SSD1306

#include "main.h"


#define SDA_PIN GPIO_NUM_21//13 //15
#define SCL_PIN GPIO_NUM_22//14 //2
#define SSD1306_PORT I2C_NUM_0
#define I2C_CLOCK_HZ 800000 //800KHz //400000 //400KHz //1600000 - 1.6MHz  -> WORK !!!

xSemaphoreHandle lcd_mutex;

const char *TAG_OLED = "OLED";

uint8_t invert = OLED_CMD_DISPLAY_NORMAL;//0xA6 //OLED_CMD_DISPLAY_INVERTED - 0xA7

uint8_t font8x8[128][8] = {
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
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0014
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0015
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

//******************************************************************************************

void i2c_ssd1306_init()
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_CLOCK_HZ
    };
    i2c_param_config(SSD1306_PORT, &i2c_config);
    i2c_driver_install(SSD1306_PORT, I2C_MODE_MASTER, 0, 0, 0);
}
//-----------------------------------------------------------------------------------------
esp_err_t ssd1306_on(bool flag)
{
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
uint8_t bt;
esp_err_t ret = ESP_FAIL;

    if (flag) bt = OLED_CMD_DISPLAY_ON; else bt = OLED_CMD_DISPLAY_OFF;

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
        i2c_master_write_byte(cmd, bt, true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(SSD1306_PORT, cmd, 20/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }

    if (ret != ESP_OK) {
        if (flag) {
            ESP_LOGE(TAG_OLED, "[%s] Display ON ERROR ! (0x%.2X)", __func__, ret);
        } else {
            ESP_LOGE(TAG_OLED, "[%s] Display OFF ERROR ! (0x%.2X)", __func__, ret);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------------------
esp_err_t ssd1306_init()
{
esp_err_t ret = ESP_FAIL;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

        i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
        i2c_master_write_byte(cmd, 0x14, true);

        i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
        i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

        i2c_master_write_byte(cmd, OLED_CMD_SET_COLUMN_RANGE, true);
        i2c_master_write_byte(cmd, 0x00, true);
        i2c_master_write_byte(cmd, 0x7F, true);
        i2c_master_write_byte(cmd, OLED_CMD_SET_PAGE_RANGE, true);
        i2c_master_write_byte(cmd, 0x00, true);
        i2c_master_write_byte(cmd, 0x07, true);

        i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);

        i2c_master_write_byte(cmd, invert, true);//OLED_CMD_DISPLAY_INVERTED       0xA7

        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(SSD1306_PORT, cmd, 20/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG_OLED, "[%s] Display configuration failed. code: 0x%.2X", __func__, ret);
    }

    return ret;
}
//-----------------------------------------------------------------------------------------
void ssd1306_invert()
{
    if (invert == OLED_CMD_DISPLAY_INVERTED) invert = OLED_CMD_DISPLAY_NORMAL;
                                        else invert = OLED_CMD_DISPLAY_INVERTED;

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
        i2c_master_write_byte(cmd, invert, true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 20/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_clear()
{
i2c_cmd_handle_t cmd;
uint8_t i, zero[128] = {0};

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        for (i = 0; i < 8; i++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);

            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
            i2c_master_write_byte(cmd, 0xB0 | i, true);

            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
            i2c_master_write(cmd, zero, sizeof(zero), true);

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(SSD1306_PORT, cmd, 16/portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_clear_lines(uint8_t cy, uint8_t cnt)
{
i2c_cmd_handle_t cmd;
uint8_t i, zero[128] = {0};


    cy--;
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {


        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

        i2c_master_write_byte(cmd, OLED_CMD_SET_COLUMN_RANGE, true);
        i2c_master_write_byte(cmd, 0, true);
        i2c_master_write_byte(cmd, 0x7F, true);
        i2c_master_write_byte(cmd, OLED_CMD_SET_PAGE_RANGE, true);
        i2c_master_write_byte(cmd, cy, true);
        i2c_master_write_byte(cmd, 0x07, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        for (i = cy; i < (cy + cnt); i++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);

            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
            i2c_master_write_byte(cmd, 0xB0 | i, true);

            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
            i2c_master_write(cmd, zero, sizeof(zero), true);

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(SSD1306_PORT, cmd, 16/portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_pattern()
{
i2c_cmd_handle_t cmd;
uint8_t i, j;

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        for (i = 0; i < 8; i++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);

            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
            i2c_master_write_byte(cmd, 0xB0 | i, true);
            i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
            for (j = 0; j < 128; j++) i2c_master_write_byte(cmd, 0xFF >> (j % 8), true);

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_contrast(uint8_t value)//0xff or 0x00
{
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);

        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
        i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true);
        i2c_master_write_byte(cmd, value, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 20/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_shift(bool left, uint8_t line)
{
uint8_t dir;

    if (left) dir = 0x27; else dir = 0x26;
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);

        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

        i2c_master_write_byte(cmd, 0x2E, true);//before seting scroll area we must unset scroll //deactivate scroll (p29)

        i2c_master_write_byte(cmd, dir, true);//0x27 - left horizontal scroll, 0x26 - right horizontal scroll
        i2c_master_write_byte(cmd, 0x00, true);
        i2c_master_write_byte(cmd, 0x00, true);//begin page
        i2c_master_write_byte(cmd, 0x07, true);//speed......
        i2c_master_write_byte(cmd, 0x3F, true);//end page
        i2c_master_write_byte(cmd, 0X2F, true);// activate scroll (p29)

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_scroll(bool flag)
{
uint8_t byte;

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);

        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

        i2c_master_write_byte(cmd, 0x27, true);//0x29// vertical and horizontal scroll (p29)
        i2c_master_write_byte(cmd, 0x00, true);
        i2c_master_write_byte(cmd, 0x00, true);
        i2c_master_write_byte(cmd, 0x07, true);
        i2c_master_write_byte(cmd, 0x01, true);
        i2c_master_write_byte(cmd, 0x3F, true);

        i2c_master_write_byte(cmd, 0xA3, true);// set vertical scroll area (p30)
        i2c_master_write_byte(cmd, 0x20, true);//0x20
        i2c_master_write_byte(cmd, 0x40, true);//0x40

        if (flag) byte = 0x2F;// activate scroll (p29)
             else byte = 0x2E;// deactivate scroll (p29)
        i2c_master_write_byte(cmd, byte, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        xSemaphoreGive(lcd_mutex);
    }
}
//-----------------------------------------------------------------------------------------
void ssd1306_text_xy(const char *stroka, uint8_t cx, uint8_t cy)
{

uint8_t i, lin = cy - 1, col = cx - 1, len = strlen(stroka);
i2c_cmd_handle_t cmd;

    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

        i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

        i2c_master_write_byte(cmd, OLED_CMD_SET_COLUMN_RANGE, true);
        i2c_master_write_byte(cmd, col * 8, true);
        i2c_master_write_byte(cmd, 0x7F, true);
        i2c_master_write_byte(cmd, OLED_CMD_SET_PAGE_RANGE, true);
        i2c_master_write_byte(cmd, lin, true);
        i2c_master_write_byte(cmd, 0x07, true);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        for (i = 0; i < len; i++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);

            i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            if (stroka[i] == '\n') {
                i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
                i2c_master_write_byte(cmd, 0x00, true); // reset column
                i2c_master_write_byte(cmd, 0x10, true);//0x10
                i2c_master_write_byte(cmd, 0xB0 | ++lin, true); // increment page
            } else {
                i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
                i2c_master_write(cmd, font8x8[(uint8_t)stroka[i]], 8, true);
            }

            i2c_master_stop(cmd);
            i2c_master_cmd_begin(SSD1306_PORT, cmd, 10/portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }

        xSemaphoreGive(lcd_mutex);
    }

}
//-----------------------------------------------------------------------------------------
void ssd1306_text(const char *stroka)
{
    ssd1306_text_xy(stroka, 1, 1);
}
//-----------------------------------------------------------------------------------------
uint8_t ssd1306_calcx(int len)
{
uint8_t ret = 1;

    if ( (len > 0) && (len < 16) ) ret = ((16 - len) >> 1) + 1;

    return ret;
}
//******************************************************************************************

#endif

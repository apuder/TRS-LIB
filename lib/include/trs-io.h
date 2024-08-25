
#ifndef __TRS_IO_H__
#define __TRS_IO_H__

#include <defs.h>

extern int16_t trs_io_err;

#define TRS_IO_CORE_MODULE_ID 0

#define TRS_IO_SEND_VERSION 0
#define TRS_IO_SEND_STATUS 1
#define TRS_IO_CMD_CONFIGURE_WIFI 2
#define TRS_IO_SEND_WIFI_SSID 3
#define TRS_IO_SEND_WIFI_IP 4
#define TRS_IO_SET_LED 7
#define TRS_IO_GET_FPGA_VERSION 8

#define TRS_IO_STATUS_WIFI_NOT_NEEDED 0
#define TRS_IO_STATUS_WIFI_CONNECTING 1
#define TRS_IO_STATUS_WIFI_CONNECTED 2
#define TRS_IO_STATUS_WIFI_NOT_CONNECTED 3
#define TRS_IO_STATUS_WIFI_NOT_CONFIGURED 4
#define TRS_IO_STATUS_NO_TRS_IO 0xff

bool is_m3();
void wait_for_esp();
bool is_esp_done();
uint8_t trs_io_status();
void trs_io_version(uint8_t* revision, uint16_t* version);
void trs_io_set_led(bool r, bool g, bool b, bool blink, bool auto_off);
void trs_io_get_fpga_version(uint8_t* major, uint8_t* minor);

#endif

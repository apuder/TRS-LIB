
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

#define TRS_IO_STATUS_WIFI_NOT_NEEDED 0
#define TRS_IO_STATUS_WIFI_CONNECTING 1
#define TRS_IO_STATUS_WIFI_CONNECTED 2
#define TRS_IO_STATUS_WIFI_NOT_CONNECTED 3
#define TRS_IO_STATUS_WIFI_NOT_CONFIGURED 4
#define TRS_IO_STATUS_NO_TRS_IO 0xff

void wait_for_esp();
uint8_t trs_io_status();
void trs_io_version(uint8_t* revision, uint16_t* version);

#endif


#include <trs-io.h>
#include <inout.h>


int16_t trs_io_err;

__sfr __at 0xe0 PORT_0xE0;

bool is_m3()
{
  return *((volatile uint8_t*) 0x125) == 'I';
}

bool is_esp_done()
{
  if (is_m3()) {
    return !(PORT_0xE0 & 8);
  } else {
    return !(*((uint8_t*) 0x37e0) & 0x20);
  }
}

void wait_for_esp()
{
  while (!is_esp_done()) ;
}

uint8_t trs_io_status()
{
  uint16_t i = 0;
  uint8_t status = TRS_IO_STATUS_NO_TRS_IO;

  out31(TRS_IO_CORE_MODULE_ID);
  out31(TRS_IO_SEND_STATUS);

  while (++i != 0 && status == TRS_IO_STATUS_NO_TRS_IO) {
    status = in31();
  }
  return status;
}

void trs_io_version(uint8_t* revision, uint16_t* version)
{
  uint8_t version_minor;
  uint8_t version_major;

  out31(TRS_IO_CORE_MODULE_ID);
  out31(TRS_IO_SEND_VERSION);
  wait_for_esp();
  *revision = in31();
  version_major = in31();
  version_minor = in31();
  *version = (version_major << 8) | version_minor;
}

void trs_io_set_led(bool r, bool g, bool b, bool blink, bool auto_off)
{
  uint8_t m = 0;
  m |= r ? (1 << 0) : 0;
  m |= g ? (1 << 1) : 0;
  m |= b ? (1 << 2) : 0;
  m |= blink ? (1 << 3) : 0;
  m |= auto_off ? (1 << 4) : 0;
  out31(TRS_IO_CORE_MODULE_ID);
  out31(TRS_IO_SET_LED);
  out31(m);
  wait_for_esp();
}

void trs_io_get_fpga_version(uint8_t* major, uint8_t* minor)
{
  out31(TRS_IO_CORE_MODULE_ID);
  out31(TRS_IO_GET_FPGA_VERSION);
  wait_for_esp();
  *major = in31();
  *minor = in31();
}


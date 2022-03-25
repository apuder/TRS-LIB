
#include <trs-io.h>
#include <inout.h>


int16_t trs_io_err;

__sfr __at 0xe0 PORT_0xE0;

static inline bool is_m3()
{
  return *((uint8_t*) 0x125) == 'I';
}

void wait_for_esp()
{
  if (is_m3()) {
    while (PORT_0xE0 & 8) ;
  } else {
    while (*((uint8_t*) 0x37e0) & 0x20) ;
  }
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

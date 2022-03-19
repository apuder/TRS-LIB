
#include <trs-fs.h>
#include <trs-io.h>
#include <inout.h>

int8_t trs_fs_open(const char* path, uint8_t mode)
{
  out31(TRS_FS_ID);
  out31(FS_COMMAND_OPEN);
  do {
    out31(*path);
  } while (*path++ != '\0');
  out31(mode);
  wait_for_esp();
  trs_io_err = in31();
  if (trs_io_err != FR_OK) {
    return -1;
  }
  return in31();
}

int8_t trs_fs_read(uint8_t fd, void* buf, uint16_t bytes_to_read, uint16_t* bytes_read)
{
  uint8_t* p = (uint8_t*) buf;
  int i;

  out31(TRS_FS_ID);
  out31(FS_COMMAND_READ);
  out31(fd);
  out31(bytes_to_read & 0xff);
  out31(bytes_to_read >> 8);
  out31(0);
  out31(0);
  wait_for_esp();
  trs_io_err = in31();
  if(trs_io_err != FR_OK) {
    return -1;
  }
  *bytes_read = in31() | (in31() << 8);
  in31();
  in31();
  for(i = 0; i < *bytes_read; i++) {
    *p++ = in31();
  }
  return 0;
}

int8_t trs_fs_write(uint8_t fd, void* buf, uint16_t bytes_to_write, uint16_t* bytes_written)
{
  uint8_t* p = (uint8_t*) buf;
  uint16_t bw;
  int i;

  out31(TRS_FS_ID);
  out31(FS_COMMAND_WRITE);
  out31(fd);
  out31(bytes_to_write & 0xff);
  out31(bytes_to_write >> 8);
  out31(0);
  out31(0);
  for(i = 0; i < bytes_to_write; i++) {
    out31(*p++);
  }
  wait_for_esp();
  trs_io_err = in31();
  if (trs_io_err != FR_OK) {
    return -1;
  }
  bw = in31() | (in31() << 8);
  if (bytes_written != NULL) {
    *bytes_written = bw;
  }
  in31();
  in31();
  return 0;
}

int8_t trs_fs_close(uint8_t fd)
{
  out31(TRS_FS_ID);
  out31(FS_COMMAND_CLOSE);
  out31(fd);
  trs_io_err = in31();
  return (trs_io_err == FR_OK) ? 0 : -1;
}

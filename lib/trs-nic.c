
#include <trs-nic.h>
#include <trs-io.h>
#include <inout.h>


int8_t trs_nic_socket(uint8_t socket_family, uint8_t socket_type)
{
  out31(TRS_NIC_ID);
  out31(IP_COMMAND_SOCKET);
  out31(socket_family);
  out31(socket_type);
  wait_for_esp();
  if (in31() == IP_COMMAND_SUCCESS) {
    return in31();
  } else {
    trs_io_err = in31();
    return -1;
  }
}

int8_t trs_nic_connect(uint8_t socket, const char* host, uint16_t port)
{
  uint8_t b;

  out31(TRS_NIC_ID);
  out31(IP_COMMAND_CONNECT);
  out31(socket);
  do {
    out31(*host);
  } while (*host++ != '\0');
  out31(port & 0xff);
  out31(port >> 8);
  wait_for_esp();
  b = in31();
  if (b == IP_COMMAND_SUCCESS) {
    return 0;
  } else {
    trs_io_err = in31();
    return -1;
  }
}


int8_t trs_nic_send(uint8_t socket, void* buf, uint16_t bytes_to_send, uint16_t* bytes_sent)
{
  uint8_t* p = (uint8_t*) buf;
  uint16_t bs;
  int i;

  out31(TRS_NIC_ID);
  out31(IP_COMMAND_SEND);
  out31(socket);
  out31(bytes_to_send & 0xff);
  out31(bytes_to_send >> 8);
  out31(0);
  out31(0);
  for(i = 0; i < bytes_to_send; i++) {
    out31(*p++);
  }
  wait_for_esp();
  in31(); // Always IP_COMMAND_SUCCESS
  bs = in31() | (in31() << 8);
  if (bytes_sent != NULL) {
    *bytes_sent = bs;
  }
  in31();
  in31();
  return 0;
}


int8_t trs_nic_recv(uint8_t socket, uint8_t receive_option, void* buf, uint16_t bytes_to_receive, uint16_t* bytes_received)
{
  uint8_t* p = (uint8_t*) buf;
  int i;

  out31(TRS_NIC_ID);
  out31(IP_COMMAND_RECV);
  out31(socket);
  out31(receive_option);
  out31(bytes_to_receive & 0xff);
  out31(bytes_to_receive >> 8);
  out31(0);
  out31(0);
  wait_for_esp();
  if(in31() == IP_COMMAND_ERROR) {
    trs_io_err = in31();
    return -1;
  }
  *bytes_received = in31() | (in31() << 8);
  in31();
  in31();
  for(i = 0; i < *bytes_received; i++) {
    *p++ = in31();
  }
  return 0;
}


int8_t trs_nic_close(uint8_t socket)
{
  out31(TRS_NIC_ID);
  out31(IP_COMMAND_CLOSE);
  out31(socket);
  wait_for_esp();
  if (in31() == IP_COMMAND_SUCCESS) {
    return 0;
  } else {
    trs_io_err = in31();
    return -1;
  }
}


#include "demo.h"
#include <trs-lib.h>

static window_t wnd;

static char host[20 + 1] = {""};

static form_item_t my_form_items[] = {
	FORM_ITEM_INPUT("Host", host, sizeof(host) - 1, 0, NULL),
	FORM_ITEM_END
};

static form_t my_form = {
	.title = "TRS-NIC Demo",
	.form_items = my_form_items
};


void demo_trs_nic()
{
  static char buf[1024];
  int8_t sd;
  uint16_t br;
  uint8_t status;
  int i;

  set_screen_to_background();
  init_window(&wnd, 0, 3, 0, 0);
  header(my_form.title);
  wnd_print(&wnd, "This demo uses TRS-NIC's TCP socket API to connect to a whois server "
                  "to retrieve information about a particular domain.\n\n");

  status = trs_io_status();

  if (status == TRS_IO_STATUS_NO_TRS_IO) {
    wnd_popup("No TRS-IO card present!");
    screen_show(false);
    get_key();
    return;
  }

  if (status != TRS_IO_STATUS_WIFI_CONNECTED) {
    wnd_popup("TRS-IO not online!");
    screen_show(false);
    get_key();
    return;
  }

  status = form_inline(&wnd, &my_form, false);

  if (status == FORM_ABORT || host[0] == '\0') {
    return;
  }

  sd = trs_nic_socket(IP_SOCKET_FAMILY_AF_INET, IP_SOCKET_TYPE_SOCK_STREAM);
  if (sd < 0) {
    wnd_popup("Error: %d", trs_io_err);
    get_key();
    return;
  }

  wnd_cls(&wnd);

  trs_nic_connect(sd, "whois.iana.org", 43);
  trs_nic_send(sd, host, strlen(host), NULL);
  trs_nic_send(sd, "\r\n", 2, NULL);

  do {
    trs_nic_recv(sd, IP_RECV_BLOCKING, buf, sizeof(buf), &br);

    for(i = 0; i < br; i++) {
      wnd_print(&wnd, "%c", buf[i]);
    }
  } while(br != 0);

  trs_nic_close(sd);

  get_key();
}


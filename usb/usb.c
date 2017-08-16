#include "usb.h"

/*
 * USBドライバをロードする
 */
void usb_load(void)
{
  u_int i;
  for(i = 0; i < USB_MAX_DEVICES; i++) {
    usb_devices[i] = NULL;
  }
  for(i = 0; i < USB_INTERFACE_ATTACH_COUNT; i++) {
    //usb_interface[i] = NULL
  }
}

/*
 * USBドライバを初期化する
 */
void usb_init(void)
{
  
}

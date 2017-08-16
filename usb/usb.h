#ifndef SYS_SCREEN_H
#define SYS_SCREEN_H

#include "../include/types.h"

/*----- 定数定義 -----*/
#define USB_MAX_DEVICES 32

// USB通信の速度
typedef enum {
  HIGH = 0,
  FULL = 0,
  LOW  = 2,
} USB_SPEED;

// デバイスの状態
euum USB_DEVICE_STATUS {
  ATTACHED   = 0;
  POWERED    = 1;
  DEFAULT    = 2;
  ADDRESSED  = 3;
  CONFIGURED = 4;
};
// USB転送エラー
enum USB_TRANSFER_ERROR {
  NO_ERROR         = 0x000;
  STALL            = 0x002;
  BUFFER_ERROR     = 0x004;
  BABBLE           = 0x008;
  NO_ACKNOWLEDGE   = 0x010;
  CRC_ERROR        = 0x020;
  BIT_ERROR        = 0x040;
  CONNECTION_ERROR = 0x080;
  AHB_ERROR        = 0x100;
  NOTYET_ERROR     = 0x200;
  PROCESSING       = 0x400;
};

/*----- 構造体定義 -----*/
typedef struct {
  u_int number;
  USB_SPEED speed;
  enum USB_DEVICE_STATUS status;
  volatile u_char configIndex;
  u_char portNumber;
  volatile enum USB_TRANSFER_ERROR error __attribute__((aligned(4)));
  // [TODO] 追加
} USB_DEVICE;

/*----- 変数定義 -----*/
USB_DEVICE *usb_devices[USB_MAX_DEVICES];


#endif

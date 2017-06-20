/*
 * time.c - 時間関連
 */
#include "time.h"
#include "../hal/pit.h"

/*
 * 一定時間だけ待機する
 *
 * @param sec 待機時間(秒)
 */
void sleep(u_int sec)
{
  u_int past_sec;
  u_int start = pit_gettick();
  while(1) {
    past_sec = (pit_gettick() - start) / 100;
    if (past_sec > sec) break;
  }
}

/*
 * pit.c - PITの操作関数
 */
#include "pit.h"
#include "pic.h"
#include "hal.h"
#include "idt.h"
#include "irq.h"
#include "../include/io.h"

static volatile u_int pit_tick = 0;

/*
 * PITを初期化する
 */
void pit_init(void)
{
  // PIT割り込みハンドラ
  idt_setup_ir(32, irq_pit_bridge);
  
  // カウンタスタート
  pit_start_counter(100, PIT_OCW_COUNTER0, PIT_OCW_MODE_SQUAREWAVEGEN);
  //pit_start_counter(600, PIT_OCW_COUNTER0, PIT_OCW_MODE_SQUAREWAVEGEN);

  fb_print("[DEBUG] PIT init\n");
}

/*
 * タイマーをインクリメントする
 */
void pit_increment_tick(void)
{
  pit_tick++;
}

/*
 * タイマーを設定する
 *
 * @param t タイマーの値
 * @return 設定前の値
 */
u_int pit_settick(u_int t)
{
  u_int ret = pit_tick;
  pit_tick = t;
  return ret;
}

/*
 * 現在のタイマーを取得する
 *
 * @return タイマーの値
 */
u_int pit_gettick(void)
{
  return pit_tick;
}

/*
 * PITにコマンドを送信する
 *
 * @param cmd コマンド
 */
void pit_sendcmd(u_char cmd)
{
  outb(PIT_REG_COMMAND, cmd);
}

/*
 * カウンタにデータを送信する
 *
 * @param data    送信するデータ
 * @param counter 送信先のカウンタ
 */
void pit_senddata(u_short data, u_char counter)
{
  u_int port;
  switch(counter) {
  case PIT_OCW_COUNTER0:
    port = PIT_REG_COUNTER0;
    break;
  case PIT_OCW_COUNTER1:
    port = PIT_REG_COUNTER1;
    break;
  case PIT_OCW_COUNTER2:
    port = PIT_REG_COUNTER2;
    break;
  default:
    port = PIT_REG_COUNTER0;
  }
  outb(port, (u_char)data);
}

/*
 * カウンタからデータを受信する
 *
 * @param counter 受信元のカウンタ
 * @return 受信したデータ
 */
u_char pit_readdata(u_char counter)
{
  u_int port;
  switch(counter) {
  case PIT_OCW_COUNTER0:
    port = PIT_REG_COUNTER0;
    break;
  case PIT_OCW_COUNTER1:
    port = PIT_REG_COUNTER1;
    break;
  case PIT_OCW_COUNTER2:
    port = PIT_REG_COUNTER2;
    break;
  default:
    port = PIT_REG_COUNTER0;
  }
  return inb(port);
}

/*
 * カウンタを開始する
 *
 * @param freq    周波数
 * @param counter カウンタ
 * @param mode    モード
 */
void pit_start_counter(u_int freq, u_char counter, u_char mode)
{
  if (freq == 0) return;
  u_short divisor = (u_short)(1193181 / (u_short)freq);
  // 操作コマンドを送信
  u_char ocw = 0;
  ocw = (ocw & ~PIT_OCW_MASK_MODE) | mode;
  ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
  ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | counter;
  pit_sendcmd(ocw);
  // 周波数レートを設定
  pit_senddata(divisor & 0xFF, 0);
  pit_senddata((divisor >> 8) & 0xFF, 0);
  // リセット
  pit_settick(0);
}

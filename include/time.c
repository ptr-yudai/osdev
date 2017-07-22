/*
 * time.c - 時間関連
 */
#include "time.h"
#include "util.h"
#include "../hal/pit.h"

#include "../include/io.h"

/*
 * FILETIMEをUNIXTIMEに変換する
 *
 * @param filetime FILETIME構造体に従った日時(8byte)
 * @return unix time
 */
u_int64 ts_file2unix(u_int64 filetime)
{
  u_int64 ticks = filetime;
  if (ticks < TS_TICKS_TO_UNIX_EPOCH) {
    return -1;
  }
  ticks -= TS_TICKS_TO_UNIX_EPOCH;
  return ticks * TS_NANOSECONDS_PER_TICK;
}

/*
 * UNIXTIMEをDATETIMEに変換する
 *
 * @param unixtime UNIXTIME形式の日時データ
 * @param datetime DATETIME構造体へのポインタ
 */
void ts_unix2date(u_int64 unixtime, DATETIME* datetime)
{
  u_int64 sec;
  memset(datetime, 0x00, sizeof(DATETIME));
  if (unixtime == (u_int64)-1) return;
  // 秒
  //sec = unixtime / TS_NANOSECONDS_PER_SECOND;
  sec = do_div64(unixtime, TS_NANOSECONDS_PER_SECOND);
  datetime->second = (u_int)sec % 60;
  // 分
  sec = do_div64(sec, 60); // sec /= 60;
  datetime->minute = (u_int)sec % 60;
  // 時間
  sec = do_div64(sec, 60); // sec /= 60;
  datetime->hour = (u_int)sec % 24;
  // 1970/01/01 UTC以降の日数
  sec = do_div64(sec, 24); // sec /= 24;
  sec += 719499;
  // 年
  for(datetime->year = 1969;
      sec > YEAR_TO_DAYS(datetime->year + 1) + 30;
      datetime->year++);
  sec -= YEAR_TO_DAYS(datetime->year);
  // 月
  for(datetime->month = 1;
      datetime->month < 12 && sec > MONTH_TO_DAYS(datetime->month + 1);
      datetime->month++);
  sec -= MONTH_TO_DAYS(datetime->month);
  // 調整
  datetime->month += 2;
  if (datetime->month > 12) {
    datetime->month -= 12;
    datetime->year++;
  }
  // 日
  datetime->day = sec;
}

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

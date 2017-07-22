#ifndef INCLUDE_TIME_H
#define INCLUDE_TIME_H

#include "types.h"
#include "../hal/pit.h"

/*----- 定数定義 -----*/
#define TS_TICKS_PER_SECOND       10000000ULL
#define TS_NANOSECONDS_PER_TICK   100ULL
#define TS_NANOSECONDS_PER_SECOND 1000000000ULL
#define TS_SEC_TO_UNIX_EPOCH      11644473600ULL
#define TS_TICKS_TO_UNIX_EPOCH    (TS_TICKS_PER_SECOND * TS_SEC_TO_UNIX_EPOCH)
// 年の日数
#define YEAR_TO_DAYS(y) (u_int64)((y)*365 + (y)/4 - (y)/100 + (y)/400)
#define MONTH_TO_DAYS(m) (u_int64)(367*(m)/12)

/*----- 構造体定義 -----*/
typedef struct {
  u_short year;
  u_char month;
  u_char day;
  u_char hour;
  u_char minute;
  u_char second;
  u_short milisecond;
} DATETIME;

/*----- 関数定義 -----*/
u_int64 ts_file2unix(u_int64 filetime);
void ts_unix2date(u_int64 unixtime, DATETIME* datetime);
void sleep(u_int ms);

#endif

/*
 * util.c - 汎用関数
 */
#include "util.h"
#include "../hal/mem.h"

/*
 * strlen - 文字列の長さを調べる
 *
 * @param str 対象の文字列
 * @return 文字列長(バイト)
 */
u_int strlen(const char* str)
{
  int len = 0;
  while(str[len]) len++;
  return len;
}

/*
 * memcpy - メモリにデータをコピーする
 *
 * @param to   コピー先のメモリブロック
 * @param from コピー元のメモリブロック
 * @param n    コピーバイト数
 * @return コピー先アドレス
 */
void *memcpy(void *to, const void *from, u_int n)
{
  u_int i;
  u_char* dest = (u_char*)to;
  u_char* src = (u_char*)from;
  // コピー実行
  for(i = 0; i < n; i++) {
    dest[i] = src[i];
  }
  return to;
}

/*
 * memset - メモリにバイトを連続コピーする
 *
 * @param to セット先のメモリブロック
 * @param ch セットする文字
 * @param n  セットバイト数
 * @return セット先アドレス
 */
void *memset(void *to, u_char ch, u_int n)
{
  u_int i;
  u_char* desc = (u_char*)to;
  for(i = 0; i < n; i++) {
    desc[i] = ch;
  }
  return to;
}

/*
 * isascii - アスキー文字かを判別する
 *
 * @param c 判別する文字
 * @return ASCIIなら1、それ以外は0
 */
u_int isascii(u_int c)
{
  return c < 0x80 ? 1 : 0;
}

/*
 * isdigit - 数字かを判別する
 *
 * @param c 判別する文字
 * @return 数字なら1、それ以外は0
 */
u_char isdigit(u_char c)
{
  return (u_char)0x30 <= c && c < (u_char)0x40 ? 1 : 0;
}

/*
 * itoa - 数値を文字列に変換する
 *
 * @param value 数値
 * @param str   文字列へのポインタ
 * @param base  基数
 */
void itoa(int value, char *str, int base)
{
  char *ptr, *low;
  if ( base < 2 || base > 36 ) {
    *str = '\0';
    return;
  }
  ptr = str;
  // 負数には'-'を付ける
  if ( value < 0 && base == 10 ) {
    *ptr++ = '-';
  }
  low = ptr;
  do {
    *ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[35 + value % base];
    value /= base;
  } while(value);
  // 終端
  *ptr-- = '\0';
  while(low < ptr) {
    char tmp = *low;
    *low++ = *ptr;
    *ptr-- = tmp;
  }
  return;
}

/*
 * unicode2ascii - UnicodeをASCIIに変換する
 *
 * @param str Unicode文字列へのポインタ
 * @param len 字数(バイト数ではない)
 */
void unicode2ascii(char* str, u_int len)
{
  u_int i;
  for(i = 0; i < len; i++) {
    str[i] = str[i*2];
  }
  str[i] = '\x00';
}

/*
 * do_div64 - 64ビット整数を32ビット整数で除算する
 *
 * @param dividend 被除数
 * @param divisor  除数
 * @return 演算結果
 */
u_int64 do_div64(u_int64 dividend, u_int divisor)
{
  u_int64 *result = 0;
  u_int *result_low = (u_int*)result;
  u_int *result_high = (u_int*)((u_int)result + 4);
  //u_int dividend_low  = dividend & 0xFFFFFFFF;
  __asm__ __volatile__("pusha");
  __asm__ __volatile__("leal (%0), %%esi"
		       :
		       :"r"(&dividend)
		       :"%esi");
  __asm__ __volatile__("leal (%0), %%edi"
		       :
		       :"r"(&divisor)
		       :"%esi", "%edi");
  __asm__ __volatile__("call __do_div64");
  __asm__ __volatile__("movl %%eax, %0"
		       :"=r"(*result_low)
		       :
		       :"%eax", "%edx");
  __asm__ __volatile__("movl %%edx, %0"
		       :"=r"(*result_high)
		       :
		       :"%edx");
  __asm__ __volatile__("popa");
  return *result;
}

/*
 * do_div64 - 64ビット整数を32ビット整数で剰余を取る
 *
 * @param dividend 被除数
 * @param divisor  除数
 * @return 演算結果
 */
u_int do_mod64(u_int64 dividend, u_int divisor)
{
  return (u_int)dividend % divisor;
} 

/*
 * malloc - メモリ領域を確保する
 *
 * @param size 確保するセグメント数
 * @return 確保した領域の物理アドレス
 */
void *malloc(u_int size)
{
  return mem_alloc_blocks(size);
}

/*
 * free - メモリ領域を解放する
 *
 * @param addr 確保した物理アドレス
 * @param size 解放するセグメント数
 */
void free(void* addr, u_int size)
{
  return mem_free_blocks(addr, size);
}

/*
 * exit - 強制終了する
 */
void exit(void)
{
  __asm__ __volatile__("jmp kexit");
}

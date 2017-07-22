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
 * do_div64 - 64ビット整数を32ビット整数で除算する
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
  /*
    mov eax,[dividend + 4]      ;edx:eax = 0x0000000012345678
    div dword [divisor]         ;eax = 0x01D208A5; edx = 0x00000006
    mov [result + 4],eax        ;result high dword = 0x01D208A5
    mov eax,[dividend]          ;edx:eax = 0x000000069ABCDEF0
    div dword [divisor]         ;eax = 0xA912E318; edx = 0
    mov [result],eax            ;result low dword = 0xA912E318
    mov [remainder],edx         ;remainder = 0
  */
  return *result;
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

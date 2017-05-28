/*
 * util.c - 汎用関数
 */
#include "util.h"

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

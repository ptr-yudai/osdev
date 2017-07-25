#ifndef FS_NTFS_H
#define FS_NTFS_H

#include "../include/types.h"
#include "../include/util.h"
#include "../include/io.h"
#include "../hal/ide.h"
#include "mbr.h"

/*----- 定数定義 -----*/
// FILE RecordのFlags
#define NTFS_FILE_FLAG_FILE_IN_USE  0x01
#define NTFS_FILE_FLAG_DIRECTORY    0x02
// MFTのtypeID
#define NTFS_MFT_ATTRIBUTE_STDINFO  0x10
#define NTFS_MFT_ATTRIBUTE_ATTRLIST 0x20
#define NTFS_MFT_ATTRIBUTE_FILENAME 0x30
#define NTFS_MFT_ATTRIBUTE_OBJECTID 0x40
#define NTFS_MFT_ATTRIBUTE_SECDESC  0x50
#define NTFS_MFT_ATTRIBUTE_VOLNAME  0x60
#define NTFS_MFT_ATTRIBUTE_VOLINFO  0x70
#define NTFS_MFT_ATTRIBUTE_DATA     0x80
#define NTFS_MFT_ATTRIBUTE_INDXROOT 0x90
#define NTFS_MFT_ATTRIBUTE_INDXALLC 0xA0
#define NTFS_MFT_ATTRIBUTE_BITMAP   0xB0
#define NTFS_MFT_ATTRIBUTE_REPARSE  0xC0
#define NTFS_MFT_ATTRIBUTE_LOGGED   0x100
// MFTのformCode
#define NTFS_MFT_ATTRIBUTE_RESIDENT    0x0
#define NTFS_MFT_ATTRIBUTE_NONRESIDENT 0x1
// STANDARD_INFORMATIONおよびFILE_NAMEエントリのflags
#define NTFS_MFT_ENTRY_FLAGS_READONLY 0x0001
#define NTFS_MFT_ENTRY_FLAGS_HIDDEN   0x0002
#define NTFS_MFT_ENTRY_FLAGS_SYSTEM   0x0004
// [TODO] 追加
// FILE_NAMEのnameType
#define NTFS_MFT_ENTRY_NAMETYPE_POSIX  0
#define NTFS_MFT_ENTRY_NAMETYPE_WIN32  1
#define NTFS_MFT_ENTRY_NAMETYPE_DOS    2
#define NTFS_MFT_ENTRY_NAMETYPE_WIN32D 3
// INDEX_ROOTのflags
#define NTFS_MFT_ENTRY_FLAGS_SMALLINDX 0x00
#define NTFS_MFT_ENTRY_FLAGS_LARGEINDX 0x01

/*----- 構造体定義 -----*/
// BIOS Parameter Block
typedef struct {
  u_short bytesPerSector; 
  u_char  sectorsPerCluster;
  u_short reservedSectors;
  u_char  fats;
  u_short rootEntries;
  u_short sectors;
  u_char  mediaType;
  u_short sectorsPerFAT;
  u_short sectorsPerTrack;
  u_short heads;
  u_int   hiddenSectors;
  u_int   bigSectors;
} __attribute__((__packed__)) NTFS_BPB;

// ブートセクタ
typedef struct {
  u_char   jmp[3];           // ジャンプコード
  u_char   oem[8];           // OEM ID
  NTFS_BPB bpb;              // BIOS Parameter Block
  u_char   reserved1[4];     // 未使用
  s_int64  numSectors;       // セクタ数
  s_int64  mftCluster;       // MFTの開始クラスタ
  s_int64  mftmirrCluster;   // MFT Mirrorの開始クラスタ
  s_char   clustersPerMFT;   // MFTあたりのクラスタ数
  u_char   reserved2[3];     // 予約済み
  s_char   clustersPerIndex; // インデクスブロックあたりのクラスタ数
  u_char   reserved3[3];     // 予約済み
  u_int64  serialNum;        // シリアル番号
  u_int    checksum;         // チェックサム
  u_char   bootstrap[426];   // ブートコード
  u_short  signature;        // シグネチャ
} __attribute__((__packed__, __aligned__(8))) NTFS_BS;

// マスターファイルテーブル
typedef struct {
  u_char  signature[4];
  u_short fixupOffset;
  u_short fixupSize;
  u_int64 logSeqNumber;
  u_short sequence;
  u_short hardlinks;
  u_short attribOffset;
  u_short flags;
  u_int   recLength;
  u_int   allLength;
  u_int64 baseMFTRec;
  u_int   nextAttrID;
  u_int   fixupPattern;
  u_short MFTRecNumber;
} __attribute__((__packed__)) NTFS_MFT;
// INODE Header
typedef struct {
  u_int   inodeOffset;
  u_int   inodeLength;
  u_int   inodeAllocLength;
  u_int   flags;
} __attribute__((__packed__)) NTFS_INODE_HEADER;
// インックスレコード
typedef struct {
  u_char  signature[4];
  u_short fixupOffset;
  u_short fixupEntries;
  u_int64 updateSeqNumber;
  u_int64 vcnIndexAlloc;
  NTFS_INODE_HEADER inode;
} __attribute__((__packed__)) NTFS_RECORD_INDEX;

// アトリビュートヘッダ(レジデント)
typedef struct {
  u_int   typeID;
  u_int   length;
  u_char  formCode;
  u_char  nameLength;
  u_short nameOffset;
  u_short flag;
  u_short attribID;
  u_int   contentLength;
  u_short contentOffset;
  u_short reserved;
} __attribute__((__packed__)) NTFS_ATTR_HEADER_R;
// アトリビュートヘッダ(ノンレジデント)
typedef struct {
  u_int typeID;
  u_int length;
  u_char  formCode;
  u_char  nameLength;
  u_short nameOffset;
  u_short flag;
  u_short attribID;
  u_int64 startVCN;
  u_int64 endVCN;
  u_short runListOffset;
  u_short compressSize;
  u_int   reserved;
  u_int64 contentDiskSize;
  u_int64 contentSize;
  u_int64 initContentSize;
} __attribute__((__packed__)) NTFS_ATTR_HEADER_NR;

/* MFTエントリー */
// STANDARD_INFORMATION
typedef struct {
  u_int64 tsCreated;     // 作成日
  u_int64 tsModified;    // 変更日
  u_int64 tsMFTModified; // MFTの変更日
  u_int64 tsAccessed;    // アクセス日
  u_int   flags;         // フラグ(NTFS_MFT_ENTRY_FLAGS_*)
  u_int   maxVersions;
  u_int   versionNum;
  u_int   classID;
  u_int   ownerID;
  u_int   securityID;
  u_int64 quotaCharged;
  u_int64 updateSequence;
} __attribute__((__packed__)) NTFS_ENTRY_STDINFO;
// FILE_NAME
typedef struct {
  u_int64 parentDir;     // 親ディレクトリ
  u_int64 tsCreated;     // 作成日
  u_int64 tsModified;    // 変更日
  u_int64 tsMFTModified; // MFTの変更日
  u_int64 tsAccessed;    // アクセス日
  u_int64 logicalSize;   // 論理サイズ
  u_int64 physicalSize;  // 物理サイズ
  u_int   flags;         // フラグ(NTFS_MFT_ENTRY_FLAGS_*)
  u_int   reparse;
  u_char  nameLength;    // ファイル名の長さ
  u_char  nameType;      // ファイル名の種別(NTFS_MFT_ENTRY_NAMETYPE_*)
} __attribute__((__packed__)) NTFS_ENTRY_FILENAME;
// INDEX_ROOT
typedef struct {
  u_int  attributeType;
  u_int  collationRule;
  u_int  bytesPerIndex;
  u_char clustersPerIndex;
  u_char padding[3];
  NTFS_INODE_HEADER inode;
} __attribute__((__packed__)) NTFS_ENTRY_INDXROOT;
// INDEX_ALLOCATION

// Index Node Header
typedef struct {
  u_int64 mftref;
  u_short length;
  u_short dataLength;
  u_short flags;
} __attribute__((__packed__)) NTFS_INODE_I30_HEADER;

// runlist
typedef struct {
  long long int offset;
  long long int length;
  //NTFS_RUNLIST *blink;
  //NTFS_RUNLIST *flink;
} __attribute__((__packed__)) NTFS_RUNLIST;

// 共用情報
typedef struct {
  // パーティションテーブル
  u_short lbaFirst[4];
  // サイズ情報
  u_short bytesPerSector;
  u_char  sectorsPerCluster;
  u_short sectorsPerRecord;
  u_short bytesPerRecord;
  // ファイルシステム全体
  u_int numClusters;        // トータルクラスタ数
} NTFS_INFO;

/*----- 関数定義 -----*/
NTFS_BS* ntfs_bootsector(MBR* mbr);
NTFS_MFT* ntfs_mft(u_int mftCluster);
void* ntfs_find_attribute(NTFS_MFT* mftHeader, u_short typeID);
NTFS_RUNLIST* ntfs_parse_runlist(NTFS_ATTR_HEADER_NR *entry);
NTFS_RECORD_INDEX *ntfs_find_index(NTFS_RUNLIST *runlist, u_int n);
void ata_read_ntfs(char *buf, u_int lba, u_int n);

/*----- 変数定義 -----*/
NTFS_INFO ntfs_info;

#endif

#include "ntfs_scanner.h"

/**
 * timeline - タイムラインを作成する
 *
 * @param mftSector $MFTの開始セクタ番号
 */
void ntfs_timeline(u_int mftSector)
{
  int i, j;
  NTFS_MFT              *mft;            // $MFTレコード
  NTFS_ATTR_HEADER_NR   *mft_data;       // $MFTのDATA属性
  NTFS_ATTR_HEADER_R    *mft_stdinfo;    // STANDARD_INFO属性
  NTFS_ENTRY_STDINFO    *entry_stdinfo;  // STANDARD_INFOエントリ
  NTFS_RUNLIST          *runlist = NULL; // datarun
  void                  *mft_list;       // リストの先頭
  NTFS_MFT              *tmft = NULL;    // リスト中のレコード
  u_int64 unixtime;                      // 作成日時（unixタイム）
  DATETIME datetime;                     // 作成日時
  char* filepath;                        // ファイルパス
  NTFS_TIMELINE st_link, *current_link, *seek;
  // [TODO] どれくらい確保すればよい？
  NTFS_TIMELINE *timeline;
  int memsize;
  char c;
  char enable_filepath = 1;

  // 各種設定は尋ねる
  fb_print("A large region of memory is required to run timeline.\n"
	   "Would you like to configure timeline? [Y/n]");
  c = kb_getc();
  if (c == 'Y' || c == 'y') { // メモリ設定
    // ファイルパス探索機能
    fb_print("\nExploring filepath requires a large memory.\n"
	     "If it is disabled, only MFT reference number is shown.\n"
	     "Would you like to enable filepath exploring? [Y/n]");
    c = kb_getc();
    if (c != 'Y' && c != 'y') {
      enable_filepath = 0;
    }
    // メモリサイズ
    fb_print("\nConfigure the memory block size to use in timeline.\n"
	     "Set a large number when the disk has lots of files.\n"
	     "Default value: 256\n"
	     ">> ");
    memsize = 0;
    while(memsize <= 0) {
      if (kb_getnumber(&memsize)) {
	break;
      } else {
	memsize = 0;
      }
      fb_debug("Invalid memory size.\n", ER_CATION);
    }
    // [TODO] どの時刻でソートするかを決める
  } else {
    memsize = 0x100;
  }

  timeline = (NTFS_TIMELINE*)malloc(0x100);
  if (timeline == NULL) {
    fb_debug("This device has no enough memory to run timeline.\n", ER_FATAL);
    return;
  }
  memset(timeline, 0, 0x100 * MEMORY_BLOCK_SIZE);

  // $MFTのレコードを取得
  mft = ntfs_mft(mftSector);
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    fb_debug("Invalid MFT Record!\n", ER_WARNING);
    goto timeline_return;
  }

  // DATA属性を取得(ほぼ間違いなくnon-resident)
  mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA, 0);
  // [TODO] residentの実装(優先度低)
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    fb_debug("Resident $MFT is not supported!\n", ER_FATAL);
    goto timeline_return;
  }
  // datarunを取得
  runlist = ntfs_parse_runlist(mft_data);
  if (runlist == NULL) {
    fb_debug("Invalid datarun!\n", ER_WARNING);
    goto timeline_return;
  }

  // runlistを探索
  st_link.flink = timeline;
  current_link = timeline;
  for(i = 0; ; i++) {
    NTFS_RUNLIST *trun;
    trun = ntfs_extract_runlist(runlist, i);
    // RUNLISTからMFTに到達する
    mft_list = ntfs_find_data(runlist, i);
    if (mft_list == NULL) break;

    // 対象のIDに到達するまで調べる
    for(j = 0; j < 0xFFFF; j++) {
      // 対象のレコードを取得する
      tmft = mft_list;
      if (!((tmft->signature[0] == 'F') &
	    (tmft->signature[1] == 'I') &
	    (tmft->signature[2] == 'L') &
	    (tmft->signature[3] == 'E'))) {
	continue;
      }
	
      // IDと時刻を保存
      mft_stdinfo = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_STDINFO, 0);
      entry_stdinfo = (NTFS_ENTRY_STDINFO*)((char*)mft_stdinfo + mft_stdinfo->contentOffset);
      // 作成日時
      unixtime = ts_file2unix(entry_stdinfo->tsCreated);
      ts_unix2date(unixtime, &datetime);

      // データを格納
      current_link->mftref   = tmft->MFTRecNumber;
      current_link->unixtime = unixtime;
      
      // 最初のデータ
      if (current_link == timeline) {
	current_link = NULL;
	goto timeline_loop;
      }

      // リストに繋げる入れる
      for(seek = &st_link; seek->flink != NULL; seek = seek->flink) {
	// 自己参照
	if (seek->flink == seek) {
	  fb_debug("Detected an infinite loop!", ER_FATAL);
	  free(mft_list, trun->length);
	  goto timeline_return;
	}

	if (seek->flink->unixtime >= current_link->unixtime) {
	  // 次のデータが自分より大きい
	  // [TODO] unixtimeが同じ場合はmftrefを比較
	  current_link->flink = seek->flink;
	  seek->flink = current_link;
	  break;
	}
	if (seek->flink->flink == NULL) {
	  // 自分が最大
	  current_link->flink = NULL;
	  seek->flink->flink = current_link;
	  break;
	}
      }
      
    timeline_loop:
      current_link = (NTFS_TIMELINE*)((char*)current_link + sizeof(NTFS_TIMELINE));
      mft_list += ntfs_info.bytesPerSector * ntfs_info.sectorsPerRecord;
    }

    free(mft_list, trun->length);
  }

  // タイムラインを表示
  for(seek = &st_link; seek->flink != NULL; seek = seek->flink) {
    // 時刻
    ts_unix2date(seek->flink->unixtime, &datetime);
    fb_printf("[%d-%d-%d %d:%d:%d] ",
	      datetime.year, datetime.month, datetime.day,
	      datetime.hour, datetime.minute, datetime.second);
    fb_printx(seek->flink->mftref);
    // ファイルパス
    ///*
    if (enable_filepath) {
      filepath = ntfs_getpath(mftSector, seek->flink->mftref);
      fb_print(" ");
      fb_printf(" %s\n", filepath);
      free(filepath, 1);
    } else {
      fb_print("\n");
    }
  }

 timeline_return:
  free(mft, ntfs_info.sectorsPerRecord);
  free(timeline, 0x100);
}

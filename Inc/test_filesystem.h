#ifndef TEST_FILESYSTEM_H
#define TEST_FILESYSTEM_H

//#include "types.h"

struct tFileBuffers {
  char buff1[256*2], buff2[256*2], nameBuff[FILE_NAME_SZ];
  uint16_t test;
  int pos, index;
};

extern struct tFileBuffers fileBuffers;

void Init_Test(struct tFileBuffers *buffers);

void Test_FileWrite(struct tFileBuffers *buffers, struct tagFileSystem *fsys);

#endif
#include "filesystem.h"
#include "test_filesystem.h"
#include <string.h>
#include <stdio.h>
#include "exeptions.h"


struct tFileBuffers fileBuffers;

void Init_Test(struct tFileBuffers *buffers)
{
      for(int i = 0; i < 256; i++)
         {
           buffers->buff1[i] = i;
           buffers->buff1[i+256] = 256-i;
         }
      buffers->test = TRUE;
      buffers->index = 0;
}


void Test_FileWrite(struct tFileBuffers *buffers, struct tagFileSystem *fsys)
{
    if(buffers->test)
    {
        buffers->pos = 0;
        sprintf(&buffers->nameBuff[0], "%d\n",buffers->index++);
        filesystem_file_put(fsys, filesystem_create_file(fsys, &buffers->nameBuff[0], "txt", 1), &buffers->pos, (uint8_t*)&buffers->buff1[0], 256*2);
        for(int i = 0; i < 256; i++)
          buffers->buff2[i] = 0;
        buffers->pos = 0;
        filesystem_file_get(fsys, filesystem_open_file(fsys, &buffers->nameBuff[0], "txt"), &buffers->pos, (uint8_t*)&buffers->buff2[0], 256*2);
        if(strcmp(&buffers->buff1[0], &buffers->buff2[0]))
          exception(__FILE__,__FUNCTION__,__LINE__,"file corrupted");
        filesystem_detect_memorylow(fsys);
        if((fsys->bMemoryLow) || (buffers->index == 200))
        {
          buffers->test = FALSE;
          buffers->index = 0;
        }
    }
}
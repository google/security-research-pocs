#ifndef MEM_IMG_H
#define MEM_IMG_H

#include <stddef.h>
#include <stdint.h>

#include "sleuthkit/tsk/tsk_tools_i.h"

TSK_IMG_INFO *mem_open(const uint8_t *data, size_t size);

#endif  // # MEM_IMG_H

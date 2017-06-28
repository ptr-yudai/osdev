#ifndef INCLUDE_LINKER_H
#define INCLUDE_LINKER_H

#include "types.h"

extern u_int _KERNEL_TOP, _KERNEL_BOTTOM;

#define __KERNEL_TOP    &_KERNEL_TOP
#define __KERNEL_BOTTOM &_KERNEL_BOTTOM

#endif

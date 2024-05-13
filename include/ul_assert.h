#ifndef ASSERT_H
#define ASSERT_H
#include "logger.h"
#include <stdbool.h>

void ul_assert(bool value, const char *msg);

#endif // ASSERT_H
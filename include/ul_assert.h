#ifndef ASSERT_H
#define ASSERT_H

#include "location.h"
#include "logger.h"
#include <stdbool.h>

void ul_assert(bool value, const char *msg);
void ul_assert_location(location_t loc, bool value, const char *msg);

#endif // ASSERT_H
#ifndef UUID_UTILS_H_
#define UUID_UTILS_H_

#include "util/type.h"

typedef uint8_t UUIDValue[16];

struct UUIDUtils {
	static void generate(UUIDValue &out);
};

#endif

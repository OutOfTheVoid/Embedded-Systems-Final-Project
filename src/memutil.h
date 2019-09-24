#ifndef MEMUTIL_H
#define MEMUTIL_H

#include <stddef.h>
#include <stdint.h>

static inline void memzero(void * buffer, size_t count) {
	for (size_t i = 0; i < count; ++ i) {
		* (unsigned char *) buffer = 0;
	}
}

#endif

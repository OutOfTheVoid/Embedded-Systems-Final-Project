#include <stdint.h>

void * memcpy(void * dst, const void * src, uint32_t n) {
	for (uint32_t i = 0; i < n; i ++) {
		((uint8_t *) dst)[i] = ((const uint8_t *) src)[i];
	}
	return dst;
}
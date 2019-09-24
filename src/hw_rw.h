/*
Liam Taylor
wttesla@uw.edu

hw_rw.h:
Hardware read/write routines
============================
Contains routines for reading/writing to/from memory mapped hardware.
*/

#ifndef HW_WRITE_H
#define HW_WRITE_H

#include <stdint.h>

/*
write_hw_u8:
-----------------------------------------
Writes to an 8-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset
data: data to write
*/
inline void write_hw_u8(void * base_address, uintptr_t offset, uint8_t data) {
	*(volatile uint8_t *)(&((char *) base_address)[offset]) = data;
}

/*
write_hw_u16:
------------------------------------------
Writes to an 16-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset
data: data to write
*/
inline void write_hw_u16(void * base_address, uintptr_t offset, uint16_t data) {
	*(volatile uint16_t *)(&((char *) base_address)[offset]) = data;
}

/*
write_hw_u32:
------------------------------------------
Writes to an 32-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset
data: data to write
*/
inline void write_hw_u32(void * base_address, uintptr_t offset, uint32_t data) {
	*(volatile uint32_t *)(&((char *) base_address)[offset]) = data;
}

/*
read_hw_u8:
------------------------------------------
Reads from an 8-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset

Returns
=======
Register value
*/
inline uint8_t read_hw_u8(void * base_address, uintptr_t offset) {
	return *(volatile uint8_t *)(& ((char *) base_address)[offset]);
}

/*
read_hw_u16:
-------------------------------------------
Reads from an 16-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset

Returns
=======
Register value
*/
inline uint16_t read_hw_u16(void * base_address, uintptr_t offset) {
	return *(volatile uint16_t *)(& ((char *) base_address)[offset]);
}

/*
read_hw_u32:
-------------------------------------------
Reads from an 32-bit memory mapped register

Parameters
==========
base_address: register base address
offset: register offset

Returns
=======
Register value
*/
inline uint32_t read_hw_u32(void * base_address, uintptr_t offset) {
	return *(volatile uint32_t *)(& ((char *) base_address)[offset]);
}

#endif /* HW_WRITE_H */
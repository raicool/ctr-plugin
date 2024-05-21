#pragma once

#include "types.h"
#include "struct.h"

#include <vector>

// https://www.3dbrew.org/wiki/Memory_layout
#define EXEFS_RANGE(x)   x > 0x00100000 && x <= 0x14000000
#define FCRAM_0_RANGE(x) x > 0x14000000 && x <= 0x18000000
#define IO_RANGE(x)      x > 0x10000000 && x <= 0x18000000
#define VRAM_RANGE(x)    x > 0x18000000 && x <= 0x18600000 

static u32 offset;

/*
	@brief goes through a multi-level pointer and returns the pointer from given offsets
	@return nullptr if a pointer within the multi-level pointer is invalid
*/
uint32_t seek_ptr(uint32_t first_ptr, std::vector<int16_t> offset);

bool is_racing();

race_structure* get_race_ptr();

player_structure* get_player_ptr();

void write_speed(u32 speed);

void write_flag(u16 flag);

void accel_write(u32 speed);

const char* kcl_type_name_from_char(char type);

u32 get_input_frame();

u32 ARMBranch(const void* src, const void* dst);

u32 ARMBranchLink(const void* src, const void* dst);
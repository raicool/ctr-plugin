#include "util.h"

#include <CTRPluginFramework.hpp>

using namespace CTRPluginFramework;

static u32 original_speed[0x2D];

uint32_t seek_ptr(uint32_t first_ptr, std::vector<int16_t> offset)
{
	uint32_t ptr = *(uint32_t*)(first_ptr);

	for (int i = 0; i < offset.size(); i++)
	{
		if (Process::Read32(ptr + offset[i], ptr) == 0 && FCRAM_0_RANGE(ptr)) return 0;
	}

	return ptr;
}

bool is_racing()
{
	return (seek_ptr(0x14000084, { 0x316c, 0x118 }) != 0);
}

player_structure* get_player_ptr()
{
	if (is_racing())
	{
		return (player_structure*)seek_ptr(0x140002F4, { 0x14, 0x518, 0x1c });
	}

	return nullptr;
}

unknown_race_structure* get_unk_ptr()
{
	u32 ptr;
	if (is_racing())
	{
		Process::Read32(0x0065c528, ptr);
		return (unknown_race_structure*)ptr;
		//Process::Read8(data + 0xbc2, player_coins);
		//Process::Read8(data + 0xc32, ghost_coins);
	}

	return nullptr;
}

void write_speed(u32 speed)
{
	for (int i = 0; i < 0x2D; i++)
	{
		offset = seek_ptr(0x140002F4, { -0xa4 });

		if (original_speed[2] != speed)
		{
			Process::Read32((offset - 0x2C3B0) + (i * 4), original_speed[i]);
		}

		Process::Write32((offset - 0x2C3B0) + (i * 4), speed);
		Process::Write32((offset - 0x28E90) + (i * 4), speed);
		Process::Write32((offset - 0x1C730) + (i * 4), speed);
	}
}

void accel_write(u32 speed)
{
	for (int i = 0; i < 0x2B; i++)
	{
		Process::Write32(0x15386670, speed);
		Process::Write32(0x15386CF0, speed);
	}
}

const char* kcl_type_name_from_char(char type)
{
	switch (type)
	{
		case 0x00: return "Road 1"; break;
		case 0x01: return "Road 2"; break;
		case 0x02: return "Road 3"; break;
		case 0x03: return "Road 4"; break;
		case 0x04: return "Slippery Road 1"; break;
		case 0x05: return "Off-road"; break;
		case 0x06: return "Heavy Off-road"; break;
		case 0x07: return "Slippery Road 2"; break;
		case 0x08: return "Boost Pad"; break;
		case 0x09: return "Glider Pad"; break;
		case 0x0a: return ""; break;
		case 0x0b: return "Solid OOB"; break;
		case 0x0c: return "Moving Terrain"; break;
		case 0x0d: return "Road 5"; break;

		case 0x0f: return ""; break;
		case 0x10: return "Wall 1"; break;
		case 0x11: return "Wall 2"; break;
		case 0x12: return "Kart Barrier"; break;
		case 0x13: return "Item Barrier"; break;
		case 0x14: return "Wall 3"; break;
		case 0x15: return "Wall 4"; break;
		case 0x16: return ""; break;
		case 0x17: return ""; break;
		case 0x18: return ""; break;
		case 0x19: return "OOB 1"; break;
		case 0x1a: return "Effect Trigger 1"; break;
		case 0x1b: return ""; break;
		case 0x1c: return "Glider Activator"; break;
		case 0x1d: return "OOB 2"; break;
		case 0x1e: return ""; break;
		case 0x1f: return "Effect Trigger 2"; break;

		default: return "???"; break;
	}
}

u32 get_input_frame()
{
	u32 ptr = 0;
	u32 frame = 0;
	Process::Read32(0x14214e31, ptr);
	Process::Read32(ptr + 0xec3, ptr);
	Process::Read32(ptr + 0xbec, frame);
	return frame;
}

u32 ARMBranch(const void* src, const void* dst)
{
	u32 instrBase = 0xEA000000;
	/* off is actually a signed 26 bits integer, stored as a signed 24 bits integer (off a>> 2). Therefore, we need to sign extend it */
	s32 off = (s32)((const u8*)dst - ((const u8*)src + 8));
	off = (off << 6) >> 6;

	return instrBase | ((off >> 2) & 0xFFFFFF);
}

u32 ARMBranchLink(const void* src, const void* dst)
{
	u32 instrBase = 0xEB000000;
	/* off is actually a signed 26 bits integer, stored as a signed 24 bits integer (off a>> 2). Therefore, we need to sign extend it*/
	s32 off = (s32)((const u8*)dst - ((const u8*)src + 8));
	off = (off << 6) >> 6;
	return instrBase | ((off >> 2) & 0xFFFFFF);
}

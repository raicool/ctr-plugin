#pragma once

#include "types.h"

/*
	a structure that is created while racing, destroyed once finished racing
*/
struct PACKED player_structure
{
	char  data_1[0x24];   // 0x000 - 0x054
	float player_x;       // 0x054 - 0x05c
	float player_y;       // 0x05c - 0x064
	float player_z;       // 0x064 - 0x06c
	char  data_2[0xd0];   // 0x06c - 0x100
	int   data_int_1;     // 0x100 - 0x104
	int   data_int_2;     // 0x104 - 0x108
	char  data_3[0xc0c];  // 0x108 - 0xd48
	char  ground_type_id; // 0xc10 - 0xc11
	char  data_4[0x33];   // 0x108 - 0xd48
	int   player_airtime; // 0xd48 - 0xd4c
	char  data_5[0x1be];  // 0xd48 - 0xf2c
	char  miniturbo;      // 0xf2c - 0xf2d
	char  miniturbo_type; // 0xf2d - 0xf2e
	char  data_6[0x2];    // 0xf2e - 0xf30
	// could be a 2 byte integer instead
	char  miniturbo1;     // 0xf30 - 0xf31 
	char  miniturbo2;     // 0xf31 - 0xf32 
	char  data_7[0x1c];   // 0xf32 - 0xf43
	float player_speed;   // 0xf4e - 0xf52
	char  data_8[0x6c];   // 0xf52 - 0xfe8
	char  boost_duration; // 
	char  data_9[0x24];   // 
	bool  boosting;       // 0xfe8 - 0xfe9
};


/*
	base ptr(s) -> 0x0065c528
*/
struct PACKED unknown_structure
{
	char          data_1[0xbc2]; // 0x000 - 0xbc2
	unsigned char player_coins;  // 0xbc2 - 0xbc3
	char          data_2[0x06E]; // 0xbc3 - 0xc32
	unsigned char ghost_coins;   // 0xc32 - 0xc34
};

struct PACKED unknown_race_structure
{
	char data_1 [0xcef]; // 0x000 - 0xcef
	char player_kcp;     // 0xcef - 0xcf0
};
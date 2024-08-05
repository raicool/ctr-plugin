#pragma once

#include "types.h"

/*
	a structure that is created while racing, destroyed once finished racing
*/
struct PACKED player_structure
{
	char  data_1[0x24];   // 
	float player_x;       // 
	float player_y;       // 
	float player_z;       // 
	char  data_2[0xd0];   // 
	int   data_int_1;     // 
	int   data_int_2;     // 
	char  data_3[0xc0c];  // 
	char  ground_type_id; // 
	char  data_4[0x33];   // 
	int   player_airtime; // 
	char  data_5[0x1bc];  // 
	float miniturbo;      // 
	char  data_7[0x20];   // 
	float player_speed;   // 
	char  data_8[0x6c];   // 
	char  boost_duration; // 
	char  data_9[0x24];   // 
	bool  boosting;       // 
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
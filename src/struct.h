#pragma once

#include "types.h"

/*
	a structure that is created while racing, destroyed once finished racing
*/
struct PACKED player_structure
{
	char  data_1[0x20];   // 0x000 - 0x020
	float angle;          // 0x021 - 0x024
	float player_x;       // 0x025 - 0x028
	float player_y;       // 0x029 - 0x02c
	float player_z;       // 0x02d - 0x030
	char  data_2[0x8];
	float random_float2;
	float random_float;
	char  data_3[0xc0];   // 0x031 - 0x100
	int   data_int_1;     // 0x101 - 0x104
	int   data_int_2;     // 0x105 - 0x108
	char  data_4[0xc0c];  // 0x109 - 0xd1c
	char  ground_type_id; // 0xd1d - 0xd1d
	char  data_5[0x33];   // 0xd1e - 0xd50
	int   player_airtime; // 0xd51 - 0xd54
	char  data_6[0x1b8];  // 0xd55 - 0xf0c
	int   dokan_warp;     // 0xf0d - 0xf10
	float miniturbo;      // 0xf11 - 0xf14
	char  data_7[0x14];   // 0xf15 - 0xf30
	float data_float_0;
	float yaw_strength_external;   // 
	float yaw_strength_internal;   // 0xf31 - 0xf34
	float player_speed;   // 0xf35 - 0xf38
	float data_float_1;   // 0xf39 - 0xf3c
	float data_float_2;   // 0xf39 - 0xf3c
	float data_float_3;   // 0xf39 - 0xf3c
	char  data_8[0x60];   // 0xf3d - 0xfa4
	char  boost_duration; // 0xfa5 - 0xfa5
	char  data_9[0x6];    // 0xfa6 - 0xfab
	float boost_speed;    // 0xfac - 0xfaf
	char  data_10[0x1c];  // 0xfb0 - 0xfcb
	bool  boosting;       // 0xfcc - 0xfcc
	char  data_11[0x23];  // 0xfcd - 0xfef
	short trick;          // 0xff0 - 0xff1
};
static_assert(sizeof(player_structure) == 0xfe9);

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
#pragma once

#include "types.h"

/*
	a structure that is created while racing, destroyed once finished racing

	base ptr(s) -> 
				   
*/
struct race_structure 
{
	char  data_1[0xd48];       // 0x000 - 0xd48
	int   race_player_airtime; // 0xd48 - 0xd4c
	char  data_2[0x1e0];       // 0xd48 - 0xf2c
	float race_player_speed;   // 0xf2c - 0xf30
};

/*
	base ptr(s) -> 0x0065c528
*/
struct unknown_structure
{
	char          data_1[0xbc2]; // 0x000 - 0xbc2
	unsigned char player_coins;  // 0xbc2 - 0xbc3
	char          data_2[0x06E]; // 0xbc3 - 0xc32
	unsigned char ghost_coins;   // 0xc32 - 0xc34
};
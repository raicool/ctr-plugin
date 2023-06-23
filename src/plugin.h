#include <3ds.h>
#include <CTRPluginFramework.hpp>

#include "csvc.h"
#include "struct.h"

// https://www.3dbrew.org/wiki/Memory_layout
#define EXEFS_RANGE(x) x > 0x00100000 && x < 0x14000000
#define IN_RANGE(x)    x > 0x14000000 && x < 0x18000000
#define IO_RANGE(x)    x > 0x10000000 && x < 0x18000000

using namespace CTRPluginFramework;

unknown_structure** unk  = (unknown_structure**)0x0065c528;
//unknown_structure* unk;
//race_structure**   race = (race_structure**)0x0708b2b0;
race_structure*    race;
u32                offset;
u32                data;
static u32         original_speed[0x2D];


bool is_racing()
{
	if (Process::Read32(0x14000084, offset) && IN_RANGE(offset))
		if (Process::Read32(offset + 0x316C, offset) && IN_RANGE(offset))
			return Process::Read32(offset + 0x118, offset) && (offset & 0xFF);
	return 0;
}


void* get_race_ptr()
{
	if (is_racing())
		if (Process::Read32(0x140002F4, offset) && IN_RANGE(offset))
			if (Process::Read32(offset + 0x14, offset) && IN_RANGE(offset))
				if (Process::Read32(offset + 0x518, offset) && IN_RANGE(offset))
					if (Process::Read32(offset + 0x1C, offset) && IN_RANGE(offset))
						return (void*)offset;
	return 0;
}


void write_speed(u32 speed)
{
	for (int i = 0; i < 0x2D; i++)
	{
		if (Process::Read32(0x140002F4, offset) && IN_RANGE(offset) && Process::Read32(offset - 0xA4, offset) && IN_RANGE(offset))
		{
			if (original_speed[2] != speed)
				Process::Read32((offset - 0x2C3B0) + (i * 4), original_speed[i]);
			Process::Write32((offset - 0x2C3B0) + (i * 4), speed);
			Process::Write32((offset - 0x28E90) + (i * 4), speed);
			Process::Write32((offset - 0x1C730) + (i * 4), speed);
		}
	}
}

void write_location(u32 location)
{
	if (Process::Read32(0x6673C8, offset) && IO_RANGE(offset))
		Process::Write32(offset + 0xF7CC, location);
}

void write_flag(u16 flag)
{
	if (Process::Read32(0x6673C8, offset) && IO_RANGE(offset))
	{
		Process::Write8(offset + 0xE7CA, flag);
		Process::Write8(offset + 0xF7D0, flag);
	}
}

void accel_write(int speed)
{
	for (int i = 0; i < 0x2B; i++)
	{
		Process::Write32(0x15386670, speed);
		Process::Write32(0x15386CF0, speed);
	}
}

/////////////////////////////////////////////////////////    Start of menu codes    /////////////////////////////////////////////////////////

void speedometer(MenuEntry *entry)
{
	if (!*(unk)) return;

	race = (race_structure*)get_race_ptr();
	//Process::Read32(0x0065c528, data);
	//unk = (unknown_structure*)data;
	

	OSD::Run([](const Screen& screen)
	{
		static float player_old_speed;
		u8           player_coins;
		u8           ghost_coins;

		/*
			race offsets
			ghost airtime -> 0xd48
		 
			coins
			some random ass base ptr -> 0x0065c528

			offsets
			player coins -> 0xbc2
			ghost coins  -> 0xc32


			air timer?
			some base ptr -> 0x154b71e8

			offsets
			signed short?   -> 0x1b5d6
			unsigned short? -> 0x1b5d4
		*/

		Process::Read32(0x0065c528, data);
		Process::Read8(data + 0xbc2, player_coins);
		Process::Read8(data + 0xc32, ghost_coins);

		float player_speed = race->race_player_speed * 10.376756f; // km/h

		if (screen.IsTop)
		{
			/*
				start of top screen
			*/

			if (race->race_player_airtime == 0)
			{
				screen.Draw(Utils::Format("Air : %i", race->race_player_airtime), 332, 190, Color::Red);
			}
			else
			{
				screen.Draw(Utils::Format("Air : %i", race->race_player_airtime), 332, 190, Color::LimeGreen);
			}


			if (ghost_coins >= 10)
			{
				screen.Draw(Utils::Format("Coins %i", ghost_coins), 332, 200, Color::Yellow);
			}
			else
			{
				screen.Draw(Utils::Format("Coins %i", ghost_coins), 332, 200);
			}


			screen.Draw(Utils::Format("%.1f km/h", player_speed), 332, 210);

			float player_delta = player_speed - player_old_speed;

			if (player_delta == 0)      screen.Draw(Utils::Format("( %.2f)", player_delta), 332, 220, Color::Gray);
			else if (player_delta >= 0) screen.Draw(Utils::Format("(+%.2f)", player_delta), 332, 220, Color::ForestGreen);
			else                        screen.Draw(Utils::Format("(%.2f)",  player_delta), 332, 220, Color::Maroon);

			player_old_speed = player_speed;
		}
		else
		{
			/*
				start of bottom screen
			*/
			if (Controller::IsKeyDown(DPadDown))
			{
				screen.Draw(Utils::Format("race %p", get_race_ptr()), 10, 10);
				screen.Draw(Utils::Format("race2%p", race), 10, 20);
				screen.Draw(Utils::Format("unk  %p", (*unk)), 10, 30);
				screen.Draw(Utils::Format("pc   %i", (*unk)->player_coins), 10, 40);
			}
		}

		return true;
	});
}

/*
	You must enter a race and then exit/finish/restart the race in order for this to take effect
	Having this enabled for more than 1 race will crash the game, unless you click restart and not exit or next course or anything else.
*/
void replay_ghost(MenuEntry *entry)
{
	OSD::Run([](const Screen& screen)
	{
		u8 char_0x119 = 0;
		u32 some_int = 0;
		u32 int_0x208 = 0;

		if (is_racing() && Process::Read32(0x0FFFF5D4, data) && Process::Read32(data - 0x00000004, data) && Process::Read32(data + 0x00000018, data))
		{
			if (Process::Read32(0x14000084, offset) && Process::Read32(offset + 0x0000316C, offset) && IN_RANGE(offset))
			{
				//Process::Read8(offset + 0x00000119, char_0x119);
				//Process::Write8(offset + 0x00000119, 0);

				/*
					target camera and model to ghost instead of player
				*/
				Process::Read32(data + 0x00000208, int_0x208);
				Process::Write32(data + 0x00000208, 0x00010001);

				if (Process::Read32(0x0065DA44, data) && Process::Read32(data + 0x000020E0, data))
				{
					// Move player to above finish line
					Process::Read32(data + 0x24, some_int);
					Process::Write32(data + 0x24, 0x49000000); 
				}
			}
		}

		/*
		*	OSD Render section
		*/

		if (screen.IsTop)
		{
			
		}
		else
		{
			screen.Draw(Utils::Format("char_0x119 %i", char_0x119), 10, 100, Color::Gray);
			screen.Draw(Utils::Format("int_0x208  %i", int_0x208), 10, 120, Color::Gray);
			screen.Draw(Utils::Format("int        %i", some_int), 10, 130, Color::Gray);
		}

		return true;
	});
}
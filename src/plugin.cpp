#include "plugin.h"

#include <3ds.h>
#include <CTRPluginFramework.hpp>

#include "csvc.h"
#include "struct.h"
#include "util.h"

using namespace CTRPluginFramework;

//
// credits to https://twitter.com/B_squo
//
__attribute__((naked)) void change_ghost_alpha()
{
	asm volatile
	(
		"cmp   r2, #1      \n"
		"moveq r5, #0      \n"
		"bx    lr          \n"
	);
}

//
// credits to https://twitter.com/B_squo
//
__attribute__((naked)) void live_replay()
{
	asm volatile
	(
		"cmp   r1, #0          \n"
		"strne r2, [pc, #0x10] \n"
		"streq r2, [pc, #0x10] \n"
		"ldreq r2, [pc, #0x8]  \n"
		"cmp   r2, #0          \n"
		"ldreq r2, [pc, #4]    \n"
		"bx    lr              \n"
		".word 0x00000000      \n"
		".word 0x00000000      \n"
	);
}

static u32 input_frame_count;

// TODO: optimize (probably not necessary)
__attribute__((naked)) void debug_hook_callback()
{
	asm volatile
	(
		"push  {r5}       \n"
		"cmp   r4, %[in]  \n"
		"ldreq r5, %[out] \n"
		"addeq r5, r5, #1 \n"
		"streq r5, %[out] \n"
		"pop   {r5}       \n"
		"bx    lr         \n"
		: [out]"=m"(input_frame_count)
		: [in]"r"(race)
	);
}

void info(MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		if (!System::IsCitra())
		{
			OSD::Notify("info is only available if using citra!", Color::White, Color::Maroon);
			entry->Disable();
			return;
		}

		const std::vector <u32> func_pattern =
		{
			0xE59400E4, 0xED9F9AC2, 0xE5901008, 0xE5911110, 0xE3110001, 0x03110010, 0xE59410FC
		};

		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		debug_hook.Initialize(func_ptr, reinterpret_cast<u32>(debug_hook_callback));

		if (debug_hook.Enable() == HookResult::Success)
		{
			OSD::Notify("debug hook enabled!", Color::LimeGreen);
			OSD::Notify(Utils::Format("%p", debug_hook_callback), Color::Gray);
		}
		else
		{
			OSD::Notify("debug hook failed to enable!", Color::Red);
			debug_hook.Disable();
			entry->Disable();
		}
	}

	race = get_race_ptr();
	player = get_player_ptr();

	if (race && player)
	{
		infodisplay = [](const Screen& screen)
		{
			static float player_kmh;
			static float player_old_kmh;
			static float kmh_delta;
			static u32 miniturbo_old;
			static bool screen_switch;

			player_kmh = race->player_speed * 10.376756f;

			// switch the OSD render screen whenever dpad down is pressed
			if (Controller::IsKeyPressed(DPadDown))
			{
				screen_switch = !screen_switch;
			}

			if (screen.IsTop ^ screen_switch)
			{
				if (race->miniturbo_type == 67)
				{
					screen.Draw(Utils::Format("(+%i)", race->miniturbo - miniturbo_old), 10, 40, Color::Gray);

					/*
						MT charges at value > 90
						SMT charges at value > 230
					*/

					Color mt_text_color;

					if (race->miniturbo < 90)
					{
						mt_text_color = Color::Gray;
					}

					// blue mt
					if (race->miniturbo >= 90 && race->miniturbo < 230)
					{
						mt_text_color = Color::SkyBlue;
					}

					// red mt
					if (race->miniturbo >= 230)
					{
						mt_text_color = Color::Red;
					}

					screen.Draw(Utils::Format((race->miniturbo < 100) ? "MT : %i/230" : "MT :  %i/230", race->miniturbo), 10, 50, mt_text_color);
				}

				// miniturbo charge value wrapped around, so just print this instead
				if (race->miniturbo_type >= 68)
				{
					screen.Draw("MT : 255/230", 10, 50, Color::Red);
				}

				screen.Draw(Utils::Format("Air : %i", race->player_airtime), 10, 60, (race->player_airtime == 0) ? Color::Red : Color::LimeGreen);

				screen.Draw(Utils::Format("%f km/h", player_kmh), 10, 70);

				kmh_delta = player_kmh - player_old_kmh;

				if (kmh_delta == 0)
				{
					screen.Draw(Utils::Format("( %f)", kmh_delta), 10, 80, Color::Gray);
				}
				else if (kmh_delta >= 0)
				{
					screen.Draw(Utils::Format("(+%f)", kmh_delta), 10, 80, Color::ForestGreen);
				}
				else 
				{
					screen.Draw(Utils::Format("(%f)", kmh_delta), 10, 80, Color::Maroon);
				}

				screen.Draw(Utils::Format("Frame %i", input_frame_count), 10, 190, Color::SkyBlue);

				if (race->boosting)
				{
					screen.Draw("Boost : Yes", 10, 200, Color::LimeGreen);
				}
				else
				{
					screen.Draw("Boost : No", 10, 200, Color::Red);
				}

				screen.Draw(Utils::Format("KCL Type : %s", kcl_type_name_from_char(race->ground_type_id)), 10, 210, Color::DeepSkyBlue);
				screen.Draw(Utils::Format("(%.3f, %.3f, %.3f)", race->player_x, race->player_y, race->player_z), 10, 220, Color::DodgerBlue);

				miniturbo_old = race->miniturbo;
				player_old_kmh = player_kmh;
			}

			return true;
		};
	}

	if (!race || !player || !is_racing() || !entry->IsActivated())
	{
		OSD::Stop(infodisplay);
		input_frame_count = 0;
	}
	else
	{
		OSD::Run(infodisplay);
	}
}

void replay_ghost(MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		// beginning instructions of Kart::VehicleControl::setPad()
		const std::vector <u32> func_pattern =
		{
			0xe590002c, 0xe7900101, 0xe1a01002, 0xe590002c, 0xe1a00000, 0xe58010e0, 0xe5911038, 0xe58010e4
		};

		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		live_replay_hook.Initialize(func_ptr, reinterpret_cast<u32>(live_replay));
		live_replay_hook.SetFlags(USE_LR_TO_RETURN | EXECUTE_OI_AFTER_CB);

		if (live_replay_hook.Enable() == HookResult::Success)
		{
			OSD::Notify("live_replay hook enabled!", Color::LimeGreen);
		}
		else
		{
			OSD::Notify("live_replay hook failed to enable!", Color::Red);
			live_replay_hook.Disable();
			entry->Disable();
		}
	}

	if (!entry->IsActivated())
	{
		if (live_replay_hook.Disable() == HookResult::Success)
		{
			OSD::Notify("live_replay hook disabled!", Color::LimeGreen);
		}
		else
		{
			OSD::Notify("live_replay hook failed to disable!", Color::Red);
		}
	}
}

void ghost_disable_overwrite(MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xeb000061, 0xe320f000, 0xe320f000, 0xea000008, 0xe1a01004
		};

		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		if (func_ptr)
		{
			// System::SaveDataManager::calcInner_(int) + 0xa8
			// disable writing new ghosts by replacing instruction with an NOP
			Process::Write32(func_ptr, 0xe320f000);
		}
		else
		{
			OSD::Notify("hide ghost failed to enable!", Color::Red);
			entry->Disable();
		}
	}

	if (!entry->IsActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xeb000061, 0xe320f000, 0xe320f000, 0xea000008, 0xe1a01004
		};

		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		// restore old instruction
		Process::Write32(func_ptr, 0xeb000061);
	}
}


void ghost_hide(MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xee400a00, 0xed900a00, 0xee208a80, 0xed9f9a94, 0xe3570000, 0xe5d4003f, 0x1eb08a49
		};

		// search through part of exefs memory for our instruction ptr
		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		overwrite_ghost_alpha_hook.Initialize(func_ptr + 0x20, reinterpret_cast<u32>(change_ghost_alpha));

		// dont run the overwritten instruction
		overwrite_ghost_alpha_hook.SetFlags(USE_LR_TO_RETURN);

		if (overwrite_ghost_alpha_hook.Enable() == HookResult::Success)
		{
			OSD::Notify("overwrite_ghost_alpha hook enabled!", Color::LimeGreen);
		}
		else
		{
			OSD::Notify("overwrite_ghost_alpha hook failed to enable!", Color::Red);
			overwrite_ghost_alpha_hook.Disable();
		}
	}

	if (!entry->IsActivated())
	{
		if (overwrite_ghost_alpha_hook.Disable() == HookResult::Success)
		{
			OSD::Notify("overwrite_ghost_alpha hook disabled!", Color::LimeGreen);
		}
		else
		{
			OSD::Notify("overwrite_ghost_alpha hook failed to disable!", Color::Red);
		}
	}
}

void disable_music(MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
 		const std::vector <u32> func_pattern =
 		{
			0xEBFFFFA5, 0xE3500000, 0x1A000002, 0xE594001C, 0xE3A01001, 0xE5C0108A
 		};
 
 		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		svcInvalidateEntireInstructionCache();

		// disables course ambient audio
		//Process::Write32(0x003DA0CC, 0xe320f000);

		Process::Write32(func_ptr, 0xe320f000);
	}

	if (!entry->IsActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xEBFFFFA5, 0xE3500000, 0x1A000002, 0xE594001C, 0xE3A01001, 0xE5C0108A
		};

		u32 func_ptr = Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		// restore original function (BL nw__snd__internal__driver__StreamSoundPlayer__StreamDataLoadTask__LoadStreamData)
		Process::Write32(func_ptr, 0xebffffa5);
	}
}
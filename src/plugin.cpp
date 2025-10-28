#include "plugin.h"

#include <3ds.h>
#include <CTRPluginFramework.hpp>

#include "csvc.h"
#include "vertex.h"
#include "mk7structs.h"

#include "disp.h"
#include "struct.h"
#include "util.h"

//#include "mk7-memory/include/System/RootSystem.hpp"
#include "sead/include/basis/seadTypes.h"
#include "sead/include/math/seadVector.hpp"

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
		: [in]"r"(player)
	);
}

__attribute__((naked)) void tas_code_callback_5eba18()
{
	asm volatile
	(
		"cmp   r5, #0          \n"
		"strne r1, [pc, #0x1c] \n"
		"streq r1, [pc, #0x1c] \n"
		"streq r0, [pc, #0x1c] \n"
		"ldreq r1, [pc, #0x10] \n"
		"cmp   r1, #0          \n"
		"ldreq r1, [pc, #0xc]  \n"
		"str   r1, [r0, #0xe4] \n"
		"bx    lr              \n"
		".word 0x00000000      \n"
		".word 0x00000000      \n"
	);
}

static bool instant_finish_callback_called = false;
__attribute__((naked)) void instant_finish_callback_4640e4()
{
	asm volatile
	(
		"push  {r5}    \n"
		"ldr r5, %0    \n"
		"mov r5, #0xff \n"
		"str r5, %0    \n"
		"pop  {r5}     \n"
		"bx  lr        \n"
		: "=m" (instant_finish_callback_called)
	);
}

void render_info()
{
	static float player_old_x;
	static float player_old_y;
	static float player_old_z;
	static float player_kmh;
	static float player_old_kmh;
	static float kmh_delta;
	static float miniturbo_old;
	static bool screen_switch;
	
	player_kmh = player->player_speed * 10.376756f;

	// switch the OSD render screen whenever dpad down is pressed
	if (CTRPluginFramework::Controller::IsKeyPressed(DPadDown))
	{
		screen_switch = !screen_switch;
	}

	const CTRPluginFramework::Screen screen = screen_switch ? CTRPluginFramework::OSD::GetBottomScreen() : CTRPluginFramework::OSD::GetTopScreen();

	if (player->miniturbo)
	{
		screen.Draw(CTRPluginFramework::Utils::Format("(+%f)", player->miniturbo - miniturbo_old), 10, 110, Color::Gray);

		CTRPluginFramework::Color mt_text_color;

		if (player->miniturbo < 220)
		{
			mt_text_color = Color::Gray;

			screen.Draw(CTRPluginFramework::Utils::Format((player->miniturbo < 100) ? "MT :  %.2f/220.00" : "MT : %.2f/220.00", player->miniturbo), 10, 120, mt_text_color);
		}
		else
		{
			// blue mt
			if (player->miniturbo >= 220 && player->miniturbo < 460)
			{
				mt_text_color = Color::SkyBlue;
			}

			// red mt
			if (player->miniturbo >= 460)
			{
				mt_text_color = Color::Red;
			}

			screen.Draw(CTRPluginFramework::Utils::Format((player->miniturbo < 100) ? "MT :  %.2f/460.00" : "MT : %.2f/460.00", player->miniturbo), 10, 120, mt_text_color);
		}
	}

	screen.Draw(CTRPluginFramework::Utils::Format("%f km/h", player_kmh), 10, 130);
	//screen.Draw(Utils::Format("Yaw Int : %f\xb0", radian_to_degrees(player->yaw_strength_internal)), 10, 90, Color::DeepSkyBlue);
	//screen.Draw(Utils::Format("Yaw Ext : %f\xb0", radian_to_degrees(player->yaw_strength_external)), 10, 80, Color::DodgerBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("YV Int : %f", player->random_float * 10.376756f), 280, 210, CTRPluginFramework::Color::Orange);
	screen.Draw(CTRPluginFramework::Utils::Format("XV Int : %f", player->random_float2 * 10.376756f), 280, 220, CTRPluginFramework::Color::SkyBlue);


	kmh_delta = player_kmh - player_old_kmh;

	if (kmh_delta == 0)
	{
		screen.Draw(CTRPluginFramework::Utils::Format("( %f)", kmh_delta), 10, 140, CTRPluginFramework::Color::Gray);
	}
	else if (kmh_delta >= 0)
	{
		screen.Draw(CTRPluginFramework::Utils::Format("(+%f)", kmh_delta), 10, 140, CTRPluginFramework::Color::ForestGreen);
	}
	else
	{
		screen.Draw(CTRPluginFramework::Utils::Format("(%f)", kmh_delta), 10, 140, CTRPluginFramework::Color::Maroon);
	}

	//screen.Draw(Utils::Format("Trick %i", player->trick), 10, 150, Color::SkyBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("Frame %i", input_frame_count), 10, 160, CTRPluginFramework::Color::SkyBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("Air : %i", player->player_airtime), 10, 170, (player->player_airtime == 0) ? CTRPluginFramework::Color::Red : CTRPluginFramework::Color::LimeGreen);
	screen.Draw(CTRPluginFramework::Utils::Format("Boost : %i", player->boost_duration), 10, 180, player->boosting ? CTRPluginFramework::Color::LimeGreen : CTRPluginFramework::Color::Red);
	screen.Draw(CTRPluginFramework::Utils::Format("KCL : %s", kcl_type_name_from_char(player->ground_type_id)), 10, 190, (player->player_airtime == 0) ? CTRPluginFramework::Color::DeepSkyBlue : CTRPluginFramework::Color::Gray);
	screen.Draw(CTRPluginFramework::Utils::Format("X : %.3f", player->player_x), 10, 200, CTRPluginFramework::Color::DodgerBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("Y : %.3f", player->player_y), 10, 210, CTRPluginFramework::Color::DodgerBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("Z : %.3f", player->player_z), 10, 220, CTRPluginFramework::Color::DodgerBlue);
	screen.Draw(CTRPluginFramework::Utils::Format("debug: %i", instant_finish_callback_called), 10, 230, CTRPluginFramework::Color::DodgerBlue);
	

	miniturbo_old = player->miniturbo;
	player_old_kmh = player_kmh;
	player_old_x = player->player_x;
	player_old_y = player->player_y;
	player_old_z = player->player_z;

	render_player_yaw(&screen, player);
}

void info(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
 		if (!CTRPluginFramework::System::IsCitra())
		{
			CTRPluginFramework::OSD::Notify("info is only available if using citra!", Color::White, Color::Maroon);
			entry->Disable();
			return;
		}

		const std::vector <u32> func_pattern =
		{
			0xE59400E4, 0xED9F9AC2, 0xE5901008, 0xE5911110, 0xE3110001, 0x03110010, 0xE59410FC
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		debug_hook.Initialize(func_ptr, reinterpret_cast<u32>(debug_hook_callback));

		if (debug_hook.Enable() == CTRPluginFramework::HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("debug hook enabled!", Color::LimeGreen);
//			OSD::Notify(Utils::Format("%p", debug_hook_callback), Color::Gray);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("debug hook failed to enable!", CTRPluginFramework::Color::Red);
			debug_hook.Disable();
			entry->Disable();
		}
	}

	player = get_player_ptr();

	if (player)
	{
		render_info();
	}

	if (!player || !is_racing() || !entry->IsActivated())
	{
		input_frame_count = 0;
	}
}

void instant_finish(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		// beginning instructions of RaceSys::TimeAttackManager::calcRace()
		const std::vector <u32> func_pattern =
		{
			0xe92d4070, 0xe1a04000, 0xeb0000e0, 0xe3a00f7a, 0xe59414c4, 0xe19000b4, 0xe3a05001, 0xe1510000
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);
		func_ptr += 52;

		instant_finish_hook.Initialize(func_ptr, reinterpret_cast<u32>(instant_finish_callback_4640e4));
		instant_finish_hook.SetFlags(USE_LR_TO_RETURN);
		instant_finish_callback_called = false;

		if (instant_finish_hook.Enable() == CTRPluginFramework::HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("instant_finish hook enabled!", Color::LimeGreen);
			CTRPluginFramework::OSD::Notify(CTRPluginFramework::Utils::Format("%p", func_ptr));
		}
		else
		{
			CTRPluginFramework::OSD::Notify("instant_finish hook failed to enable!", Color::Red);
			instant_finish_hook.Disable();
			entry->Disable();
		}
	}

	if (instant_finish_callback_called)
	{
		CTRPluginFramework::OSD::Notify("instant_finish_callback was called!", Color::LimeGreen);
		instant_finish_hook.Disable();
		entry->Disable();
		return;
	}

	if (entry->IsActivated() == false)
	{
		CTRPluginFramework::OSD::Notify("instant_finish_callback hook disabled!", Color::LimeGreen);
		instant_finish_hook.Disable();
		return;
	}
}

void replay_ghost(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		// beginning instructions of Kart::VehicleControl::setPad()
		const std::vector <u32> func_pattern =
		{
			0xe590002c, 0xe7900101, 0xe1a01002, 0xe590002c, 0xe1a00000, 0xe58010e0, 0xe5911038, 0xe58010e4
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		live_replay_hook.Initialize(func_ptr, reinterpret_cast<u32>(live_replay));
		live_replay_hook.SetFlags(USE_LR_TO_RETURN | EXECUTE_OI_AFTER_CB);

		if (live_replay_hook.Enable() == CTRPluginFramework::HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("live_replay hook enabled!", Color::LimeGreen);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("live_replay hook failed to enable!", Color::Red);
			live_replay_hook.Disable();
			entry->Disable();
		}
	}

	if (!entry->IsActivated())
	{
		if (live_replay_hook.Disable() == HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("live_replay hook disabled!", Color::LimeGreen);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("live_replay hook failed to disable!", Color::Red);
		}
	}
}

void replay_ghost_old(MenuEntry* entry)
{
	u8 char_0x119 = 0;
	u32 some_int = 0;
	u32 int_0x208 = 0;

	if (is_racing() && CTRPluginFramework::Process::Read32(0x0FFFF5D4, data) && CTRPluginFramework::Process::Read32(data - 0x00000004, data) && CTRPluginFramework::Process::Read32(data + 0x00000018, data))
	{
		if (CTRPluginFramework::Process::Read32(0x14000084, offset) && CTRPluginFramework::Process::Read32(offset + 0x0000316C, offset) && FCRAM_0_RANGE(offset))
		{
			//Process::Read8(offset + 0x00000119, char_0x119);
			//Process::Write8(offset + 0x00000119, 0);

			/*
				target camera and model to ghost instead of player
			*/
			CTRPluginFramework::Process::Read32(data + 0x00000208, int_0x208);
			CTRPluginFramework::Process::Write32(data + 0x00000208, 0x00010001);

			if (CTRPluginFramework::Process::Read32(0x0065DA44, data) && CTRPluginFramework::Process::Read32(data + 0x000020E0, data))
			{
				// Move player to above finish line
				CTRPluginFramework::Process::Read32(data + 0x24, some_int);
				CTRPluginFramework::Process::Write32(data + 0x24, 0x49000000);
			}
		}
	}
}

void ghost_disable_overwrite(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xeb000061, 0xe320f000, 0xe320f000, 0xea000008, 0xe1a01004
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00400000, func_pattern);

		if (func_ptr)
		{
			svcInvalidateEntireInstructionCache();

			// System::SaveDataManager::calcInner_(int) + 0xa8
			// disable writing new ghosts by replacing instruction with an NOP
			CTRPluginFramework::Process::Write32(func_ptr, 0xe320f000);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("never save ghosts failed to enable!", CTRPluginFramework::Color::Red);
			entry->Disable();
		}
	}

	if (!entry->IsActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xe320f000, 0xe320f000, 0xe320f000, 0xea000008, 0xe1a01004
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		svcInvalidateEntireInstructionCache();

		// restore old instruction
		CTRPluginFramework::Process::Write32(func_ptr, 0xeb000061);
	}
}


void ghost_hide(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector <u32> func_pattern =
		{
			0xee400a00, 0xed900a00, 0xee208a80, 0xed9f9a94, 0xe3570000, 0xe5d4003f, 0x1eb08a49
		};

		// search through part of exefs memory for our instruction ptr
		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		overwrite_ghost_alpha_hook.Initialize(func_ptr + 0x20, reinterpret_cast<u32>(change_ghost_alpha));

		// dont run the overwritten instruction
		overwrite_ghost_alpha_hook.SetFlags(USE_LR_TO_RETURN);

		if (overwrite_ghost_alpha_hook.Enable() == CTRPluginFramework::HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("overwrite_ghost_alpha hook enabled!", CTRPluginFramework::Color::LimeGreen);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("overwrite_ghost_alpha hook failed to enable!", CTRPluginFramework::Color::Red);
			overwrite_ghost_alpha_hook.Disable();
		}
	}

	if (!entry->IsActivated())
	{
		if (overwrite_ghost_alpha_hook.Disable() == CTRPluginFramework::HookResult::Success)
		{
			CTRPluginFramework::OSD::Notify("overwrite_ghost_alpha hook disabled!", CTRPluginFramework::Color::LimeGreen);
		}
		else
		{
			CTRPluginFramework::OSD::Notify("overwrite_ghost_alpha hook failed to disable!", CTRPluginFramework::Color::Red);
		}
	}
}

void disable_music(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
 		const std::vector<u32> func_pattern =
 		{
			0xEBFFFFA5, 0xE3500000, 0x1A000002, 0xE594001C, 0xE3A01001, 0xE5C0108A
 		};
 
 		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00800000, func_pattern);

		svcInvalidateEntireInstructionCache();

		// disables course ambient audio
		//Process::Write32(0x003DA0CC, 0xe320f000);

		CTRPluginFramework::Process::Write32(func_ptr, 0xe320f000);
	}

	if (!entry->IsActivated())
	{
		const std::vector<u32> func_pattern =
		{
			0xe320f000, 0xE3500000, 0x1A000002, 0xE594001C, 0xE3A01001, 0xE5C0108A
		};

		u32 func_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00300000, func_pattern);

		svcInvalidateEntireInstructionCache();

		// restore original function (BL nw__snd__internal__driver__StreamSoundPlayer__StreamDataLoadTask__LoadStreamData)
		CTRPluginFramework::Process::Write32(func_ptr, 0xebffffa5);
	}
}

void new_aspect_ratio(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector<u32> memory_pattern =
		{
			0xebfa0830, 0xe5c47102, 0xecbd8b02, 0xe8bd81f0
		};

		u32 mem_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00800000, memory_pattern);
		
		if (mem_ptr)
		{
			//OSD::Notify(Utils::Format("%p", mem_ptr));
		}
		else
		{
			CTRPluginFramework::OSD::Notify("aspect ratio var not found!", CTRPluginFramework::Color::White, CTRPluginFramework::Color::Maroon);
			entry->Disable();
			return;
		}

		mem_ptr += 16;
		svcInvalidateEntireInstructionCache();

		// overwrite aspect ratio var with 16/9
		std::vector<std::string> strings = 
		{
			"16:10 (Original)",
			"4:3",
			"16:9",
			"21:9"
		};

		CTRPluginFramework::Keyboard aspect_ratio_option("Select aspect ratio", strings);
		u32 selection = aspect_ratio_option.Open();

		switch (selection)
		{
		case 0:
			CTRPluginFramework::Process::Write32(mem_ptr, 0x3fd55555); // 16:10 (Original)
			//Process::Write32(mem_ptr + 4, 0x3fd55555); // 16:10 (Original)
			break;
		case 1:
			CTRPluginFramework::Process::Write32(mem_ptr, 0x3faaaaab); // 4:3
			//Process::Write32(mem_ptr + 4, 0x3faaaaab); // 4:3
			break;
		case 2:
			CTRPluginFramework::Process::Write32(mem_ptr, 0x3fe38e39); // 16:9
			//Process::Write32(mem_ptr + 4, 0x3fe38e39); // 16:9
			break;
		case 3:
			CTRPluginFramework::Process::Write32(mem_ptr, 0x40155555); // 21:9
			//Process::Write32(mem_ptr + 4, 0x40155555); // 21:9
			break;
		default:
			CTRPluginFramework::OSD::Notify("invalid option!", CTRPluginFramework::Color::White, CTRPluginFramework::Color::Maroon);
			entry->Disable();
			break;
		}
	}

	if (!entry->IsActivated())
	{
		const std::vector<u32> memory_pattern =
		{
			0xebfa0830, 0xe5c47102, 0xecbd8b02, 0xe8bd81f0
		};

		u32 mem_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00800000, memory_pattern);

		if (mem_ptr)
		{
			CTRPluginFramework::OSD::Notify(CTRPluginFramework::Utils::Format("%p", mem_ptr));
		}
		else
		{
			CTRPluginFramework::OSD::Notify("aspect ratio var not found!");
			return;
		}

		mem_ptr += 16;
		svcInvalidateEntireInstructionCache();

		// restore original floating point value (1.66667)
		Process::Write32(mem_ptr, 0x3fd55555);
	}
}

/*
void test_popup(CTRPluginFramework::MenuEntry* entry)
{
	if (entry->WasJustActivated())
	{
		const std::vector<u32> g_root_system_pattern =
		{
			0xe5952000, 0xe8bd4070
		};

		System::g_root_system = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00800000, memory_pattern);

		if (System::g_root_system)
		{
			OSD::Notify("RootSystem ptr: %p", System::g_root_system);
		}

		// first 6 instructions of UI::UnitTextPane::create(nw::lyt::Pane *, wchar_t *, int, bool)
		const std::vector<u32> memory_pattern =
		{
			0xe92d4fff, 0xe1a04000, 0xe1b05001, 0xe1a0b001, 0xe59f64a0, 0xed2d8b02
		};

		u32 mem_ptr = CTRPluginFramework::Utils::Search<u32>(0x00100000, 0x00800000, memory_pattern);

		if (mem_ptr)
		{
			CTRPluginFramework::OSD::Notify(CTRPluginFramework::Utils::Format("%p", mem_ptr));
		}
		else
		{
			CTRPluginFramework::OSD::Notify("UI::UnitTextPane::create() var not found!", CTRPluginFramework::Color::White, CTRPluginFramework::Color::Maroon);
			entry->Disable();
			return;
		}

		//nw::lyt::Pane* new_pane = new nw::lyt::Pane;
		//const wchar_t* text = L"hello!";
		//UI::UnitTextPane::create(new_pane, text, 0, 0, false);

		entry->Disable();
	}
}
*/
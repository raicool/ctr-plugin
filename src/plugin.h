#include <3ds.h>
#include <CTRPluginFramework.hpp>

#include "csvc.h"
#include "struct.h"
#include "util.h"

using namespace CTRPluginFramework;

static player_structure* player;
static Hook live_replay_hook;
static Hook overwrite_ghost_alpha_hook;
static Hook debug_hook;

static u32 data;

// evil assembly subroutines
__attribute__((naked)) void change_ghost_alpha();
__attribute__((naked)) void live_replay();
__attribute__((naked)) void debug_hook_callback();

// ctrpf menu entries
void info(MenuEntry* entry);
void replay_ghost(MenuEntry* entry);
void replay_ghost_old(MenuEntry* entry);
void ghost_disable_overwrite(MenuEntry* entry);
void ghost_hide(MenuEntry* entry);
void disable_music(MenuEntry* entry);
void new_aspect_ratio(MenuEntry* entry);
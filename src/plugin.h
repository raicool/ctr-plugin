#include <3ds.h>
#include <CTRPluginFramework.hpp>

#include "csvc.h"
#include "struct.h"
#include "util.h"

using namespace CTRPluginFramework;

static player_structure* player;
static race_structure* race;

static Hook live_replay_hook;
static Hook overwrite_ghost_alpha_hook;
static Hook debug_hook;
static Hook mute_music_hook;
static OSDCallback infodisplay;

// evil assembly subroutines
__attribute__((naked)) void change_ghost_alpha();
__attribute__((naked)) void live_replay();

// ctrpf menu entries
void info(MenuEntry* entry);
void replay_ghost(MenuEntry* entry);
void ghost_disable_overwrite(MenuEntry* entry);
void ghost_hide(MenuEntry* entry);
void disable_music(MenuEntry* entry);
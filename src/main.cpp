#include <3ds.h>
#include <CTRPluginFramework.hpp>
#include <vector>

#include "csvc.h"
#include "plugin.h"

using namespace CTRPluginFramework;

// This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
static void toggle_touchscreen_force_on(void)
{
    static u32 original = 0;
    static u32* patchAddress = nullptr;

    if (patchAddress && original)
    {
        *patchAddress = original;
        return;
    }

    static const std::vector<u32> pattern =
    {
        0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
        0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
        0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
    };

    Result res;
    Handle processHandle;
    s64    textTotalSize = 0;
    s64    startAddress = 0;
    u32*   found;

    if (R_FAILED(svcOpenProcess(&processHandle, 16)))
        return;

    svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
    svcGetProcessInfo(&startAddress, processHandle, 0x10005);
    if (R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
        goto exit;

    found = (u32*)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

    if (found != nullptr)
    {
        original = found[13];
        patchAddress = (u32*)PA_FROM_VA((found + 13));
        found[13] = 0xE1A00000;
    }

    svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
    svcCloseHandle(processHandle);
}

// This function is called before main and before the game starts
// Useful to do code edits safely
void patch_process(FwkSettings& settings)
{
    toggle_touchscreen_force_on();
}

// This function is called when the process exits
// Useful to save settings, undo patchs or clean up things
void on_process_exit(void)
{
    toggle_touchscreen_force_on();
}

void init_menu(PluginMenu& menu)
{
    // Create your entries here, or elsewhere
    // You can create your entries whenever/wherever you feel like it

    menu += new MenuEntry("Info", speedometer, "View things such as ghost coin count, air time, speed, acceleration, etc.");
    menu += new MenuEntry("Replay", replay_ghost, "Time trial ghost");
}

namespace CTRPluginFramework
{
    int main(void)
    {
        PluginMenu* menu = new PluginMenu("Storm Plugin", 0, 1, 1, "Storm Plugin");

        // Synnchronize the menu with frame event
        menu->SynchronizeWithFrame(true);

        // Init our menu entries & folders
        init_menu(*menu);

        // Launch menu and mainloop
        menu->Run();

        delete menu;

        // Exit plugin
        return (0);
    }
}

int main()
{
    return CTRPluginFramework::main();
}
#include <CTRPluginFramework.hpp>


#include <sead/include/math/seadMathCalcCommon.h>
#include <sead/include/math/seadVector.h>

#include "vertex.h"

#include "struct.h"
#include "vshader_shbin.h"

using namespace CTRPluginFramework;

void render_player_yaw(const Screen* screen, player_structure* player)
{
	sead::Vector2<f32> begin = sead::Vector2<f32>(200, 210);

	for (int i = 0; i <= 65; i++)
	{
		f32 end_x = begin.x - ((float)i * sead::MathCalcCommon<f32>::sin(player->yaw_strength_external));
		f32 end_y = begin.y - (((float)i / 1.5) * sead::MathCalcCommon<f32>::cos(player->yaw_strength_external));

		for (int j = 0; j <= 9; j++)
		{
			screen->DrawPixel(((u32)end_x - 1) + (j % 3), ((u32)end_y - 1) + sead::MathCalcCommon<f32>::floor(j / 3.0f), Color::Lime);
		}
	}

	for (int i = 0; i <= 60; i++)
	{
		f32 end_x = begin.x - ((float)i * sead::MathCalcCommon<f32>::sin(player->yaw_strength_internal));
		f32 end_y = begin.y - (((float)i / 1.5) * sead::MathCalcCommon<f32>::cos(player->yaw_strength_internal));

		for (int j = 0; j <= 9; j++)
		{
			screen->DrawPixel(((u32)end_x - 1) + (j % 3), ((u32)end_y - 1) + sead::MathCalcCommon<f32>::floor(j / 3.0f), Color::Maroon);
		}
	}
}

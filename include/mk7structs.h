#pragma once

#include "mk7-memory/vendor/nw4c/include/nw/lyt/Pane.hpp"

namespace UI
{
	class UnitTextPane
	{
		void(*create)(nw::lyt::Pane* panel, const wchar_t* text, int int_0, int font_type, bool bool_0);
	};
}
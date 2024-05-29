#pragma once
#include "../Kiero/kiero.h"

namespace KieroHooks
{
	inline bool Imgui_Init = false;
	void InitImgui();
	void Shutdown_Menu();
	void Init_Menu();
}

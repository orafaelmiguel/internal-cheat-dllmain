#include <D3DX11.h>
#include <d3d11.h>
#include "Hooks_Present.h"
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")

void KieroHooks::init()
{
	bool should_load = false;
	do
	{

		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			//menu
			should_load = true;
		}

	} while (should_load == false);
}

void KieroHooks::shutdown()
{
}

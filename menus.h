#pragma once
#include <d3d11.h>
#include "imgui/imgui.h"
#include <string>
#include "imgui/imbrowser.h"
#include "misc/rainbow.h"


namespace Overlay
{
	void Render();
	extern bool LoadCrosshairImage();
}

namespace Menu
{
	void Render();
	void Init();

}

namespace Global
{
	extern bool running;
	extern ID3D11ShaderResourceView* texture;
	extern ImVec4 color;
	extern int style;
	extern int size;
	extern int gap;
	extern std::string path;
	extern ID3D11Device* device;
	extern float thickness;
	extern bool onhold;
	extern bool antialiasing;
	extern int x_offset;
	extern int y_offset;
	extern bool rainbow;
	extern float rainbowSpeed;
	extern ImGui::FileBrowser fileDialog;
	extern Color rainbowColor;
}

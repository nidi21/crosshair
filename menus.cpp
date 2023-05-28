#include "menus.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"

#include "misc/config.h"

bool loadedImg = false;

namespace Global
{
	bool running = true;
	ID3D11ShaderResourceView* texture = NULL;
	int style = 0;
	ImVec4 color = {1.f,1.f,1.f,1.f};
	int size = 1;
	int gap = 1;
	std::string path = "";
	ID3D11Device* device = nullptr;
	float thickness = 1;
	bool onhold = false;
	bool antialiasing = true;
	int x_offset = 0;
	int y_offset = 0;
	bool rainbow = false;
	float rainbowSpeed = 0.005f;
	ImGui::FileBrowser fileDialog;
	Color rainbowColor;
}

bool LoadTextureFromFile(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** out_srv)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();


	stbi_image_free(image_data);

	return true;
}

bool Overlay::LoadCrosshairImage()
{
	if (loadedImg && Global::texture != nullptr)
	{
		Global::texture->Release();
		Global::texture = nullptr;
		loadedImg = false;
	}

	bool ret = LoadTextureFromFile(Global::device, Global::path.c_str(), &Global::texture);
	return ret;
}

ImVec2 GetDesktopResolution()
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return { (float)desktop.right, (float)desktop.bottom };
}

void Overlay::Render()
{
	auto ImDrawList = ImGui::GetBackgroundDrawList();
	auto vecMiddle = ImGui::GetMainViewport()->GetCenter();

	if (ImGui::GetStyle().AntiAliasedFill != Global::antialiasing)
		ImGui::GetStyle().AntiAliasedFill = Global::antialiasing;

	if (ImGui::GetStyle().AntiAliasedLines != Global::antialiasing)
		ImGui::GetStyle().AntiAliasedLines = Global::antialiasing;

	static float fRainbow;
	fRainbow += Global::rainbowSpeed;
	if (fRainbow > 1.f) fRainbow = 0.f;
	Global::rainbowColor = Color::FromHSB(fRainbow, 1.f, 1.f);

	switch (Global::style)
	{
	case 0:
	{
		ImDrawList->AddLine({ (vecMiddle.x + Global::x_offset), (vecMiddle.y + Global::y_offset) - (Global::size - 1) }, { vecMiddle.x + Global::x_offset, (vecMiddle.y + Global::y_offset) + Global::size }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness); //vertical
		ImDrawList->AddLine({ (vecMiddle.x + Global::x_offset) - (Global::size - 1), (vecMiddle.y + Global::y_offset) }, { (vecMiddle.x + Global::x_offset) + Global::size, (vecMiddle.y + Global::y_offset) }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness); //horizontal
		break;
	}
	case 1:
	{
		ImDrawList->AddCircleFilled({ (vecMiddle.x + Global::x_offset), (vecMiddle.y + Global::y_offset) }, (float)1.8 + (float)Global::size, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), 100);
		ImDrawList->AddCircle({ (vecMiddle.x + Global::x_offset), (vecMiddle.y + Global::y_offset) }, (float)1.9 + (float)Global::size, IM_COL32(0, 0, 0, 255), 100, 1);
		break;
	}
	case 2:
	{
		//horizontal
		ImDrawList->AddLine({ ((vecMiddle.x + Global::x_offset) - Global::gap - Global::size), (vecMiddle.y + Global::y_offset) }, { ((vecMiddle.x + Global::x_offset) - Global::gap - Global::size) + Global::size, (vecMiddle.y + Global::y_offset) }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness);
		ImDrawList->AddLine({ ((vecMiddle.x + Global::x_offset) + Global::gap + Global::size) - (Global::size - 1), (vecMiddle.y + Global::y_offset) }, { ((vecMiddle.x + Global::x_offset) + Global::gap + Global::size + 1), (vecMiddle.y + Global::y_offset) }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness);
	
		//vertical
		ImDrawList->AddLine({ (vecMiddle.x + Global::x_offset), ((vecMiddle.y + Global::y_offset) - Global::gap - Global::size) }, { (vecMiddle.x + Global::x_offset), ((vecMiddle.y + Global::y_offset) - Global::gap) }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness);
		ImDrawList->AddLine({ (vecMiddle.x + Global::x_offset), ((vecMiddle.y + +Global::y_offset) + Global::gap + Global::size + 1) }, { (vecMiddle.x + Global::x_offset), ((vecMiddle.y + Global::y_offset) + Global::gap + 1) }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color), Global::thickness);
		break;
	}
	case 3:
	{
		if (loadedImg && Global::texture != nullptr)
		{
			ImDrawList->AddImage((void*)Global::texture, { (vecMiddle.x + Global::x_offset) - Global::size, (vecMiddle.y + Global::y_offset) - Global::size }, { (vecMiddle.x + Global::x_offset) + Global::size, (vecMiddle.y + Global::y_offset) + Global::size }, { 0,0 }, { 1,1 }, Global::rainbow ? Color::getABGR(Global::rainbowColor) : ImColor(Global::color));
		}
		break;
	}
	}
}

void Menu::Init()
{
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f,0.5f);
	ImGui::GetStyle().FrameBorderSize = 1.f;
	ImGui::GetStyle().WindowBorderSize = 1.f;
	ImGui::GetStyle().FrameRounding = 3.f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.07f, 0.94f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.03f, 0.07f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.31f, 0.30f, 0.59f, 0.50f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.28f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.35f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.28f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.11f, 0.28f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.11f, 0.28f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.35f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.55f, 0.75f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.63f, 0.64f, 1.00f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.21f, 0.64f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.32f, 0.77f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.16f, 0.83f, 0.54f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.64f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.21f, 0.81f, 0.54f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.41f, 0.42f, 0.96f, 0.54f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.21f, 0.64f, 0.54f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.20f, 0.21f, 0.64f, 0.54f);
}

void Menu::Render()
{
	ImGui::SetNextWindowSize({ 487,319 });
	static bool initPos = false;
	if (!initPos)
	{
		ImGui::SetNextWindowPos({ GetDesktopResolution().x * 0.5f, GetDesktopResolution().y * 0.5f }, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		initPos = true;
	}

	ImGui::Begin("crosshair.exe", &Global::running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ColorEdit3("Crosshair Color", (float*)&Global::color); 
	ImGui::Combo("Crosshair Style", &Global::style, "Default\0Circle\0Casual\0Image\0");
	ImGui::SliderInt("Crosshair Size", &Global::size, 1, 15);

	if (Global::style == 2)
	{
		ImGui::SliderInt("Crosshair Gap", &Global::gap, 1, 8);
	}

	if (Global::style == 2 || Global::style == 0)
	{
		ImGui::SliderFloat("Crosshair Thickness", &Global::thickness, 1, 5);

	}

	if (Global::style == 3)
	{
		ImGui::InputText("Image Path", &Global::path);
		if (ImGui::Button("Load Image"))
		{
			loadedImg = Overlay::LoadCrosshairImage();
		}
		ImGui::SameLine();
		if (ImGui::Button("Open file"))
		{
			Global::fileDialog.Open();
		}

		if (Global::texture != nullptr && loadedImg)
		{
			ImGui::SameLine();

			ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "The image is loaded!");
		}
	}

	ImGui::Checkbox("Rainbow", &Global::rainbow);
	ImGui::PushItemWidth(70);
	ImGui::SliderFloat("Speed", &Global::rainbowSpeed, 0.001f, 0.015f);
	ImGui::PopItemWidth();

	ImGui::Dummy({ 0,10 });
	ImGui::Checkbox("Anti-aliasing", &Global::antialiasing);

	ImGui::Text("Offset from screen center [X / Y]");

	ImGui::PushItemWidth(100);
	ImGui::SliderInt("##x", &Global::x_offset, -200, 200);
	ImGui::SameLine();
	ImGui::SliderInt("##y", &Global::y_offset, -200, 200);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		Global::x_offset = 0; 
		Global::y_offset = 0;
	}

	ImGui::Text("Config");

	if (ImGui::Button("Save"))
	{
		Config::Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		Config::Load();
	}
	

	ImGui::End();

	if (!Global::fileDialog.IsOpened())
		ImGui::SetNextWindowPos({ GetDesktopResolution().x * 0.5f, GetDesktopResolution().y * 0.5f }, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	Global::fileDialog.Display();

	if (Global::fileDialog.HasSelected())
	{
		Global::path = Global::fileDialog.GetSelected().string();
		loadedImg = Overlay::LoadCrosshairImage();
		Global::fileDialog.ClearSelected();
	}
}
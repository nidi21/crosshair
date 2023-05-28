#include "config.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <shlobj.h>
#include "../menus.h"

std::string GetMyDocumentsFolderPath()
{
    wchar_t Folder[1024];
    HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, Folder);
    if (SUCCEEDED(hr))
    {
        char str[1024];
        size_t i;
        wcstombs_s(&i, str, Folder, 1023);
        return str;
    }
    else return "";
}

namespace Config
{

	void Load()
	{
        mINI::INIFile file(GetMyDocumentsFolderPath() + "\\crosshair_settings.ini");
        mINI::INIStructure ini;
        if (!file.read(ini))
        {
            file.generate(ini, true);
            ini["config"]["style"] = "0";
            ini["config"]["size"] = "1";
            ini["config"]["gap"] = "1";
            ini["config"]["thickness"] = "1.0";
            ini["config"]["path"] = "";
            ini["config"]["antialiasing"] = "0";
            ini["config"]["x_offset"] = "0";
            ini["config"]["y_offset"] = "0";
            ini["config"]["rainbow"] = "0";
            ini["config"]["rainbowSpeed"] = "0.005";

            ini["colors"]["R"] = "1.0";
            ini["colors"]["G"] = "1.0";
            ini["colors"]["B"] = "1.0";
            
            file.write(ini, true);
        }
        else {
            Global::style = std::stoi(ini["config"]["style"]);
            Global::size = std::stoi(ini["config"]["size"]);
            Global::gap = std::stoi(ini["config"]["gap"]);
            Global::thickness = std::stof(ini["config"]["thickness"]);
            Global::path = ini["config"]["path"];
            Global::antialiasing = std::stoi(ini["config"]["antialiasing"]);
            Global::x_offset = std::stoi(ini["config"]["x_offset"]);
            Global::y_offset = std::stoi(ini["config"]["y_offset"]);
            Global::rainbow = std::stoi(ini["config"]["rainbow"]);
            Global::rainbowSpeed = std::stof(ini["config"]["rainbowSpeed"]);

            Global::color.x = std::stof(ini["colors"]["r"]);
            Global::color.y = std::stof(ini["colors"]["g"]);
            Global::color.z = std::stof(ini["colors"]["b"]);
        }

	}

	void Save()
	{
        mINI::INIFile file(GetMyDocumentsFolderPath() + "\\crosshair_settings.ini");
        mINI::INIStructure ini;
        if (!file.read(ini))
        {
            file.generate(ini, true);
        }

        ini["config"]["style"] = std::to_string(Global::style);
        ini["config"]["size"] = std::to_string(Global::size);
        ini["config"]["gap"] = std::to_string(Global::gap);
        ini["config"]["thickness"] = std::to_string(Global::thickness);
        ini["config"]["path"] = Global::path;
        ini["config"]["antialiasing"] = std::to_string(Global::antialiasing);
        ini["config"]["x_offset"] = std::to_string(Global::x_offset);
        ini["config"]["y_offset"] = std::to_string(Global::y_offset);
        ini["config"]["rainbow"] = std::to_string(Global::rainbow);
        ini["config"]["rainbowSpeed"] = std::to_string(Global::rainbowSpeed);

        ini["colors"]["r"] = std::to_string(Global::color.x);
        ini["colors"]["g"] = std::to_string(Global::color.y);
        ini["colors"]["b"] = std::to_string(Global::color.z);
        file.write(ini, true);

	}
}
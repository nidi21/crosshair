#pragma once
#include <Windows.h>
#include "../imgui/imgui.h"

struct Color
{
	int r, g, b, a;

	Color()
	{
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	Color(int r, int g, int b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Color(int r, int g, int b, int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	static Color FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1) {
			return Color((BYTE)(brightness * 255), (BYTE)(t * 255), (BYTE)(p * 255));
		}
		else if (h < 2) {
			return Color((BYTE)(q * 255), (BYTE)(brightness * 255), (BYTE)(p * 255));
		}
		else if (h < 3) {
			return Color((BYTE)(p * 255), (BYTE)(brightness * 255), (BYTE)(t * 255));
		}
		else if (h < 4) {
			return Color((BYTE)(p * 255), (BYTE)(q * 255), (BYTE)(brightness * 255));
		}
		else if (h < 5) {
			return Color((BYTE)(t * 255), (BYTE)(p * 255), (BYTE)(brightness * 255));
		}
		else return Color((BYTE)(brightness * 255), (BYTE)(p * 255), (BYTE)(q * 255));
	}

	static ImColor getARGB(Color color)
	{
		return ImColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
	}

	static ImColor getABGR(Color color)
	{
		return ImColor(color.b / 255.f, color.g / 255.f, color.r / 255.f, color.a / 255.f);
	}
};
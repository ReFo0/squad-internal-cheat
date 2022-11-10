#pragma once
#include <cstdint>



auto uworld_sig = sig_scan(0, _("48 8B 1D ?? ?? ?? ?? 48 85 DB 74 ?? 41 B0 ??"));
auto w2s_sig = sig_scan(0, _("48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 48 81 EC ? ? ? ? 41 0F ? ? 49 8B ? 48 8B ? 48 8B ? 48 85 ? 0F 84"));
auto screen_sig = sig_scan(0, _("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 C0 49 8B F8 89 02 48 8B F2 41 89 00 48 8B 99"));

typedef bool(__thiscall* this_ProjectWorldLocationToScreens)(uintptr_t playerController, vec3 pos, vec2* screen, bool bPlayerViewportRelative);
this_ProjectWorldLocationToScreens m_ProjectWorldLocationToScreens;

typedef void(__thiscall* this_GetViewPortSize)(uintptr_t playerController, int32_t& ScreenWidth, int32_t& ScreenHeight);
this_GetViewPortSize m_GetViewPortSize;

bool worldtoscreen(uintptr_t playerController, vec3 pos, vec2* screen, bool bPlayerViewportRelative)
{
	this_ProjectWorldLocationToScreens m_projworld = (this_ProjectWorldLocationToScreens)(DWORD64)w2s_sig;
	return m_projworld(playerController, pos, screen, bPlayerViewportRelative);
}

void screensize(uintptr_t playerController, int32_t& ScreenWidth, int32_t& ScreenHeight) {
	this_GetViewPortSize m_getviewportsize = (this_GetViewPortSize)((DWORD64)screen_sig);
	return m_getviewportsize(playerController, ScreenWidth, ScreenHeight);
}

template <typename T> T read(const uintptr_t address)
{
	T Novalue = {};
	if (!IsBadReadPtr((const void*)address, sizeof(T)))
	{
		return *(T*)(address);
	}
	else
	{
		return Novalue;
	}
}
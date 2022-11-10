#pragma once
#include <cstdint>
#include <misc/offsets.h>
#include <misc/math.h>
#include <misc/memory.h>

bool esp = false;
inline static void initcitos() {
	if (esp) {
		auto u_world = read<uintptr_t>(uworld_sig + *(int*)(uworld_sig + 0x3) + 0x7);
		auto game_instance = read<uintptr_t>(u_world + offset_game_instance);
		auto local_player_array = read<uintptr_t>(game_instance + offset_local_players_array);
		auto local_player = read<uintptr_t>(local_player_array);
		auto local_player_controller = read<uintptr_t>(local_player + offset_player_controller);
		auto local_player_pawn = read<uintptr_t>(local_player_controller + offset_apawn);
		auto local_player_root = read<uintptr_t>(local_player_pawn + offset_root_component);
		auto local_player_state = read<uintptr_t>(local_player_pawn + offset_player_state);
		auto persistent_level = read<uintptr_t>(u_world + offset_persistent_level);
		auto actors = read<uintptr_t>(persistent_level + offset_actor_array);
		auto actor_count = read<int>(persistent_level + offset_actor_count);
		int32_t width, height;
		screensize(local_player_controller, width, height);
		for (int i = 0; i < actor_count; i++)
		{
			auto actor_pawn = read<uintptr_t>(actors + i * 0x8);
			auto uinstigator = read<uintptr_t>(actor_pawn + offsets_instigator_actor);
			auto root = read<uintptr_t>(uinstigator + offset_root_component);
			vec2 screen;
			if (worldtoscreen(local_player_controller, read<vec3>(root + offset_relative_location), &screen, false))
			{
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(width / 2, height), ImVec2(screen.x, screen.y), ImColor(255, 255, 255, 255));
			}
		}
	}
		
}


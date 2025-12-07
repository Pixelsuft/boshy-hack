#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <imgui.h>
#include <cstdlib>

bool win_shown = true;
bool no_cursor_kill = false;
bool skip_msg_box = true;
bool new_rng_custom_seed = false;
int new_rng_seed = 0;
bool new_rng_custom = false;
bool god_mode = false;
int new_rng = 0;
bool no_trans = false;
bool no_viewport = false;
int ppos[2] = {0, 0};

void ui_render() {
	static LARGE_INTEGER last_tick = { .QuadPart = 0 };
	static LARGE_INTEGER pref_freq = { .QuadPart = 0 };
	if (pref_freq.QuadPart == 0)
		QueryPerformanceFrequency(&pref_freq);
	LARGE_INTEGER now_tick;
	QueryPerformanceCounter(&now_tick);
	double dt = (double)(now_tick.QuadPart - last_tick.QuadPart) / (double)pref_freq.QuadPart;
	last_tick = now_tick;
	if (!win_shown)
		return;
	if (ImGui::Begin("Boshy Hack")) {
        ImGui::Text("Boshy hack by Pixelsuft");
        ImGui::Text("Real FPS: %i", (int)(dt <= 0.00001 ? 10000 : (int)(1.0 / dt)));
        ImGui::Text("Player position (Doesn't work): (%i, %i)", ppos[0], ppos[1]);
		ImGui::Checkbox("No cursor move/kill", &no_cursor_kill);
		ImGui::Checkbox("Skip message boxes", &skip_msg_box);
		/*
		ImGui::Checkbox("Force static seed for new RNG (Ganon, Warcraft...)", &new_rng_custom_seed);
		if (ImGui::InputInt("Value#NewRngSeed", &new_rng_seed)) {
			if (new_rng_seed < 0)
				new_rng_seed = 0;
			else if (new_rng_seed > 0xffff)
				new_rng_seed = 0xffff;
		}
		*/
		ImGui::Checkbox("Force static value for new RNG (Ganon, Warcraft...)", &new_rng_custom);
		if (ImGui::InputInt("Value##NewRngValue", &new_rng)) {
			if (new_rng < 0)
				new_rng = 0;
			else if (new_rng > RAND_MAX)
				new_rng = RAND_MAX;
        }
        ImGui::Checkbox("God Mode", &god_mode);
        ImGui::Checkbox("No Scene Transitions", &no_trans);
        ImGui::Checkbox("Disable viewport (No screen image manipulation)", &no_viewport);
	}
	ImGui::End();
}

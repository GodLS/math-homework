#pragma once
#include "Misc.h"
#include <string>
//#include "imgui/imgui.h"
//#include "imgui/imgui_internal.h"
#include "valve_sdk/sdk.hpp"
#include "Settings.h"
#include "imgui2/imgui.h"
#include "imgui2/imgui_internal.h"

//static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

using namespace ImGui;

static const char* keyNames[] =
{
	encrypt(""),
	encrypt("Mouse 1"),
	encrypt("Mouse 2"),
	encrypt("Cancel"),
	encrypt("Middle Mouse"),
	encrypt("Mouse 4"),
	encrypt("Mouse 5"),
	encrypt(""),
	encrypt("Backspace"),
	encrypt("Tab"),
	encrypt(""),
	encrypt(""),
	encrypt("Clear"),
	encrypt("Enter"),
	encrypt(""),
	encrypt(""),
	encrypt("Shift"),
	encrypt("Control"),
	encrypt("Alt"),
	encrypt("Pause"),
	encrypt("Caps"),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt("Escape"),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt("Space"),
	encrypt("Page Up"),
	encrypt("Page Down"),
	encrypt("End"),
	encrypt("Home"),
	encrypt("Left"),
	encrypt("Up"),
	encrypt("Right"),
	encrypt("Down"),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt("Print"),
	encrypt("Insert"),
	encrypt("Delete"),
	encrypt(""),
	encrypt("0"),
	encrypt("1"),
	encrypt("2"),
	encrypt("3"),
	encrypt("4"),
	encrypt("5"),
	encrypt("6"),
	encrypt("7"),
	encrypt("8"),
	encrypt("9"),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt("A"),
	encrypt("B"),
	encrypt("C"),
	encrypt("D"),
	encrypt("E"),
	encrypt("F"),
	encrypt("G"),
	encrypt("H"),
	encrypt("I"),
	encrypt("J"),
	encrypt("K"),
	encrypt("L"),
	encrypt("M"),
	encrypt("N"),
	encrypt("O"),
	encrypt("P"),
	encrypt("Q"),
	encrypt("R"),
	encrypt("S"),
	encrypt("T"),
	encrypt("U"),
	encrypt("V"),
	encrypt("W"),
	encrypt("X"),
	encrypt("Y"),
	encrypt("Z"),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt(""),
	encrypt("Numpad 0"),
	encrypt("Numpad 1"),
	encrypt("Numpad 2"),
	encrypt("Numpad 3"),
	encrypt("Numpad 4"),
	encrypt("Numpad 5"),
	encrypt("Numpad 6"),
	encrypt("Numpad 7"),
	encrypt("Numpad 8"),
	encrypt("Numpad 9"),
	encrypt("Multiply"),
	encrypt("Add"),
	encrypt(""),
	encrypt("Subtract"),
	encrypt("Decimal"),
	encrypt("Divide"),
	encrypt("F1"),
	encrypt("F2"),
	encrypt("F3"),
	encrypt("F4"),
	encrypt("F5"),
	encrypt("F6"),
	encrypt("F7"),
	encrypt("F8"),
	encrypt("F9"),
	encrypt("F10"),
	encrypt("F11"),
	encrypt("F12"),
};

bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(unique_id);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
	ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;
	if (label_size.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);

	const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
	if (label_size.x > 0)
	{
		ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (label_size.x > 0.0f)
		RenderText(check_bb.GetTL(), Buf);

	return pressed;
}

void KeyBindButton(ButtonCode_t* key)
{
	bool clicked = false;

	std::string text = g_InputSystem->ButtonCodeToString(*key);
	std::string unique_id = std::to_string((int)key);

	if (*key <= BUTTON_CODE_NONE)
		text = "Key not set";

	if (Settings::input_shouldListen && Settings::input_receivedKeyval == key) {
		clicked = true;
		text = "...";
	}
	text += "]";

	ImGui::SameLine();
	LabelClick("[", "[", text.c_str(), &clicked, unique_id.c_str());

	if (clicked)
	{
		Settings::input_shouldListen = true;
		Settings::input_receivedKeyval = key;
	}

	if (*key == KEY_DELETE)
	{
		*key = BUTTON_CODE_NONE;
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Bind the \"del\" key to remove current bind.");
}


inline void DrawMenu()
{
	ImGui::Begin("bestest csgo cheat world", &Settings::menuopen, ImVec2(540, 500), 0.9f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	{
	//ImGui::Text("Aimkey");
	ImGui::Text(encrypt("Keys"));
	ImGui::Combo(encrypt("Main"), &Settings::legit_aimkey, keyNames, ARRAYSIZE(keyNames));
	ImGui::Combo(encrypt("Backup"), &Settings::legit_aimkey2, keyNames, ARRAYSIZE(keyNames));
	//ImGui::Checkbox(encrypt("Always on"), &Settings::legitbotalwayson);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	//KeyBindButton(&Settings::legit_aimkey);
	//ImGui::Text("Aimkey 2");
	//KeyBindButton(&Settings::legit_aimkey2);
	ImGui::End();
	}
}
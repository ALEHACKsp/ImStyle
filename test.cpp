
#include <SFML/Graphics.hpp>
#include <sfmlimgui.h>
#include <sini.hpp>
#include "ImStyle.hpp"
#include <iostream>

void ShowTestWindow();
void ShowStyleEditor();

int main()
{
	sf::RenderWindow window({ 1280, 700 }, "ImStyle");
	ImGui::SFML::Init(window);
	ImStyle::LoadStyleFromINI("style.ini");
	sf::Event e;
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			ImGui::SFML::ProcessEvent(e);
			if (e.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::White);
		ImGui::SFML::NewFrame();
		ShowTestWindow();
		ShowStyleEditor();
		ImGui::Render();
		window.display();
	}

	ImStyle::SaveStyleToINI("style.ini");
	ImGui::SFML::Shutdown();
	return EXIT_SUCCESS;
}

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#ifdef _WIN32
#define IM_NEWLINE "\r\n"
#else
#define IM_NEWLINE "\n"
#endif

void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(desc);
}

void ShowTestWindow()
{
	bool* p_opened = (bool*)true;

	static bool no_titlebar = false;
	static bool no_border = true;
	static bool no_resize = false;
	static bool no_move = false;
	static bool no_scrollbar = false;
	static bool no_collapse = false;
	static bool no_menu = false;
	static float bg_alpha = -0.01f; // <0: default

									// Demonstrate the various window flags. Typically you would just use the default.
	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
	if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
	if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
	if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
	if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
	if (!ImGui::Begin("ImGui Demo", p_opened, ImVec2(550, 680), bg_alpha, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	//ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
	ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

	ImGui::Text("Dear ImGui says hello.");

	// Menu
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Examples"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Help"))
	{
		ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
	}

	if (ImGui::CollapsingHeader("Window options"))
	{
		ImGui::Checkbox("No titlebar", &no_titlebar); ImGui::SameLine(150);
		ImGui::Checkbox("No border", &no_border); ImGui::SameLine(300);
		ImGui::Checkbox("No resize", &no_resize);
		ImGui::Checkbox("No move", &no_move); ImGui::SameLine(150);
		ImGui::Checkbox("No scrollbar", &no_scrollbar); ImGui::SameLine(300);
		ImGui::Checkbox("No collapse", &no_collapse);
		ImGui::Checkbox("No menu", &no_menu);

		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Window Fill Alpha", &bg_alpha, 0.005f, -0.01f, 1.0f, bg_alpha < 0.0f ? "(default)" : "%.3f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
		ImGui::PopItemWidth();

		if (ImGui::TreeNode("Style"))
		{
			ShowStyleEditor();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size))
		{
			ImGui::SameLine(); ShowHelpMarker("Tip: Load fonts with io.Fonts->AddFontFromFileTTF()\nbefore calling io.Fonts->GetTex* functions.");
			ImFontAtlas* atlas = ImGui::GetIO().Fonts;
			if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
			{
				ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
				ImGui::TreePop();
			}
			ImGui::PushItemWidth(100);
			for (int i = 0; i < atlas->Fonts.Size; i++)
			{
				ImFont* font = atlas->Fonts[i];
				ImGui::BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
				ImGui::TreePush((void*)(intptr_t)i);
				if (i > 0) { ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { atlas->Fonts[i] = atlas->Fonts[0]; atlas->Fonts[0] = font; } }
				ImGui::PushFont(font);
				ImGui::Text("The quick brown fox jumps over the lazy dog");
				ImGui::PopFont();
				if (ImGui::TreeNode("Details"))
				{
					ImGui::DragFloat("font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");             // scale only this font
					ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
					ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
					for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
					{
						ImFontConfig* cfg = &font->ConfigData[config_i];
						ImGui::BulletText("Input %d: \'%s\'\nOversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			static float window_scale = 1.0f;
			ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
			ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
			ImGui::PopItemWidth();
			ImGui::SetWindowFontScale(window_scale);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Logging"))
		{
			ImGui::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded. You can also call ImGui::LogText() to output directly to the log without a visual output.");
			ImGui::LogButtons();
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Widgets"))
	{
		if (ImGui::TreeNode("Tree"))
		{
			for (int i = 0; i < 5; i++)
			{
				if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
				{
					ImGui::Text("blah blah");
					ImGui::SameLine();
					if (ImGui::SmallButton("print"))
						printf("Child %d pressed", i);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Bullets"))
		{
			ImGui::BulletText("Bullet point 1");
			ImGui::BulletText("Bullet point 2\nOn multiple lines");
			ImGui::Bullet(); ImGui::Text("Bullet point 3 (two calls)");
			ImGui::Bullet(); ImGui::SmallButton("Button");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Colored Text"))
		{
			// Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
			ImGui::TextDisabled("Disabled");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Word Wrapping"))
		{
			// Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
			ImGui::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
			ImGui::Spacing();

			static float wrap_width = 200.0f;
			ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

			ImGui::Text("Test paragraph 1:");
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), 0xFFFF00FF);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
			ImGui::Text("lazy dog. This paragraph is made to fit within %.0f pixels. The quick brown fox jumps over the lazy dog.", wrap_width);
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
			ImGui::PopTextWrapPos();

			ImGui::Text("Test paragraph 2:");
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight()), 0xFFFF00FF);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
			ImGui::Text("aaaaaaaa bbbbbbbb, cccccccc,dddddddd. eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
			ImGui::PopTextWrapPos();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("UTF-8 Text"))
		{
			// UTF-8 test with Japanese characters
			// (needs a suitable font, try Arial Unicode or M+ fonts http://mplus-fonts.sourceforge.jp/mplus-outline-fonts/index-en.html)
			// Most compiler appears to support UTF-8 in source code (with Visual Studio you need to save your file as 'UTF-8 without signature')
			// However for the sake for maximum portability here we are *not* including raw UTF-8 character in this source file, instead we encode the string with hexadecimal constants.
			// In your own application be reasonable and use UTF-8 in source or retrieve the data from file system!
			// Note that characters values are preserved even if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
			ImGui::TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->LoadFromFileTTF() manually to load extra character ranges.");
			ImGui::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)");
			ImGui::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
			static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
			ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Images"))
		{
			ImGui::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");
			ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
			float tex_w = (float)ImGui::GetIO().Fonts->TexWidth;
			float tex_h = (float)ImGui::GetIO().Fonts->TexHeight;
			ImTextureID tex_id = ImGui::GetIO().Fonts->TexID;
			ImGui::Text("%.0fx%.0f", tex_w, tex_h);
			ImGui::Image(tex_id, ImVec2(tex_w, tex_h), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				float focus_sz = 32.0f;
				float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
				float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
				ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
				ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
				ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
				ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
				ImGui::Image(tex_id, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
				ImGui::EndTooltip();
			}
			ImGui::TextWrapped("And now some textured buttons..");
			static int pressed_count = 0;
			for (int i = 0; i < 8; i++)
			{
				if (i > 0)
					ImGui::SameLine();
				ImGui::PushID(i);
				int frame_padding = -1 + i;     // -1 = uses default padding
				if (ImGui::ImageButton(tex_id, ImVec2(32, 32), ImVec2(0, 0), ImVec2(32.0f / tex_w, 32 / tex_h), frame_padding, ImColor(0, 0, 0, 255)))
					pressed_count += 1;
				ImGui::PopID();
			}
			ImGui::Text("Pressed %d times.", pressed_count);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Selectables"))
		{
			if (ImGui::TreeNode("Basic"))
			{
				static bool selected[4] = { false, true, false, false };
				ImGui::Selectable("1. I am selectable", &selected[0]);
				ImGui::Selectable("2. I am selectable", &selected[1]);
				ImGui::Text("3. I am not selectable");
				ImGui::Selectable("4. I am selectable", &selected[2]);
				if (ImGui::Selectable("5. I am double clickable", selected[3], ImGuiSelectableFlags_AllowDoubleClick))
					if (ImGui::IsMouseDoubleClicked(0))
						selected[3] = !selected[3];
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Rendering more text into the same block"))
			{
				static bool selected[3] = { false, false, false };
				ImGui::Selectable("main.c", &selected[0]);    ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
				ImGui::Selectable("Hello.cpp", &selected[1]); ImGui::SameLine(300); ImGui::Text("12,345 bytes");
				ImGui::Selectable("Hello.h", &selected[2]);   ImGui::SameLine(300); ImGui::Text(" 2,345 bytes");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Grid"))
			{
				static bool selected[16] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
				for (int i = 0; i < 16; i++)
				{
					ImGui::PushID(i);
					if (ImGui::Selectable("Me", &selected[i], 0, ImVec2(50, 50)))
					{
						int x = i % 4, y = i / 4;
						if (x > 0) selected[i - 1] ^= 1;
						if (x < 3) selected[i + 1] ^= 1;
						if (y > 0) selected[i - 4] ^= 1;
						if (y < 3) selected[i + 4] ^= 1;
					}
					if ((i % 4) < 3) ImGui::SameLine();
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Filtered Text Input"))
		{
			static char buf1[64] = ""; ImGui::InputText("default", buf1, 64);
			static char buf2[64] = ""; ImGui::InputText("decimal", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
			static char buf3[64] = ""; ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
			static char buf4[64] = ""; ImGui::InputText("uppercase", buf4, 64, ImGuiInputTextFlags_CharsUppercase);
			static char buf5[64] = ""; ImGui::InputText("no blank", buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
			struct TextFilters { static int FilterImGuiLetters(ImGuiTextEditCallbackData* data) { if (data->EventChar < 256 && strchr("imgui", (char)data->EventChar)) return 0; return 1; } };
			static char buf6[64] = ""; ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);

			ImGui::Text("Password input");
			static char bufpass[64] = "password123";
			ImGui::InputText("password", bufpass, 64, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
			ImGui::SameLine(); ShowHelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
			ImGui::InputText("password (clear)", bufpass, 64, ImGuiInputTextFlags_CharsNoBlank);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Multi-line Text Input"))
		{
			static bool read_only = false;
			static char text[1024 * 16] =
				"/*\n"
				" The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
				" the hexadecimal encoding of one offending instruction,\n"
				" more formally, the invalid operand with locked CMPXCHG8B\n"
				" instruction bug, is a design flaw in the majority of\n"
				" Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
				" processors (all in the P5 microarchitecture).\n"
				"*/\n\n"
				"label:\n"
				"\tlock cmpxchg8b eax\n";

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox("Read-only", &read_only);
			ImGui::PopStyleVar();
			ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));
			ImGui::TreePop();
		}

		static bool a = false;
		if (ImGui::Button("Button")) { printf("Clicked\n"); a ^= 1; }
		if (a)
		{
			ImGui::SameLine();
			ImGui::Text("Thanks for clicking me!");
		}

		static bool check = true;
		ImGui::Checkbox("checkbox", &check);

		static int e = 0;
		ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("radio c", &e, 2);

		// Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::Button("Click");
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}

		ImGui::Text("Hover over me");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("I am a tooltip");

		ImGui::SameLine();
		ImGui::Text("- or me");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("I am a fancy tooltip");
			static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
			ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
			ImGui::EndTooltip();
		}

		// Testing IMGUI_ONCE_UPON_A_FRAME macro
		//for (int i = 0; i < 5; i++)
		//{
		//  IMGUI_ONCE_UPON_A_FRAME
		//  {
		//      ImGui::Text("This will be displayed only once.");
		//  }
		//}

		ImGui::Separator();

		ImGui::LabelText("label", "Value");

		static int item = 1;
		ImGui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");   // Combo using values packed in a single constant string (for really quick combo)

		const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK" };
		static int item2 = -1;
		ImGui::Combo("combo scroll", &item2, items, IM_ARRAYSIZE(items));   // Combo using proper array. You can also pass a callback to retrieve array value, no need to create/copy an array just for that.

		{
			static char str0[128] = "Hello, world!";
			static int i0 = 123;
			static float f0 = 0.001f;
			ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
			ImGui::SameLine(); ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");

			ImGui::InputInt("input int", &i0);
			ImGui::SameLine(); ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

			ImGui::InputFloat("input float", &f0, 0.01f, 1.0f);

			static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
			ImGui::InputFloat3("input float3", vec4a);
		}

		{
			static int i1 = 50, i2 = 42;
			ImGui::DragInt("drag int", &i1, 1);
			ImGui::SameLine(); ShowHelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

			ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%.0f%%");

			static float f1 = 1.00f, f2 = 0.0067f;
			ImGui::DragFloat("drag float", &f1, 0.005f);
			ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
		}

		{
			static int i1 = 0;
			ImGui::SliderInt("slider int", &i1, -1, 3);
			ImGui::SameLine(); ShowHelpMarker("CTRL+click to input value.");

			static float f1 = 0.123f, f2 = 0.0f;
			ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat("slider log float", &f2, -10.0f, 10.0f, "%.4f", 3.0f);
			static float angle = 0.0f;
			ImGui::SliderAngle("slider angle", &angle);
		}

		static float col1[3] = { 1.0f,0.0f,0.2f };
		static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
		ImGui::ColorEdit3("color 1", col1);
		ImGui::SameLine(); ShowHelpMarker("Click on the colored square to change edit mode.\nCTRL+click on individual component to input value.\n");

		ImGui::ColorEdit4("color 2", col2);

		const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
		static int listbox_item_current = 1;
		ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

		//static int listbox_item_current2 = 2;
		//ImGui::PushItemWidth(-1);
		//ImGui::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
		//ImGui::PopItemWidth();

		if (ImGui::TreeNode("Range Widgets"))
		{
			ImGui::Unindent();

			static float begin = 10, end = 90;
			static int begin_i = 100, end_i = 1000;
			ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
			ImGui::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %.0f units", "Max: %.0f units");

			ImGui::Indent();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Multi-component Widgets"))
		{
			ImGui::Unindent();

			static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
			static int vec4i[4] = { 1, 5, 100, 255 };

			ImGui::InputFloat2("input float2", vec4f);
			ImGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
			ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
			ImGui::DragInt2("drag int2", vec4i, 1, 0, 255);
			ImGui::InputInt2("input int2", vec4i);
			ImGui::SliderInt2("slider int2", vec4i, 0, 255);
			ImGui::Spacing();

			ImGui::InputFloat3("input float3", vec4f);
			ImGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
			ImGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
			ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
			ImGui::InputInt3("input int3", vec4i);
			ImGui::SliderInt3("slider int3", vec4i, 0, 255);
			ImGui::Spacing();

			ImGui::InputFloat4("input float4", vec4f);
			ImGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
			ImGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
			ImGui::InputInt4("input int4", vec4i);
			ImGui::DragInt4("drag int4", vec4i, 1, 0, 255);
			ImGui::SliderInt4("slider int4", vec4i, 0, 255);

			ImGui::Indent();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Vertical Sliders"))
		{
			ImGui::Unindent();
			const float spacing = 4;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

			static int int_value = 0;
			ImGui::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
			ImGui::SameLine();

			static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
			ImGui::PushID("set1");
			for (int i = 0; i < 7; i++)
			{
				if (i > 0) ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
				ImGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
				if (ImGui::IsItemActive() || ImGui::IsItemHovered())
					ImGui::SetTooltip("%.3f", values[i]);
				ImGui::PopStyleColor(4);
				ImGui::PopID();
			}
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID("set2");
			static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
			const int rows = 3;
			const ImVec2 small_slider_size(18, (160.0f - (rows - 1)*spacing) / rows);
			for (int nx = 0; nx < 4; nx++)
			{
				if (nx > 0) ImGui::SameLine();
				ImGui::BeginGroup();
				for (int ny = 0; ny < rows; ny++)
				{
					ImGui::PushID(nx*rows + ny);
					ImGui::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
					if (ImGui::IsItemActive() || ImGui::IsItemHovered())
						ImGui::SetTooltip("%.3f", values2[nx]);
					ImGui::PopID();
				}
				ImGui::EndGroup();
			}
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID("set3");
			for (int i = 0; i < 4; i++)
			{
				if (i > 0) ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);
				ImGui::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f");
				ImGui::PopStyleVar();
				ImGui::PopID();
			}
			ImGui::PopID();
			ImGui::PopStyleVar();

			ImGui::Indent();
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Graphs widgets"))
	{
		static bool animate = true;
		ImGui::Checkbox("Animate", &animate);

		static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
		ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

		// Create a dummy array of contiguous float values to plot
		// Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
		static float values[90] = { 0 };
		static int values_offset = 0;
		if (animate)
		{
			static float refresh_time = ImGui::GetTime(); // Create dummy data at fixed 60 hz rate for the demo
			for (; ImGui::GetTime() > refresh_time + 1.0f / 60.0f; refresh_time += 1.0f / 60.0f)
			{
				static float phase = 0.0f;
				values[values_offset] = cosf(phase);
				values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
				phase += 0.10f*values_offset;
			}
		}
		ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));

		// Use functions to generate output
		// FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
		struct Funcs
		{
			static float Sin(void*, int i) { return sinf(i * 0.1f); }
			static float Saw(void*, int i) { return (i & 1) ? 1.0f : 0.0f; }
		};
		static int func_type = 0, display_count = 70;
		ImGui::Separator();
		ImGui::PushItemWidth(100); ImGui::Combo("func", &func_type, "Sin\0Saw\0"); ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::SliderInt("Sample count", &display_count, 1, 500);
		float(*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
		ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
		ImGui::Separator();

		// Animate a simple progress bar
		static float progress = 0.0f, progress_dir = 1.0f;
		if (animate)
		{
			progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
			if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
			if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
		}

		// Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Progress Bar");

		float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
		char buf[32];
		sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
	}

	if (ImGui::CollapsingHeader("Layout"))
	{
		if (ImGui::TreeNode("Child regions"))
		{
			ImGui::Text("Without border");
			static int line = 50;
			bool goto_line = ImGui::Button("Goto");
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			goto_line |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::BeginChild("Sub1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (int i = 0; i < 100; i++)
			{
				ImGui::Text("%04d: scrollable region", i);
				if (goto_line && line == i)
					ImGui::SetScrollHere();
			}
			if (goto_line && line >= 100)
				ImGui::SetScrollHere();
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
			ImGui::BeginChild("Sub2", ImVec2(0, 300), true);
			ImGui::Text("With border");
			ImGui::Columns(2);
			for (int i = 0; i < 100; i++)
			{
				if (i == 50)
					ImGui::NextColumn();
				char buf[32];
				sprintf(buf, "%08x", i * 5731);
				ImGui::Button(buf, ImVec2(-1.0f, 0.0f));
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Widgets Width"))
		{
			static float f = 0.0f;
			ImGui::Text("PushItemWidth(100)");
			ImGui::SameLine(); ShowHelpMarker("Fixed width.");
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("float##1", &f);
			ImGui::PopItemWidth();

			ImGui::Text("PushItemWidth(GetWindowWidth() * 0.5f)");
			ImGui::SameLine(); ShowHelpMarker("Half of window width.");
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::DragFloat("float##2", &f);
			ImGui::PopItemWidth();

			ImGui::Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f)");
			ImGui::SameLine(); ShowHelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
			ImGui::DragFloat("float##3", &f);
			ImGui::PopItemWidth();

			ImGui::Text("PushItemWidth(-100)");
			ImGui::SameLine(); ShowHelpMarker("Align to right edge minus 100");
			ImGui::PushItemWidth(-100);
			ImGui::DragFloat("float##4", &f);
			ImGui::PopItemWidth();

			ImGui::Text("PushItemWidth(-1)");
			ImGui::SameLine(); ShowHelpMarker("Align to right edge");
			ImGui::PushItemWidth(-1);
			ImGui::DragFloat("float##5", &f);
			ImGui::PopItemWidth();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Basic Horizontal Layout"))
		{
			ImGui::TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceeding item)");

			// Text
			ImGui::Text("Two items: Hello"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

			// Adjust spacing
			ImGui::Text("More spacing: Hello"); ImGui::SameLine(0, 20);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

			// Button
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Text("Normal buttons"); ImGui::SameLine();
			ImGui::Button("Banana"); ImGui::SameLine();
			ImGui::Button("Apple"); ImGui::SameLine();
			ImGui::Button("Corniflower");

			// Button
			ImGui::Text("Small buttons"); ImGui::SameLine();
			ImGui::SmallButton("Like this one"); ImGui::SameLine();
			ImGui::Text("can fit within a text block.");

			// Aligned to arbitrary position. Easy/cheap column.
			ImGui::Text("Aligned");
			ImGui::SameLine(150); ImGui::Text("x=150");
			ImGui::SameLine(300); ImGui::Text("x=300");
			ImGui::Text("Aligned");
			ImGui::SameLine(150); ImGui::SmallButton("x=150");
			ImGui::SameLine(300); ImGui::SmallButton("x=300");

			// Checkbox
			static bool c1 = false, c2 = false, c3 = false, c4 = false;
			ImGui::Checkbox("My", &c1); ImGui::SameLine();
			ImGui::Checkbox("Tailor", &c2); ImGui::SameLine();
			ImGui::Checkbox("Is", &c3); ImGui::SameLine();
			ImGui::Checkbox("Rich", &c4);

			// Various
			static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
			ImGui::PushItemWidth(80);
			const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
			static int item = -1;
			ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
			ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
			ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
			ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(80);
			ImGui::Text("Lists:");
			static int selection[4] = { 0, 1, 2, 3 };
			for (int i = 0; i < 4; i++)
			{
				if (i > 0) ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
				ImGui::PopID();
				//if (ImGui::IsItemHovered()) ImGui::SetTooltip("ListBox %d hovered", i);
			}
			ImGui::PopItemWidth();

			// Dummy
			ImVec2 sz(30, 30);
			ImGui::Button("A", sz); ImGui::SameLine();
			ImGui::Dummy(sz); ImGui::SameLine();
			ImGui::Button("B", sz);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Groups"))
		{
			ImGui::TextWrapped("(Using ImGui::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.)");
			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
				ImGui::Button("AAA");
				ImGui::SameLine();
				ImGui::Button("BBB");
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Button("CCC");
				ImGui::Button("DDD");
				ImGui::EndGroup();
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Group hovered");
				ImGui::SameLine();
				ImGui::Button("EEE");
				ImGui::EndGroup();
			}
			// Capture the group size and create widgets using the same size
			ImVec2 size = ImGui::GetItemRectSize();
			const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
			ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

			ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
			ImGui::SameLine();
			ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
			ImGui::EndGroup();
			ImGui::SameLine();

			ImGui::Button("LEVERAGE\nBUZZWORD", size);
			ImGui::SameLine();

			ImGui::ListBoxHeader("List", size);
			ImGui::Selectable("Selected", true);
			ImGui::Selectable("Not Selected", false);
			ImGui::ListBoxFooter();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Text Baseline Alignment"))
		{
			ImGui::TextWrapped("(This is testing the vertical alignment that occurs on text to keep it at the same baseline as widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets)");

			ImGui::Text("One\nTwo\nThree"); ImGui::SameLine();
			ImGui::Text("Hello\nWorld"); ImGui::SameLine();
			ImGui::Text("Banana");

			ImGui::Text("Banana"); ImGui::SameLine();
			ImGui::Text("Hello\nWorld"); ImGui::SameLine();
			ImGui::Text("One\nTwo\nThree");

			ImGui::Button("HOP"); ImGui::SameLine();
			ImGui::Text("Banana"); ImGui::SameLine();
			ImGui::Text("Hello\nWorld"); ImGui::SameLine();
			ImGui::Text("Banana");

			ImGui::Button("HOP"); ImGui::SameLine();
			ImGui::Text("Hello\nWorld"); ImGui::SameLine();
			ImGui::Text("Banana");

			ImGui::Button("TEST"); ImGui::SameLine();
			ImGui::Text("TEST"); ImGui::SameLine();
			ImGui::SmallButton("TEST");

			ImGui::AlignFirstTextHeightToWidgets(); // If your line starts with text, call this to align it to upcoming widgets.
			ImGui::Text("Text aligned to Widget"); ImGui::SameLine();
			ImGui::Button("Widget"); ImGui::SameLine();
			ImGui::Text("Widget"); ImGui::SameLine();
			ImGui::SmallButton("Widget");

			// Tree
			const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::Button("Button##1");
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::TreeNode("Node##1")) { for (int i = 0; i < 6; i++) ImGui::BulletText("Item %d..", i); ImGui::TreePop(); }    // Dummy tree data

			ImGui::AlignFirstTextHeightToWidgets();         // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget. Otherwise you can use SmallButton (smaller fit).
			bool tree_opened = ImGui::TreeNode("Node##2");  // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add child content.
			ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##2");
			if (tree_opened) { for (int i = 0; i < 6; i++) ImGui::BulletText("Item %d..", i); ImGui::TreePop(); }   // Dummy tree data

																													// Bullet
			ImGui::Button("Button##3");
			ImGui::SameLine(0.0f, spacing);
			ImGui::BulletText("Bullet text");

			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::BulletText("Node");
			ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##4");

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scrolling"))
		{
			ImGui::TextWrapped("(Use SetScrollHere() or SetScrollFromPosY() to scroll to a given position.)");
			static bool track = true;
			static int track_line = 50, scroll_to_px = 200;
			ImGui::Checkbox("Track", &track);
			ImGui::SameLine(130); track |= ImGui::DragInt("##line", &track_line, 0.25f, 0, 99, "Line %.0f");
			bool scroll_to = ImGui::Button("Scroll To");
			ImGui::SameLine(130); scroll_to |= ImGui::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "y = %.0f px");
			if (scroll_to) track = false;

			for (int i = 0; i < 5; i++)
			{
				if (i > 0) ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
				ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)i), ImVec2(ImGui::GetWindowWidth() * 0.17f, 200.0f), true);
				if (scroll_to)
					ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scroll_to_px, i * 0.25f);
				for (int line = 0; line < 100; line++)
				{
					if (track && line == track_line)
					{
						ImGui::TextColored(ImColor(255, 255, 0), "Line %d", line);
						ImGui::SetScrollHere(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
					}
					else
					{
						ImGui::Text("Line %d", line);
					}
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Horizontal Scrolling"))
		{
			ImGui::Bullet(); ImGui::TextWrapped("Horizontal scrolling for a window has to be enabled explicitly via the ImGuiWindowFlags_HorizontalScrollbar flag.");
			ImGui::Bullet(); ImGui::TextWrapped("You may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
			static int lines = 7;
			ImGui::SliderInt("Lines", &lines, 1, 15);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
			ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetItemsLineHeightWithSpacing() * 7 + 30), true, ImGuiWindowFlags_HorizontalScrollbar);
			for (int line = 0; line < lines; line++)
			{
				// Display random stuff
				int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
				for (int n = 0; n < num_buttons; n++)
				{
					if (n > 0) ImGui::SameLine();
					ImGui::PushID(n + line * 1000);
					char num_buf[16];
					const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : (sprintf(num_buf, "%d", n), num_buf);
					float hue = n*0.05f;
					ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue, 0.6f, 0.6f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue, 0.8f, 0.8f));
					ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
					ImGui::PopStyleColor(3);
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar(2);
			float scroll_x_delta = 0.0f;
			ImGui::SmallButton("<<"); if (ImGui::IsItemActive()) scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f;
			ImGui::SameLine(); ImGui::Text("Scroll from code"); ImGui::SameLine();
			ImGui::SmallButton(">>"); if (ImGui::IsItemActive()) scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f;
			if (scroll_x_delta != 0.0f)
			{
				ImGui::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
				ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
				ImGui::End();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Clipping"))
		{
			static ImVec2 size(100, 100), offset(50, 20);
			ImGui::TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
			ImGui::DragFloat2("size", (float*)&size, 0.5f, 0.0f, 200.0f, "%.0f");
			ImGui::TextWrapped("(Click and drag)");
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec4 clip_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
			ImGui::InvisibleButton("##dummy", size);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging()) { offset.x += ImGui::GetIO().MouseDelta.x; offset.y += ImGui::GetIO().MouseDelta.y; }
			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(90, 90, 120, 255));
			ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize()*2.0f, ImVec2(pos.x + offset.x, pos.y + offset.y), ImColor(255, 255, 255, 255), "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Popups & Modal windows"))
	{
		if (ImGui::TreeNode("Popups"))
		{
			ImGui::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

			static int selected_fish = -1;
			const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
			static bool toggles[] = { true, false, false, false, false };

			if (ImGui::Button("Select.."))
				ImGui::OpenPopup("select");
			ImGui::SameLine();
			ImGui::Text(selected_fish == -1 ? "<None>" : names[selected_fish]);
			if (ImGui::BeginPopup("select"))
			{
				ImGui::Text("Aquarium");
				ImGui::Separator();
				for (int i = 0; i < IM_ARRAYSIZE(names); i++)
					if (ImGui::Selectable(names[i]))
						selected_fish = i;
				ImGui::EndPopup();
			}

			if (ImGui::Button("Toggle.."))
				ImGui::OpenPopup("toggle");
			if (ImGui::BeginPopup("toggle"))
			{
				for (int i = 0; i < IM_ARRAYSIZE(names); i++)
					ImGui::MenuItem(names[i], "", &toggles[i]);
				if (ImGui::BeginMenu("Sub-menu"))
				{
					ImGui::MenuItem("Click me");
					ImGui::EndMenu();
				}

				ImGui::Separator();
				ImGui::Text("Tooltip here");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("I am a tooltip over a popup");

				if (ImGui::Button("Stacked Popup"))
					ImGui::OpenPopup("another popup");
				if (ImGui::BeginPopup("another popup"))
				{
					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						ImGui::MenuItem(names[i], "", &toggles[i]);
					if (ImGui::BeginMenu("Sub-menu"))
					{
						ImGui::MenuItem("Click me");
						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Popup Menu.."))
				ImGui::OpenPopup("popup from button");
			if (ImGui::BeginPopup("popup from button"))
			{
				ImGui::EndPopup();
			}

			ImGui::Spacing();
			ImGui::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
			ImGui::Separator();
			// NB: As a quirk in this very specific example, we want to differentiate the parent of this menu from the parent of the various popup menus above.
			// To do so we are encloding the items in a PushID()/PopID() block to make them two different menusets. If we don't, opening any popup above and hovering our menu here
			// would open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it, which is the desired behavior for regular menus.
			ImGui::PushID("foo");
			ImGui::MenuItem("Menu item", "CTRL+M");
			if (ImGui::BeginMenu("Menu inside a regular window"))
			{
				ImGui::EndMenu();
			}
			ImGui::PopID();
			ImGui::Separator();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Context menus"))
		{
			static float value = 0.5f;
			ImGui::Text("Value = %.3f (<-- right-click here)", value);
			if (ImGui::BeginPopupContextItem("item context menu"))
			{
				if (ImGui::Selectable("Set to zero")) value = 0.0f;
				if (ImGui::Selectable("Set to PI")) value = 3.1415f;
				ImGui::EndPopup();
			}

			static ImVec4 color = ImColor(1.0f, 0.0f, 1.0f, 1.0f);
			ImGui::ColorButton(color);
			if (ImGui::BeginPopupContextItem("color context menu"))
			{
				ImGui::Text("Edit color");
				ImGui::ColorEdit3("##edit", (float*)&color);
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine(); ImGui::Text("(<-- right-click here)");

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Modals"))
		{
			ImGui::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

			if (ImGui::Button("Delete.."))
				ImGui::OpenPopup("Delete?");
			if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
				ImGui::Separator();

				static bool dont_ask_me_next_time = false;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
				ImGui::PopStyleVar();

				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			if (ImGui::Button("Stacked modals.."))
				ImGui::OpenPopup("Stacked 1");
			if (ImGui::BeginPopupModal("Stacked 1"))
			{
				ImGui::Text("Hello from Stacked The First");

				if (ImGui::Button("Another one.."))
					ImGui::OpenPopup("Stacked 2");
				if (ImGui::BeginPopupModal("Stacked 2"))
				{
					ImGui::Text("Hello from Stacked The Second");
					if (ImGui::Button("Close"))
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Columns"))
	{
		// Basic columns
		if (ImGui::TreeNode("Basic"))
		{
			ImGui::Columns(4, "mycolumns");
			ImGui::Separator();
			ImGui::Text("ID"); ImGui::NextColumn();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Path"); ImGui::NextColumn();
			ImGui::Text("Flags"); ImGui::NextColumn();
			ImGui::Separator();
			const char* names[3] = { "One", "Two", "Three" };
			const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
			static int selected = -1;
			for (int i = 0; i < 3; i++)
			{
				char label[32];
				sprintf(label, "%04d", i);
				if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
					selected = i;
				ImGui::NextColumn();
				ImGui::Text(names[i]); ImGui::NextColumn();
				ImGui::Text(paths[i]); ImGui::NextColumn();
				ImGui::Text("...."); ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}

		// Scrolling columns
		/*
		if (ImGui::TreeNode("Scrolling"))
		{
		ImGui::BeginChild("##header", ImVec2(0, ImGui::GetTextLineHeightWithSpacing()+ImGui::GetStyle().ItemSpacing.y));
		ImGui::Columns(3);
		ImGui::Text("ID"); ImGui::NextColumn();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Path"); ImGui::NextColumn();
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::EndChild();
		ImGui::BeginChild("##scrollingregion", ImVec2(0, 60));
		ImGui::Columns(3);
		for (int i = 0; i < 10; i++)
		{
		ImGui::Text("%04d", i); ImGui::NextColumn();
		ImGui::Text("Foobar"); ImGui::NextColumn();
		ImGui::Text("/path/foobar/%04d/", i); ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::EndChild();
		ImGui::TreePop();
		}
		*/

		// Create multiple items in a same cell before switching to next column
		if (ImGui::TreeNode("Mixed items"))
		{
			ImGui::Columns(3, "mixed");
			ImGui::Separator();

			ImGui::Text("Hello");
			ImGui::Button("Banana");
			ImGui::NextColumn();

			ImGui::Text("ImGui");
			ImGui::Button("Apple");
			static float foo = 1.0f;
			ImGui::InputFloat("red", &foo, 0.05f, 0, 3);
			ImGui::Text("An extra line here.");
			ImGui::NextColumn();

			ImGui::Text("Sailor");
			ImGui::Button("Corniflower");
			static float bar = 1.0f;
			ImGui::InputFloat("blue", &bar, 0.05f, 0, 3);
			ImGui::NextColumn();

			if (ImGui::CollapsingHeader("Category A")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
			if (ImGui::CollapsingHeader("Category B")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
			if (ImGui::CollapsingHeader("Category C")) ImGui::Text("Blah blah blah"); ImGui::NextColumn();
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}

		// Word wrapping
		if (ImGui::TreeNode("Word-wrapping"))
		{
			ImGui::Columns(2, "word-wrapping");
			ImGui::Separator();
			ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
			ImGui::TextWrapped("Hello Left");
			ImGui::NextColumn();
			ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
			ImGui::TextWrapped("Hello Right");
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Borders"))
		{
			static bool h_borders = true;
			static bool v_borders = true;
			ImGui::Checkbox("horizontal", &h_borders);
			ImGui::SameLine();
			ImGui::Checkbox("vertical", &v_borders);
			ImGui::Columns(4, NULL, v_borders);
			if (h_borders) ImGui::Separator();
			for (int i = 0; i < 8; i++)
			{
				ImGui::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
				ImGui::NextColumn();
			}
			ImGui::Columns(1);
			if (h_borders) ImGui::Separator();
			ImGui::TreePop();
		}

		bool node_opened = ImGui::TreeNode("Tree within single cell");
		ImGui::SameLine(); ShowHelpMarker("NB: Tree node must be poped before ending the cell.\nThere's no storage of state per-cell.");
		if (node_opened)
		{
			ImGui::Columns(2, "tree items");
			ImGui::Separator();
			if (ImGui::TreeNode("Hello")) { ImGui::BulletText("Sailor"); ImGui::TreePop(); } ImGui::NextColumn();
			if (ImGui::TreeNode("Bonjour")) { ImGui::BulletText("Marin"); ImGui::TreePop(); } ImGui::NextColumn();
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Filtering"))
	{
		static ImGuiTextFilter filter;
		ImGui::Text("Filter usage:\n"
			"  \"\"         display all lines\n"
			"  \"xxx\"      display lines containing \"xxx\"\n"
			"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
			"  \"-xxx\"     hide lines containing \"xxx\"");
		filter.Draw();
		const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
		for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
			if (filter.PassFilter(lines[i]))
				ImGui::BulletText("%s", lines[i]);
	}

	if (ImGui::CollapsingHeader("Keyboard, Mouse & Focus"))
	{
		if (ImGui::TreeNode("Tabbing"))
		{
			ImGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
			static char buf[32] = "dummy";
			ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
			ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
			ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
			ImGui::PushAllowKeyboardFocus(false);
			ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
			//ImGui::SameLine(); ShowHelperMarker("Use ImGui::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
			ImGui::PopAllowKeyboardFocus();
			ImGui::InputText("5", buf, IM_ARRAYSIZE(buf));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Focus from code"))
		{
			bool focus_1 = ImGui::Button("Focus on 1"); ImGui::SameLine();
			bool focus_2 = ImGui::Button("Focus on 2"); ImGui::SameLine();
			bool focus_3 = ImGui::Button("Focus on 3");
			int has_focus = 0;
			static char buf[128] = "click on a button to set focus";

			if (focus_1) ImGui::SetKeyboardFocusHere();
			ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
			if (ImGui::IsItemActive()) has_focus = 1;

			if (focus_2) ImGui::SetKeyboardFocusHere();
			ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
			if (ImGui::IsItemActive()) has_focus = 2;

			ImGui::PushAllowKeyboardFocus(false);
			if (focus_3) ImGui::SetKeyboardFocusHere();
			ImGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
			if (ImGui::IsItemActive()) has_focus = 3;
			ImGui::PopAllowKeyboardFocus();
			if (has_focus)
				ImGui::Text("Item with focus: %d", has_focus);
			else
				ImGui::Text("Item with focus: <none>");
			ImGui::TextWrapped("Cursor & selection are preserved when refocusing last used item in code.");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Dragging"))
		{
			ImGui::TextWrapped("You can use ImGui::GetItemActiveDragDelta() to query for the dragged amount on any widget.");
			ImGui::Button("Drag Me");
			if (ImGui::IsItemActive())
			{
				// Draw a line between the button and the mouse cursor
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->PushClipRectFullScreen();
				draw_list->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
				draw_list->PopClipRect();
				ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
				ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
				ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
				ImGui::SameLine(); ImGui::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f), MouseDelta (%.1f, %.1f)", value_raw.x, value_raw.y, value_with_lock_threshold.x, value_with_lock_threshold.y, mouse_delta.x, mouse_delta.y);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Keyboard & Mouse State"))
		{
			ImGuiIO& io = ImGui::GetIO();

			ImGui::Text("MousePos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
			ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("MouseWheel: %.1f", io.MouseWheel);

			ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (%.02f secs)", i, io.KeysDownDuration[i]); }
			ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::Text("%d", i); }
			ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::Text("%d", i); }
			ImGui::Text("KeyMods: %s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");

			ImGui::Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
			ImGui::Text("WantCaptureKeyboard: %s", io.WantCaptureKeyboard ? "true" : "false");
			ImGui::Text("WantTextInput: %s", io.WantTextInput ? "true" : "false");

			ImGui::Button("Hovering me sets the\nkeyboard capture flag");
			if (ImGui::IsItemHovered())
				ImGui::CaptureKeyboardFromApp(true);
			ImGui::SameLine();
			ImGui::Button("Holding me clears the\nthe keyboard capture flag");
			if (ImGui::IsItemActive())
				ImGui::CaptureKeyboardFromApp(false);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Mouse cursors"))
		{
			ImGui::TextWrapped("Your application can render a different mouse cursor based on what ImGui::GetMouseCursor() returns. You can also set io.MouseDrawCursor to ask ImGui to render the cursor for you in software.");
			ImGui::Checkbox("io.MouseDrawCursor", &ImGui::GetIO().MouseDrawCursor);
			ImGui::Text("Hover to see mouse cursors:");
			for (int i = 0; i < ImGuiMouseCursor_Count_; i++)
			{
				char label[32];
				sprintf(label, "Mouse cursor %d", i);
				ImGui::Bullet(); ImGui::Selectable(label, false);
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(i);
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void ShowStyleEditor()
{
	ImGuiStyle* ref = nullptr;
	ImGuiStyle& style = ImGui::GetStyle();

	const ImGuiStyle def; // Default style
	if (ImGui::Button("Revert Style"))
		style = ref ? *ref : def;
	if (ref)
	{
		ImGui::SameLine();
		if (ImGui::Button("Save Style"))
			*ref = style;
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

	if (ImGui::TreeNode("Rendering"))
	{
		ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
		ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
		if (style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
		ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
		ImGui::DragFloat("Window Fill Alpha Default", &style.WindowFillAlphaDefault, 0.005f, 0.0f, 1.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Sizes"))
	{
		ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
		ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
		ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Colors"))
	{
		static int output_dest = 0;
		static bool output_only_modified = false;
		if (ImGui::Button("Copy Colors"))
		{
			if (output_dest == 0)
				ImGui::LogToClipboard();
			else
				ImGui::LogToTTY();
			ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();" IM_NEWLINE);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const ImVec4& col = style.Colors[i];
				const char* name = ImGui::GetStyleColName(i);
				if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
					ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
			}
			ImGui::LogFinish();
		}
		ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY"); ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

		static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
		ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
		ImGui::SameLine();
		ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
		ImGui::SameLine();
		ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);
		//ImGui::Text("Tip: Click on colored square to change edit mode.");

		static ImGuiTextFilter filter;
		filter.Draw("Filter colors", 200);

		ImGui::BeginChild("#colors", ImVec2(0, 300), true);
		ImGui::PushItemWidth(-160);
		ImGui::ColorEditMode(edit_mode);
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColName(i);
			if (!filter.PassFilter(name))
				continue;
			ImGui::PushID(i);
			ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
			if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
			{
				ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
				if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
			}
			ImGui::PopID();
		}
		ImGui::PopItemWidth();
		ImGui::EndChild();

		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

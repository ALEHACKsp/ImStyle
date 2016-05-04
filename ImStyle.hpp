
#if !defined(IMSTYLE_H_)
#define IMSTYLE_H_

#include <imgui.h>
#include <string>
#include <sini.hpp>

namespace ImStyle
{
	bool LoadStyleFromINI(std::string path)
	{
		ImGuiStyle &style = ImGui::GetStyle();
		sini::INI ini;
		if (!ini.LoadFromFile(path)) return false;

		for (sini::Section &s : ini.sections)
		{
			if (s.name == "Colors")
			{
				for (sini::Property &p : s.properties)
				{
					auto sep = std::find(p.key.begin(), p.key.end(), ':');
					if (sep != p.key.end())
					{
						std::string index_str(p.key.begin(), sep);
						int index = std::atoi(index_str.c_str());
						char range = *(sep + 1);
						if (range == 'r') style.Colors[index].x = p.ToFloat();
						else if (range == 'g') style.Colors[index].y = p.ToFloat();
						else if (range == 'b') style.Colors[index].z = p.ToFloat();
						else if (range == 'a') style.Colors[index].w = p.ToFloat();
					}
				}
			}
			else
			{
				for (sini::Property &p : s.properties)
				{
					// Rendering
					if (p.key == "AALines") style.AntiAliasedLines = p.ToBool();
					else if (p.key == "AAShapes") style.AntiAliasedShapes = p.ToBool();
					else if (p.key == "CurveTessellationTol") style.CurveTessellationTol = p.ToFloat();
					else if (p.key == "Alpha") style.Alpha = p.ToFloat();
					else if (p.key == "WinAlpha") style.WindowFillAlphaDefault = p.ToFloat();

					// Sizes
					else if (p.key == "WindowPadding_x") style.WindowPadding.x = p.ToFloat();
					else if (p.key == "WindowPadding_y") style.WindowPadding.y = p.ToFloat();
					else if (p.key == "WindowRounding") style.WindowRounding = p.ToFloat();
					else if (p.key == "ChildWindowRounding") style.ChildWindowRounding = p.ToFloat();
					else if (p.key == "FramePadding_x") style.FramePadding.x = p.ToFloat();
					else if (p.key == "FramePadding_y") style.FramePadding.y = p.ToFloat();
					else if (p.key == "FrameRounding") style.FrameRounding = p.ToFloat();
					else if (p.key == "ItemSpacing_x") style.ItemSpacing.x = p.ToFloat();
					else if (p.key == "ItemSpacing_y") style.ItemSpacing.y = p.ToFloat();
					else if (p.key == "ItemInnerSpacing_x") style.ItemInnerSpacing.x = p.ToFloat();
					else if (p.key == "ItemInnerSpacing_y") style.ItemInnerSpacing.y = p.ToFloat();
					else if (p.key == "TouchExtraPadding_x") style.TouchExtraPadding.x = p.ToFloat();
					else if (p.key == "TouchExtraPadding_y") style.TouchExtraPadding.y = p.ToFloat();
					else if (p.key == "IndentSpacing") style.IndentSpacing = p.ToFloat();
					else if (p.key == "ScrollbarSize") style.ScrollbarSize = p.ToFloat();
					else if (p.key == "GrabMinSize") style.GrabMinSize = p.ToFloat();
					else if (p.key == "GrabRounding") style.GrabRounding = p.ToFloat();
				}
			}
		}

        return true;
	}

	bool SaveStyleToINI(std::string path)
	{
		ImGuiStyle &style = ImGui::GetStyle();
		sini::INI ini;
#define ADDPROP(key, val) ini.AddProperty(#key, sini::Util::ToStr(val))

		// Rendering
		ini.AddSection("Rendering");
		ADDPROP(AALines, style.AntiAliasedLines);
		ADDPROP(AAShapes, style.AntiAliasedShapes);
		ADDPROP(CurveTessellationTol, style.CurveTessellationTol);
		ADDPROP(Alpha, style.Alpha);
		ADDPROP(WinAlpha, style.WindowFillAlphaDefault);

		// Sizes
		ini.AddSection("Sizes");
		ADDPROP(WindowPadding_x, style.WindowPadding.x);
		ADDPROP(WindowPadding_y, style.WindowPadding.y);
		ADDPROP(WindowRounding, style.WindowRounding);
		ADDPROP(ChildWindowRounding, style.ChildWindowRounding);
		ADDPROP(FramePadding_x, style.FramePadding.x);
		ADDPROP(FramePadding_y, style.FramePadding.y);
		ADDPROP(FrameRounding, style.FrameRounding);
		ADDPROP(ItemSpacing_x, style.ItemSpacing.x);
		ADDPROP(ItemSpacing_y, style.ItemSpacing.y);
		ADDPROP(ItemInnerSpacing_x, style.ItemInnerSpacing.x);
		ADDPROP(ItemInnerSpacing_y, style.ItemInnerSpacing.y);
		ADDPROP(TouchExtraPadding_x, style.TouchExtraPadding.x);
		ADDPROP(TouchExtraPadding_y, style.TouchExtraPadding.y);
		ADDPROP(IndentSpacing, style.IndentSpacing);
		ADDPROP(ScrollbarSize, style.ScrollbarSize);
		ADDPROP(ScrollbarRounding, style.ScrollbarRounding);
		ADDPROP(GrabMinSize, style.GrabMinSize);
		ADDPROP(GrabRounding, style.GrabRounding);

#undef ADDPROP
#define ADDPROP(i, c, val) ss << i << ":" << #c; ini.AddProperty(ss.str(), sini::Util::ToStr(val)); ss.str("");

		// Colors
		ini.AddSection("Colors");
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			std::stringstream ss;
			ADDPROP(i, r, style.Colors[i].x)
			ADDPROP(i, g, style.Colors[i].y)
			ADDPROP(i, b, style.Colors[i].z)
			ADDPROP(i, a, style.Colors[i].w)
		}

#undef ADDPROP
		return ini.SaveToFile(path);
	}
}

#endif

#include "imgui/imgui.h"
#include "imbase/window.hpp"
#include "imbase/launcher.hpp"

#include <vector>
#include <cstdio>
#include <fontconfig/fontconfig.h>

struct FontItem {
	FcChar8* name = nullptr;
	FcChar8* path = nullptr;
};

int main(void) {
	/* holds both patterns to match against the available fonts,
	   as well as the information about each font. */
	FcPattern* SearchPattern = FcPatternCreate();

	/* holds a list of pattern property names
	   used to indicate which properties are to
	   be returned in the patterns from FcFontList */
	FcObjectSet* ObjSet = FcObjectSetBuild(FC_FULLNAME, FC_STYLE, FC_FILE, NULL);

	/* holds a list of patterns that are used to
	   return the results of listing available fonts. */
	FcFontSet* FontList = FcFontList(NULL, SearchPattern, ObjSet);

	std::vector<FontItem> FontItems;
	FontItems.clear();

	for (int i = 0; i < FontList->nfont; ++i) {
		FcChar8* name = FcPatternFormat(FontList->fonts[i], (const FcChar8*)"%{fullname}");
		FcChar8* filePath = FcPatternFormat(FontList->fonts[i], (const FcChar8*)"%{file}");
		if (name && filePath) {
			FontItem item;
			item.name = name;
			item.path = filePath;
			FontItems.push_back(item);
		}
	}

	ImBase::Window::Init(600, 480, "Font Viewer");
	ImBase::Window::SetMaxFPS(60);
	ImGuiIO& io = ImGui::GetIO();

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();

		static float SizeY, PosY;
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Links")) {
				if (ImGui::MenuItem("GitHub")) {
					ImBase::Launcher::OpenUrl("https://github.com/pegvin/fontviewer");
				}
				ImGui::EndMenu();
			}
			PosY = ImGui::GetWindowPos().y;
			SizeY = ImGui::GetWindowSize().y;
			ImGui::EndMainMenuBar();
		}

		ImGui::SetNextWindowPos({ 0, PosY + SizeY });
		ImGui::SetNextWindowSize({ io.DisplaySize.x, io.DisplaySize.y - PosY });
		if (ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
			static ImGuiTextFilter filter;
			filter.Draw("Filter");

			bool ResultFound = false;
			static ImGuiListClipper clipper;

			ImGui::BeginChild("###ListChild");
				clipper.Begin(FontItems.size());
				while (clipper.Step()) {
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
						auto fItem = FontItems[i];
						if (filter.PassFilter((const char*)fItem.name)) {
							ImGui::TextUnformatted((const char*)fItem.name, NULL);
							if (ImGui::IsItemHovered()) {
								ImGui::SetTooltip("%s", fItem.path);
							}
							ResultFound = true;
						}
					}
					if (!ResultFound) {
						ImGui::TextUnformatted("No Result Found");
						clipper.End();
						ResultFound = false;
						break;
					}
				}
			ImGui::EndChild();

			ImGui::End();
		}

		ImBase::Window::EndFrame();
	}

	ImBase::Window::Destroy();

	for (auto fItem : FontItems) {
		FcStrFree(fItem.name);
		FcStrFree(fItem.path);
	}
	FontItems.clear();

	FcObjectSetDestroy(ObjSet);
	FcPatternDestroy(SearchPattern);
	FcFontSetDestroy(FontList);

	return 0;
}


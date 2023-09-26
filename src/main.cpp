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

	std::vector<FontItem*> FontItems;
	std::vector<FontItem*> FilteredFontItems;
	FontItems.clear();
	FilteredFontItems.clear();

	bool didFilter = false;

	for (int i = 0; i < FontList->nfont; ++i) {
		FcChar8* name = FcPatternFormat(FontList->fonts[i], (const FcChar8*)"%{fullname}");
		FcChar8* filePath = FcPatternFormat(FontList->fonts[i], (const FcChar8*)"%{file}");
		if (name && filePath) {
			FontItem* item = new FontItem();
			item->name = name;
			item->path = filePath;
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
			#define Q_BUFF_SIZE 1024
			static char QBuff[Q_BUFF_SIZE] = { 0 };
			bool didChange = ImGui::InputTextWithHint("Filter", "(Case Sensitive)", QBuff, Q_BUFF_SIZE);
			if (QBuff[0] == 0) {
				didFilter = false;
			} else if (didChange) {
				FilteredFontItems.clear();
				didFilter = true;
				for (unsigned int i = 0; i < FontItems.size(); ++i) {
					auto fItem = FontItems[i];
					if (strstr((const char*)fItem->name, QBuff) != NULL) {
						FilteredFontItems.push_back(fItem);
					}
				}
			}

			ImGui::BeginChild("###ListChild");
				auto clipperSize = didFilter ? FilteredFontItems.size() : FontItems.size();
				if (didFilter && FilteredFontItems.empty()) {
					ImGui::TextUnformatted("No Result Found");
				} else if (clipperSize > 0) {
					static ImGuiListClipper clipper;
					clipper.Begin(clipperSize);
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto fItem = didFilter ? FilteredFontItems[i] : FontItems[i];
							ImGui::TextUnformatted((const char*)fItem->name, NULL);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip("%s", fItem->path);
						}
					}
				} else {
					ImGui::TextUnformatted("No Font Entry Found");
				}
			ImGui::EndChild();

			ImGui::End();
		}

		ImBase::Window::EndFrame();
	}

	ImBase::Window::Destroy();

	for (auto fItem : FontItems) {
		FcStrFree(fItem->name);
		FcStrFree(fItem->path);
		delete fItem;
	}
	FontItems.clear();
	FilteredFontItems.clear();

	FcObjectSetDestroy(ObjSet);
	FcPatternDestroy(SearchPattern);
	FcFontSetDestroy(FontList);

	return 0;
}


#include "imgui/imgui.h"
#include "imbase/window.hpp"
#include "imbase/launcher.hpp"

int main(void) {
	ImBase::Window::Init(600, 480, "Font Viewer");
	ImBase::Window::SetMaxFPS(60);

	while (!ImBase::Window::ShouldClose()) {
		ImBase::Window::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Links")) {
				if (ImGui::MenuItem("GitHub")) {
					ImBase::Launcher::OpenUrl("https://github.com/pegvin/fontviewer");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImBase::Window::EndFrame();
	}

	ImBase::Window::Destroy();
	return 0;
}


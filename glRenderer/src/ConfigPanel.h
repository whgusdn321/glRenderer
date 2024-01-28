#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>

#include "RenderResources.h"
#include "Model.h"
#include "3rdParty/imgui/imgui.h"
#include "3rdParty/imgui/imgui_impl_glfw.h"
#include "3rdParty/imgui/imgui_impl_opengl3.h"

class ConfigPanel
{
public:
	explicit ConfigPanel(void* window, int width, int height, RenderResources& resource) 
		: renderResources(resource), width(width), height(height)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Alpha = 0.8f;

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
	}

	~ConfigPanel()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void draw()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Settings window
		ImGui::Begin("Settings",
			nullptr,
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_AlwaysAutoResize);

		drawSettings();
		ImGui::SetWindowPos(ImVec2(width - ImGui::GetWindowWidth(), 0));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void drawSettings()
	{
		// fps
		ImGui::Separator();
		ImGui::Text("fps: %.1f (%.2f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("triangles: %zu", renderResources.modelPtr->primitiveCnt);

		// model
		ImGui::Separator();
		ImGui::Text("load model");

		int modelIdx = 0;
		for (; modelIdx < modelNames.size(); modelIdx++) {
			if (renderResources.modelName == modelNames[modelIdx]) {
				break;
			}
		}
		if (ImGui::Combo("##load model", &modelIdx, modelNames.data(), (int)modelNames.size())) {
			renderResources.modelPtr = reloadModelFunc(modelNames[modelIdx]);
			renderResources.modelName = modelNames[modelIdx];
		}
	}

	void setReloadModelFunc(const std::function<std::shared_ptr<Model>(std::string modelName)> func)
	{
		reloadModelFunc = func;
	}

private:
	RenderResources& renderResources;
	int width, height;
	std::function<std::shared_ptr<Model>(std::string modelName)> reloadModelFunc;
	std::vector<const char*> modelNames = { "school_uniform", "guitar" };
};

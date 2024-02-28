#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>

#include "ModelWithShader.h"
#include "Model.h"
#include "Config.h"
#include "3rdParty/imgui/imgui.h"
#include "3rdParty/imgui/imgui_impl_glfw.h"
#include "3rdParty/imgui/imgui_impl_opengl3.h"

extern int width, height;

class ConfigPanel
{
public:
	explicit ConfigPanel(void* window, Config& conf) 
		: config(conf)
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
		ImGui::Text("triangles: %zu", config.primitiveCnt);

		// model
		ImGui::Separator();
		ImGui::Text("load model");

		int modelIdx = 0;
		for (; modelIdx < modelNames.size(); modelIdx++) {
			if (config.modelName == modelNames[modelIdx]) {
				break;
			}
		}
		if (ImGui::Combo("##load model", &modelIdx, modelNames.data(), (int)modelNames.size())) {
			reloadModelFunc(modelNames[modelIdx]);
		}

		ImGui::Separator();
		ImGui::Checkbox("load skybox", &(config.loadSkybox));

		if (config.loadSkybox)
		{
			int skyboxIdx = 0;
			for (; skyboxIdx < skyboxNames.size(); skyboxIdx++) {
				if (config.skyboxName == skyboxNames[skyboxIdx]) {
					break;
				}
			}
			if (ImGui::Combo("##skybox", &skyboxIdx, skyboxNames.data(), (int)skyboxNames.size())) {
				reloadSkyboxModelFunc(skyboxNames[skyboxIdx]);
			}
		}

		ImGui::Separator();
		ImGui::Checkbox("highlight model", &(config.highlightBoundary));
		if (config.highlightBoundary)
		{
			enableStencilFunc();
		}
		else
		{
			disableStencilFunc();
		}
	}

	void setReloadModelFunc(const std::function<void(std::string modelName)> func)
	{
		reloadModelFunc = func;
	}

	void setReloadSkyboxModelFunc(const std::function<void(std::string modelName)> func)
	{
		reloadSkyboxModelFunc = func;
	}

	void setEnableStencilFunc(const std::function<void()> func)
	{
		enableStencilFunc = func;
	}

	void setDisableStencilFunc(const std::function<void()> func)
	{
		disableStencilFunc = func;
	}
private:
	Config& config;
	std::function<void(std::string modelName)> reloadModelFunc;
	std::function<void(std::string modelName)> reloadSkyboxModelFunc;
	std::function<void()> enableStencilFunc;
	std::function<void()> disableStencilFunc;

	std::vector<const char*> modelNames = { "school_uniform", "guitar", "rodin", "dog"};
	std::vector<const char*> skyboxNames = { "sky", "skyhsky" };
};

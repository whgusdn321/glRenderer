#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ShaderGL.h"
#include "Camera.h"
#include "Trackball.h"

#include "ModelWithShader.h"
#include "ConfigPanel.h"

#include "Model.h"
#include "ModelLoader.h"
#include "RenderHelper.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void enableStencilHl();
void disableStencilHl();
void initConfigPanel(ConfigPanel&, Config&, ModelWithShader&, ModelWithShader&, ModelWithShader&);

//viewport
int width = 800;
int height = 600;
const int shadowWidth = 1024;
const int shadowHeight = 1024;

Camera camera(60.f, 0.01f, 100.f);
Trackball trackball(camera);

//mouse
bool firstMouse = true;
float lastMouseX = width / 2.f;
float lastMouseY = height / 2.f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// trackball
bool isTrackballOn = false;
bool isTranslateOn = false;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "GL_Renderer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // register a callback function on the window that gets called each time the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    RenderHelper renderHelper;

    ModelWithShader model(phongLightShdr, Object, "rodin");
    ModelWithShader floorModel(phongLightShdr, Floor, "floor");
    ModelWithShader hlModel(singleColorShdr, Object, "rodin");
    ModelWithShader skyboxModel(skyBoxShdr, Skybox, "sky");

    camera.setPosition(0.0f, 0.0f, 5.0f); // -1.5f, 3.f, 3.f
    camera.setLookAtTargetRotation(glm::vec3(0.0f, 0.f, 0.0f));

    Config config;
    ConfigPanel configPanel(window, config);
    initConfigPanel(configPanel, config, model, hlModel, skyboxModel);

    renderHelper.setupStaticUniforms(model.shaderGL, phongLightShdr);
    renderHelper.setupStaticUniforms(floorModel.shaderGL, phongLightShdr);
    renderHelper.setupStaticUniforms(hlModel.shaderGL, singleColorShdr);
    renderHelper.setupStaticUniforms(skyboxModel.shaderGL, skyBoxShdr);

    float beforeFrameX = lastMouseX;
    float beforeFrameY = lastMouseY;

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float curFrameX = lastMouseX;
        float curFrameY = lastMouseY;

        trackball.rotate(beforeFrameX, beforeFrameY, curFrameX, curFrameY);

        processInput(window);

        // rendering commands 
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        renderHelper.setupCameraUniform(model.shaderGL, phongLightShdr);
        renderHelper.drawObject(model.shaderGL, phongLightShdr, model.modelPtr);
        renderHelper.drawFloor(floorModel.shaderGL, phongLightShdr, floorModel.modelPtr, model.modelPtr->rootAABB);

        if (config.highlightBoundary) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            renderHelper.setupCameraUniform(hlModel.shaderGL, singleColorShdr);
            renderHelper.drawObject(hlModel.shaderGL, singleColorShdr, hlModel.modelPtr);
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
        }

        if (config.loadSkybox) {
            renderHelper.setupCameraUniform(skyboxModel.shaderGL, skyBoxShdr);
            renderHelper.drawSkybox(skyboxModel.shaderGL, skyBoxShdr, skyboxModel.modelPtr);
        }

        configPanel.draw();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents(); // callback methods
        beforeFrameX = curFrameX;
        beforeFrameY = curFrameY;
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int inWidth, int inHeight)
{
    width = inWidth;
    height = inHeight;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static float sensitivity = 0.1f; // change this value to your liking

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
        return;
    }

    // offset
    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (isTranslateOn)
    {
        // cameta translate logic here
        camera.rotateByPixels(yoffset, xoffset);
    }

    // update lastX, lastY for latest values
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        isTrackballOn = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        isTrackballOn = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        isTranslateOn = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        isTranslateOn = false;
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float scrollOffset = static_cast<float>(yoffset);
    // update camera FOV & frustum's 6 planes 
    if (!Math::equalsInTolerance(scrollOffset, 0.f))
    {
        camera.accumulateFOV( scrollOffset);
    }
}

void enableStencilHl()
{
   glEnable(GL_STENCIL_TEST);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
   glStencilFunc(GL_ALWAYS, 1, 0xFF);
   glStencilMask(0xFF);
}

void disableStencilHl()
{
   glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
   glStencilFunc(GL_ALWAYS, 1, 0x00);
   glStencilMask(0x00);
   glDisable(GL_STENCIL_TEST);
}

void initConfigPanel(ConfigPanel& configPanel, Config& config, ModelWithShader& model, ModelWithShader& hlModel, ModelWithShader& skyboxModel)
{
    configPanel.setReloadModelFunc([&](const std::string modelName) -> void {
        config.modelName = modelName;
        model.setModel(Object, modelName);
        hlModel.setModel(Object, modelName);
        config.primitiveCnt = model.modelPtr->primitiveCnt;
        });
    configPanel.setReloadSkyboxModelFunc([&](const std::string modelName) -> void {
        config.skyboxName = modelName;
        skyboxModel.setModel(Skybox, modelName);
        });
    configPanel.setEnableStencilFunc(enableStencilHl);
    configPanel.setDisableStencilFunc(disableStencilHl);
    config.primitiveCnt = model.modelPtr->primitiveCnt;
}

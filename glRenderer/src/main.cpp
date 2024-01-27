#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ShaderGL.h"
#include "Camera.h"
#include "Trackball.h"

#include "Model.h"
#include "ModelLoader.h"
#include "RenderHelper.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//viewport
int width = 800;
int height = 600;
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//mouse
bool firstMouse = true;
float lastMouseX = width / 2.f;
float lastMouseY = height / 2.f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// trackball
bool isTrackballOn = false;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 3.0f);

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

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    Camera camera;
    camera.setFOV(45.f);
    camera.setNear(0.01f);
    camera.setFar(100.f);
    camera.setPosition(0.0f, 0.0f, 8.0f);
    camera.setLookAtTargetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    
    Trackball trackball(camera);

    ModelLoader modelLoader;
    std::shared_ptr<Model> model = modelLoader.loadModel("school_uniform");

    RenderHelper renderHelper;

    auto shader = renderHelper.setupShaderGL(PhongLight);
    renderHelper.setupStaticUniforms(shader, PhongLight, camera);

    float beforeFrameX = lastMouseX;
    float beforeFrameY = lastMouseY;

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderHelper.setupVertexGL(model);
        renderHelper.setupTextureGL(model);
        renderHelper.setupDynamicUniforms(shader, model, camera, trackball);
        renderHelper.drawFrame(shader, model);
    
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

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

    // update lastX, lastY for latest values
    lastMouseX = xpos;
    lastMouseY = ypos;

    std::cout << "xoffset: " << xoffset << std::endl;
    std::cout << "yoffset: " << yoffset << std::endl;
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

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    std::cout << "scroll yoffset: " << yoffset << std::endl;
}

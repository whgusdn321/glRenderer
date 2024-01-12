#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Shader.h"
#include "MathHeaders.h"

#include "Model.h"
#include "Mesh.h"

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
Trackball trackball;
glm::mat4 trackballSpin = glm::mat4(1.f);

// lighting
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(1.2f, 1.0f, 3.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    // hide the cursor and lock it to the specified window.
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader guitarShader("./shaders/guitar.vs", "./shaders/guitar.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //Model guitarModel("./model/guitar/backpack.obj");
    Model guitarModel("./model/school_uniform/school_uniform_obj.obj");
    std::cout << "model loaded" << std::endl;
    Camera camera;
    camera.setFOV(45.f);
    camera.setNear(0.1f);
    camera.setFar(100.f);
    camera.setPosition(0.0f, 0.0f, 20.0f);
    camera.setLookAtTargetRotation(glm::vec3(0.0f, 10.0f, 0.0f));
    //camera.setLookAtTargetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
    // -----------------------------------------------------------------------------------------------------------
    glm::mat4 projection = camera.getPerspectiveMatrix();
    guitarShader.use();
    guitarShader.setFloat("material.shiniess", 32.0f);

    guitarShader.setMat4f("projection", projection);

    guitarShader.setVec3f("viewPos", camera.getPosition());
    // lightingShader.setVec3f("lightPos", lightPos);


    guitarShader.setVec3f("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    guitarShader.setVec3f("directionalLight.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
    guitarShader.setVec3f("directionalLight.specular", 1.0f, 1.0f, 1.0f);
    guitarShader.setVec3f("directionalLight.direction", -0.2f, -1.0f, -0.3f);

    /*
    glm::vec3 pointLightPositions[4] = {
       glm::vec3(0.f, 5.f, 3.f),
        glm::vec3(0.f,-5.f, 0.f),
        glm::vec3(5.f, 0.f, 0.f),
        glm::vec3(-5.f,0.f, 0.f),
    };
    glm::vec3 pointLightRotateAxis[4] = {
        glm::normalize(glm::vec3(1.f, 0.f, 0.f)),
        glm::normalize(glm::vec3(-1.f, 0.f, 0.f)),
        glm::normalize(glm::vec3(0.f, 1.f, 0.f)),
        glm::normalize(glm::vec3(0.f, -1.f, 0.f)),
    };

    
    for (int pl = 0; pl < 4; ++pl) {
        // lightingShader.setVec3f("pointLights[" + std::to_string(pl) + "].position", pointLightPositions[pl]);
        lightingShader.setVec3f("pointLights[" + std::to_string(pl) + "].ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3f("pointLights[" + std::to_string(pl) + "].diffuse", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3f("pointLights[" + std::to_string(pl) + "].specular", 1.f, 1.f, 1.f);
        lightingShader.setFloat("pointLights[" + std::to_string(pl) + "].constant", 1.f);
        lightingShader.setFloat("pointLights[" + std::to_string(pl) + "].linear", 0.07f);
        lightingShader.setFloat("pointLights[" + std::to_string(pl) + "].quadratic", 0.017f);
    }
    lightingShader.setVec3f("flashLight.position", camera.getPosition());
    lightingShader.setVec3f("flashLight.direction", camera.getFrontAxis());
    lightingShader.setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("flashLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    lightingShader.setVec3f("flashLight.ambient", 0.1f, 0.1f, 0.1f);
    lightingShader.setVec3f("flashLight.flash", 0.3f, 0.3f, 0.3f);

    glm::radians(12.5f);

    lightCubeShader.use();
    lightCubeShader.setMat4f("projection", projection);
    */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // rendering commands here
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();

        /*
        // draw the lamp object
        lightCubeShader.use();
        glBindVertexArray(lightCubeVAO);
        lightCubeShader.setMat4f("view", view);
        for (unsigned int i = 0; i < 4; ++i) {
            // Quaternion qt(pointLightRotateAxis[i], Math::fMod(currentFrame, 360.f)*30);
            Quaternion qt(pointLightRotateAxis[i], currentFrame * 100);
            glm::vec3 worldPos = qt * pointLightPositions[i];
            Transform tf;
            tf.setPosition(worldPos);
            tf.setScale(glm::vec3(0.2f));
            glm::mat4 model = tf.getModelingMatrix();

            lightCubeShader.setMat4f("model", model);
            lightingShader.use();
            lightingShader.setVec3f("pointLights[" + std::to_string(i) + "].position", worldPos.x, worldPos.y, worldPos.z);

            lightCubeShader.use();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        */

        guitarShader.use();
        guitarShader.setMat4f("view", view);
        glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(0.01, 0.01, 0.01));
        model *= trackballSpin;
        guitarShader.setMat4f("model", model);
        guitarModel.draw(guitarShader);

       // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents(); // callback methods
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    //glDeleteVertexArrays(1, &lightCubeVAO);
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteBuffers(1, &VBO);
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

    if (isTrackballOn)
    {
        trackball.startX = lastMouseX;
        trackball.startY = lastMouseY;
        trackball.endX = xpos;
        trackball.endY = ypos;
        //std::cout << "x trackball: " << trackball.startX << std::endl;
        //std::cout << "y trackball: " << trackball.startY << std::endl;
        //std::cout << "x trackball: " << trackball.endX << std::endl;
        //std::cout << "y trackball: " << trackball.endY << std::endl << std::endl;
        // trackball algotithm
        trackball.rotate();
        trackballSpin = trackball.getRotationMatrix();
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

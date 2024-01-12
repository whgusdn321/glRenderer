#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "MathHeaders.h"

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
    Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

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
    Shader lightingShader("1.colors.vs", "1.colors.fs");
    Shader lightCubeShader("1.light_cube.vs", "1.light_cube.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture diffuse
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("container2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        const char* failureReason = stbi_failure_reason();
        std::cout << "Failed to load image: " << failureReason << std::endl;
    }
    stbi_image_free(data);

    // Texture specular 
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("container2_specular.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        const char* failureReason = stbi_failure_reason();
        std::cout << "Failed to load image: " << failureReason << std::endl;
    }
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    Camera camera;
    camera.setFOV(45.f);
    camera.setNear(0.1f);
    camera.setFar(100.f);
    camera.setPosition(0.0f, 0.0f, 20.0f);
    camera.setLookAtTargetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
    // -----------------------------------------------------------------------------------------------------------
    glm::mat4 projection = camera.getPerspectiveMatrix();
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setFloat("material.shiniess", 32.0f);

    lightingShader.setMat4f("projection", projection);

    lightingShader.setVec3f("viewPos", camera.getPosition());
    // lightingShader.setVec3f("lightPos", lightPos);


    lightingShader.setVec3f("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.setVec3f("directionalLight.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
    lightingShader.setVec3f("directionalLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3f("directionalLight.direction", -0.2f, -1.0f, -0.3f);

    glm::vec3 pointLightPositions[4] = {
        // glm::vec3(0.7f,  0.2f,  2.0f),
        // glm::vec3(2.3f, -3.3f, -4.0f),
        // glm::vec3(-4.0f,  2.0f, -12.0f),
        // glm::vec3(0.0f,  0.0f, -3.0f)
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

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
       
       
        // draw the lamp object
        lightCubeShader.use();
        glBindVertexArray(lightCubeVAO);
        lightCubeShader.setMat4f("view", view);
        for (unsigned int i = 0; i < 4; ++i) {
            // Quaternion qt(pointLightRotateAxis[i], Math::fMod(currentFrame, 360.f)*30);
            Quaternion qt(pointLightRotateAxis[i], currentFrame*100);
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

        lightingShader.use();
        lightingShader.setMat4f("view", view);
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4f("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents(); // callback methods
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

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

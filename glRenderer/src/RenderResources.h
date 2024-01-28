#pragma once

#include <string>
#include <memory>

#include "Model.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Trackball.h"
#include "ShaderGL.h"
#include "RenderHelper.h"

struct RenderResources 
{
    RenderResources(ModelLoader& ml, RenderHelper& rh) 
        : modelName("school_uniform"), trackball(camera),
        modelLoader(ml), renderHelper(rh)
    {
        modelPtr = modelLoader.loadModel(modelName);
        shaderGL = renderHelper.setupShaderGL(PhongLight);
        camera.setFOV(60.f);
        camera.setNear(0.01f);
        camera.setFar(100.f);
        camera.setPosition(0.0f, 0.0f, 5.0f);
        // camera.setPosition(-1.5f, 3.f, 3.f);
        camera.setLookAtTargetRotation(glm::vec3(0.0f, 0.f, 0.0f));
    }

    
    // model
    std::shared_ptr<Model> modelPtr;
    std::string modelName;

    //shader
    std::shared_ptr<ShaderGL> shaderGL;

    // camera
    Camera camera;
    Trackball trackball;

    ModelLoader& modelLoader;
    RenderHelper& renderHelper;
};

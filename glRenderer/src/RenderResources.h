#pragma once

#include <string>
#include <memory>

#include "Model.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Trackball.h"
#include "ShaderGL.h"
#include "RenderHelper.h"
#include "Frustum.h"

struct RenderResources 
{
    RenderResources(ModelLoader& ml, RenderHelper& rh) 
        : modelName("rodin"),
        modelLoader(ml), renderHelper(rh),
        frustumCulling(true)
    {
        modelPtr = modelLoader.loadModel(modelName);
        shaderGL = renderHelper.setupShaderGL(PhongLight);
    }

    
    // model
    std::shared_ptr<Model> modelPtr;
    std::string modelName;

    //shader
    std::shared_ptr<ShaderGL> shaderGL;

    ModelLoader& modelLoader;
    RenderHelper& renderHelper;

    bool frustumCulling;
};

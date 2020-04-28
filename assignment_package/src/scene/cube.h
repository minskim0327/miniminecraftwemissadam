#pragma once

#include "src/drawable.h"
#include "src/glm_includes.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Cube : public Drawable
{
public:
    Cube(OpenGLContext* context) : Drawable(context){}
    virtual ~Cube(){}
    void create() override;
};

#pragma once

#include "CompositeSurfaceElement.hpp"


namespace cagd {

class CompositeSurfaceProvider
{

public:
    CompositeSurfaceProvider(CompositeSurfaceElement &element)
        : _element(element)
    {}

    const ShaderProgram &getShader() const;
    Material &           getMaterial();
    GLdouble             getWireframeRedComponent() const;
    GLdouble             getWireframeGreenComponent() const;
    GLdouble             getWireframeBlueComponent() const;


    void setShader(std::shared_ptr<ShaderProgram> shader);
    void setMaterial(const Material &material);
    void setWireframeRedComponent(GLfloat red);
    void setWireframeGreenComponent(GLfloat green);
    void setWireframeBlueComponent(GLfloat blue);

private:
    CompositeSurfaceElement &_element;
};



} // namespace cagd

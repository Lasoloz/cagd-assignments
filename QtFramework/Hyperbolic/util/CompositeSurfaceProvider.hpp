#pragma once

#include "CompositeSurfaceElement.hpp"


namespace cagd {

class CompositeSurfaceProvider
{

public:
    CompositeSurfaceProvider(CompositeSurfaceElement &          element,
                             CompositeSurfaceElement::SurfaceId id,
                             GLuint selectedPoint = 0)
        : _element(element)
        , _selected_point(selectedPoint)
        , _id(id)
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

    void getSelectedPoint(DCoordinate3 &point);
    void setSelectedPoint(const DCoordinate3 &point);
    void getPoint(GLuint row, GLuint column, DCoordinate3 &point);
    void setPoint(GLuint row, GLuint column, const DCoordinate3 &point);

    void joinWithProvider(CompositeSurfaceProvider &other);

    CompositeSurfaceElement::SurfaceId getId() const;

    CompositeSurfaceElement::Direction getDirection() const;

private:
    CompositeSurfaceElement &          _element;
    GLuint                             _selected_point;
    CompositeSurfaceElement::SurfaceId _id;
};



} // namespace cagd

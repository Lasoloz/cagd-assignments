#include "CompositeSurfaceProvider.hpp"

namespace cagd {


const ShaderProgram &CompositeSurfaceProvider::getShader() const
{
    return *_element._shader;
}

Material &CompositeSurfaceProvider::getMaterial() { return _element._material; }

GLdouble CompositeSurfaceProvider::getWireframeRedComponent() const
{
    return _element._wireframe_red_component;
}

GLdouble CompositeSurfaceProvider::getWireframeGreenComponent() const
{
    return _element._wireframe_green_component;
}

GLdouble CompositeSurfaceProvider::getWireframeBlueComponent() const
{
    return _element._wireframe_blue_component;
}

void CompositeSurfaceProvider::setMaterial(const Material &material)
{
    _element._material = material;
}


void CompositeSurfaceProvider::setShader(std::shared_ptr<ShaderProgram> shader)
{
    _element._shader = shader;
}

void CompositeSurfaceProvider::setWireframeRedComponent(GLfloat red)
{
    _element._wireframe_red_component = red;
}

void CompositeSurfaceProvider::setWireframeGreenComponent(GLfloat green)
{
    _element._wireframe_green_component = green;
}

void CompositeSurfaceProvider::setWireframeBlueComponent(GLfloat blue)
{
    _element._wireframe_blue_component = blue;
}

void CompositeSurfaceProvider::getSelectedPoint(DCoordinate3 &point)
{
    _element._own_surface_ptr->GetData(_selected_point / 4, _selected_point % 4,
                                       point);
}
void CompositeSurfaceProvider::setSelectedPoint(const DCoordinate3 &point)
{
    _element._own_surface_ptr->SetData(_selected_point / 4, _selected_point % 4,
                                       point);
    _element._update_needed = true;
}

void CompositeSurfaceProvider::getPoint(GLuint row, GLuint column,
                                        DCoordinate3 &point)
{
    _element._own_surface_ptr->GetData(row, column, point);
}

void CompositeSurfaceProvider::setPoint(GLuint row, GLuint column,
                                        const DCoordinate3 &point)
{
    _element._own_surface_ptr->SetData(row, column, point);
}

} // namespace cagd

#include "CompositeSurfaceProvider.hpp"

namespace cagd {


const ShaderProgram & CompositeSurfaceProvider::getShader() const {
    return *_element._shader;
}

Material & CompositeSurfaceProvider::getMaterial() {
    return _element._material;
}

GLdouble CompositeSurfaceProvider::getWireframeRedComponent() const {
    return _element._wireframe_red_component;
}

GLdouble CompositeSurfaceProvider::getWireframeGreenComponent() const {
    return _element._wireframe_green_component;
}

GLdouble CompositeSurfaceProvider::getWireframeBlueComponent() const {
    return _element._wireframe_blue_component;
}

void CompositeSurfaceProvider::setMaterial(const Material &material) {
    _element._material = material;
}

void CompositeSurfaceProvider::setWireframeRedComponent(GLdouble red) {
    _element._wireframe_red_component = red;
}

void CompositeSurfaceProvider::setWireframeGreenComponent(GLdouble green) {
    _element._wireframe_green_component = green;
}

void CompositeSurfaceProvider::setWireframeBlueComponent(GLdouble blue) {
    _element._wireframe_blue_component = blue;
}

}

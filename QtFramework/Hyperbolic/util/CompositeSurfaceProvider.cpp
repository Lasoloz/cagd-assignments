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

    // Ugly rejoin:
    _element.forceConditions();
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


CompositeSurfaceElement::SurfaceId CompositeSurfaceProvider::getId() const
{
    return _id;
}


CompositeSurfaceElement::Direction
CompositeSurfaceProvider::getDirection() const
{
    switch (_selected_point) {
    case 3:
        return CompositeSurfaceElement::NORTH_WEST;
    case 7:
    case 11:
        return CompositeSurfaceElement::NORTH;
    case 15:
        return CompositeSurfaceElement::NORTH_EAST;
    case 13:
    case 14:
        return CompositeSurfaceElement::EAST;
    case 12:
        return CompositeSurfaceElement::SOUTH_EAST;
    case 4:
    case 8:
        return CompositeSurfaceElement::SOUTH;
    case 0:
        return CompositeSurfaceElement::SOUTH_WEST;
    case 1:
    case 2:
        return CompositeSurfaceElement::WEST;
    default:
        throw Exception("User must select outer point!");
    }
}


// std::vector<CompositeSurfaceElement::Direction>
// CompositeSurfaceProvider::getDirections() const
//{
//    //    std::vector<CompositeSurfaceElement::Direction> directions;
//    switch (_selected_point) {
//    case 0:
//    case 5:
//        return {CompositeSurfaceElement::SOUTH_WEST,
//                CompositeSurfaceElement::WEST,
//                CompositeSurfaceElement::SOUTH};
//    case 1:
//    case 4:
//        return {CompositeSurfaceElement::WEST,
//        CompositeSurfaceElement::SOUTH};
//    case 2:
//    case 7:
//        return {CompositeSurfaceElement::WEST,
//        CompositeSurfaceElement::NORTH};
//    case 3:
//    case 6:
//        return {CompositeSurfaceElement::WEST,
//                CompositeSurfaceElement::NORTH_WEST,
//                CompositeSurfaceElement::NORTH};
//    case 8:
//    case 13:
//        return {CompositeSurfaceElement::SOUTH,
//        CompositeSurfaceElement::EAST};
//    case 9:
//    case 12:
//        return {CompositeSurfaceElement::SOUTH, CompositeSurfaceElement::EAST,
//                CompositeSurfaceElement::SOUTH_EAST};
//    case 10:
//    case 15:
//        return {CompositeSurfaceElement::NORTH,
//                CompositeSurfaceElement::NORTH_EAST,
//                CompositeSurfaceElement::EAST};
//    case 11:
//    case 14:
//        return {CompositeSurfaceElement::NORTH,
//        CompositeSurfaceElement::EAST};
//    default:
//        return {};
//    }
//}

} // namespace cagd

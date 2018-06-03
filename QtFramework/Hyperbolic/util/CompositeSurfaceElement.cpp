#include "CompositeSurfaceElement.hpp"


namespace cagd {

double CompositeSurfaceElement::default_tension = 1.0;

CompositeSurfaceElement::CompositeSurfaceElement()
    : _use_count(0)
{
    _own_surface_ptr.reset(new SecondOrderHyperbolicPatch(
        CompositeSurfaceElement::default_tension));

    for (auto &neighbor : _neighbors) {
        neighbor = nullptr;
    }
}

CompositeSurfaceElement::CompositeSurfaceElement(
    SecondOrderHyperbolicPatch *patch_taken)
    : _use_count(0)
    , _own_surface_ptr(patch_taken)
{
    for (auto &neighbor : _neighbors) {
        neighbor = nullptr;
    }
}

CompositeSurfaceElement::~CompositeSurfaceElement()
{
    for (int i = 0; i < DIR_COUNT; ++i) {
        splitFrom(Direction(i));
    }
}

CompositeSurfaceElement::CompositeSurfaceElement(
    CompositeSurfaceElement &&other)
    : _use_count(0)
    , _surf_image()
{
    for (auto &neighbor : _neighbors) {
        neighbor = nullptr;
    }
    swap(*this, other);
}

CompositeSurfaceElement &CompositeSurfaceElement::
                         operator=(CompositeSurfaceElement &&other)
{
    swap(*this, other);

    return *this;
}



// Private methods:
// ================
// NOTE: Neighbor in direction must not be null!
//    0,0   <= NORTH ->
//       + -- + .. + -- +
//     ^ |    |    |    | ^
//     | + -- + .. + -- + ||
//  WEST ..   ..   ..   ..EAST
//    || + -- + .. + -- + |
//     v |    |    |    | v
//       + -- + -- + -- +
//          <- SOUTH =>
void CompositeSurfaceElement::forceJoinCondition(Direction direction)
{
    auto &thisPatchPtr  = _own_surface_ptr;
    auto &otherPatchPtr = _neighbors[direction]->_own_surface_ptr;

    Direction otherDirection = _neighbor_back_references[direction];


    int  thisStartX, thisStartY;
    int  thisDeltaX = 0, thisDeltaY = 0;
    int  thisInnerDeltaX = 0, thisInnerDeltaY = 0;
    bool thisOnePoint = false;

    // TODO: Remove the lot of code duplication:
    // =========================================
    switch (direction) {
    case SOUTH_WEST:
        thisOnePoint    = true;
        thisInnerDeltaX = 1;
    case SOUTH:
        thisStartX      = 0;
        thisStartY      = 0;
        thisDeltaX      = 1;
        thisInnerDeltaY = 1;
        break;
    case SOUTH_EAST:
        thisOnePoint    = true;
        thisInnerDeltaY = 1;
    case EAST:
        thisStartX      = 3;
        thisStartY      = 0;
        thisInnerDeltaX = -1;
        thisDeltaY      = 1;
        break;
    case NORTH_EAST:
        thisOnePoint    = true;
        thisInnerDeltaX = -1;
    case NORTH:
        thisStartX      = 3;
        thisStartY      = 3;
        thisDeltaX      = -1;
        thisInnerDeltaY = -1;
        break;
    case NORTH_WEST:
        thisOnePoint    = true;
        thisInnerDeltaY = -1;
    case WEST:
        thisStartX      = 0;
        thisStartY      = 3;
        thisInnerDeltaX = 1;
        thisDeltaY      = -1;
        break;
    default:
        throw Exception("Invalid direction");
    }


    int  otherStartX, otherStartY;
    int  otherDeltaX = 0, otherDeltaY = 0;
    int  otherInnerDeltaX = 0, otherInnerDeltaY = 0;
    bool otherOnePoint = false;

    switch (otherDirection) {
    case SOUTH_EAST:
        otherOnePoint    = true;
        otherInnerDeltaX = -1;
    case SOUTH:
        otherStartX      = 3;
        otherStartY      = 0;
        otherDeltaX      = -1;
        otherInnerDeltaY = 1;
        break;
    case NORTH_EAST:
        otherOnePoint    = true;
        otherInnerDeltaY = -1;
    case EAST:
        otherStartX      = 3;
        otherStartY      = 3;
        otherInnerDeltaX = -1;
        otherDeltaY      = -1;
        break;
    case NORTH_WEST:
        otherOnePoint    = true;
        otherInnerDeltaX = 1;
    case NORTH:
        otherStartX      = 0;
        otherStartY      = 3;
        otherDeltaX      = 1;
        otherInnerDeltaY = -1;
        break;
    case SOUTH_WEST:
        otherOnePoint = true;
        otherDeltaY   = 1;
    case WEST:
        otherStartX      = 0;
        otherStartY      = 0;
        otherInnerDeltaX = 1;
        otherDeltaY      = 1;
        break;
    default:
        throw Exception("Invalid direction");
    }


    if (thisOnePoint != otherOnePoint) {
        throw Exception("Cannot join edge with corner");
    }

    if (thisOnePoint) {
        GLdouble x, y, z;
        thisPatchPtr->GetData(thisStartX, thisStartY, x, y, z);
        otherPatchPtr->SetData(otherStartX, otherStartY, x, y, z);

        GLdouble x1, y1, z1;
        thisPatchPtr->GetData(thisStartX + thisInnerDeltaX,
                              thisStartY + thisInnerDeltaY, x1, y1, z1);

        x1 = 2 * x - x1;
        y1 = 2 * y - y1;
        z1 = 2 * z - z1;
        otherPatchPtr->SetData(otherStartX + otherInnerDeltaX,
                               otherStartY + otherInnerDeltaY, x1, y1, z1);
    } else {
        for (int i = 0; i < 4; ++i) {
            GLdouble x, y, z;
            thisPatchPtr->GetData(thisStartX, thisStartY, x, y, z);
            otherPatchPtr->SetData(otherStartX, otherStartY, x, y, z);

            GLdouble x1, y1, z1;
            thisPatchPtr->GetData(thisStartX + thisInnerDeltaX,
                                  thisStartY + thisInnerDeltaY, x1, y1, z1);

            x1 = 2 * x - x1;
            y1 = 2 * y - y1;
            z1 = 2 * z - z1;
            otherPatchPtr->SetData(otherStartX + otherInnerDeltaX,
                                   otherStartY + otherInnerDeltaY, x1, y1, z1);

            thisStartX += thisDeltaX;
            thisStartY += thisDeltaY;
            otherStartX += otherDeltaX;
            otherStartY += otherDeltaY;
        }
    }
}



// Public methods:
// ===============
void CompositeSurfaceElement::joinWith(Direction                direction,
                                       Direction                otherDirection,
                                       CompositeSurfaceElement *neighbor)
{
    splitFrom(direction);

    _neighbors[direction] = neighbor;
    ++_use_count;

    neighbor->splitFrom(otherDirection);
    neighbor->_neighbors[otherDirection]                = this;
    neighbor->_neighbor_back_references[otherDirection] = direction;
    neighbor->_use_count += 1;

    _neighbor_back_references[direction] = otherDirection;

    forceJoinCondition(direction);
}

void CompositeSurfaceElement::splitFrom(Direction direction)
{
    if (!_neighbors[direction]) {
        return;
    }

    Direction otherDirection = _neighbor_back_references[direction];


    _neighbors[direction]->_neighbors[otherDirection] = nullptr;
    _neighbors[direction]->_use_count -= 1;

    _neighbors[direction] = nullptr;
    --_use_count;
}


bool CompositeSurfaceElement::updateVBOs(GLuint divU, GLuint divV)
{
    _surf_image.reset(_own_surface_ptr->GenerateImage(divU, divV));
    if (!_surf_image) {
        return false;
    }

    if (!_surf_image->UpdateVertexBufferObjects()) {
        _surf_image.reset();
        return false;
    }

    return true;
}

void CompositeSurfaceElement::renderMesh() const
{
    // TODO: support render modes, and maybe support different materials
    // (Decoupling material and patch specific data from main logic (GLWIDGET))
    _surf_image->Render();
}


SecondOrderHyperbolicPatch *CompositeSurfaceElement::releaseOwnSurface()
{
    return _own_surface_ptr.release();
}



GLdouble CompositeSurfaceElement::getAlphaTension() const
{
    return _own_surface_ptr->getAlphaTension();
}



// Friend functions:
// =================
void swap(CompositeSurfaceElement &first, CompositeSurfaceElement &second)
{
    // Enable ADL. Look at:
    // https://stackoverflow.com/a/3279550/3573460
    using std::swap;

    swap(first._use_count, second._use_count);
    swap(first._neighbors, second._neighbors);
    swap(first._neighbor_back_references, second._neighbor_back_references);
    swap(first._own_surface_ptr, second._own_surface_ptr);
}

} // namespace cagd

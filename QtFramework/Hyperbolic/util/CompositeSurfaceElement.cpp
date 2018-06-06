#include "CompositeSurfaceElement.hpp"


namespace cagd {

double CompositeSurfaceElement::default_tension = 1.0;

CompositeSurfaceElement::CompositeSurfaceElement()
    : _use_count(0)
    , _update_needed(true)
    , _wireframe_red_component(1.f)
    , _wireframe_green_component(1.f)
    , _wireframe_blue_component(1.f)
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
    , _update_needed(true)
    , _wireframe_red_component(1.f)
    , _wireframe_green_component(1.f)
    , _wireframe_blue_component(1.f)
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
void CompositeSurfaceElement::forceBorderCondition(Direction direction,
                                                   evaluator eval)
{
    auto &patchPtrThis  = _own_surface_ptr;
    auto &patchPtrOther = _neighbors[direction]->_own_surface_ptr;

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
        eval(patchPtrThis, thisStartX, thisStartY, thisStartX + thisInnerDeltaX,
             thisStartY + thisInnerDeltaY, patchPtrOther, otherStartX,
             otherStartY, otherStartX + otherDeltaX, otherStartY + otherDeltaY);
    } else {
        for (int i = 0; i < 4; ++i) {
            eval(patchPtrThis, thisStartX, thisStartY,
                 thisStartX + thisInnerDeltaX, thisStartY + thisInnerDeltaY,
                 patchPtrOther, otherStartX, otherStartY,
                 otherStartX + otherInnerDeltaX,
                 otherStartY + otherInnerDeltaY);

            // Iterate:
            thisStartX += thisDeltaX;
            thisStartY += thisDeltaY;
            otherStartX += otherDeltaX;
            otherStartY += otherDeltaY;
        }
    }

    _update_needed = true;
}



// Public methods:
// ===============
// Join methods:
// =============
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

    forceBorderCondition(
        direction, [](std::unique_ptr<SecondOrderHyperbolicPatch> &firstPatch,
                      GLuint x1t, GLuint y1t, GLuint x2t, GLuint y2t,
                      std::unique_ptr<SecondOrderHyperbolicPatch> &secondPatch,
                      GLuint x1o, GLuint y1o, GLuint x2o, GLuint y2o) {
            DCoordinate3 pThis;
            // Border
            firstPatch->GetData(x1t, y1t, pThis);
            secondPatch->SetData(x1o, y1o, pThis);

            // Inner:
            DCoordinate3 pOther;
            firstPatch->GetData(x2t, y2t, pOther);
            pOther = 2 * pThis - pOther;

            secondPatch->SetData(x2o, y2o, pOther);
        });
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


// Merge methods:
// ==============
void CompositeSurfaceElement::mergeWith(Direction                direction,
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

    forceBorderCondition(
        direction, [](std::unique_ptr<SecondOrderHyperbolicPatch> &firstPatch,
                      GLuint x1t, GLuint y1t, GLuint x2t, GLuint y2t,
                      std::unique_ptr<SecondOrderHyperbolicPatch> &secondPatch,
                      GLuint x1o, GLuint y1o, GLuint x2o, GLuint y2o) {
            DCoordinate3 pThis, pOther;
            // Border
            firstPatch->GetData(x1t, y1t, pThis);
            secondPatch->GetData(x1o, y1o, pOther);

            DCoordinate3 middle = 0.5 * (pThis + pOther);
            firstPatch->SetData(x1t, y1t, middle);
            secondPatch->SetData(x1o, y1o, middle);

            // Inner:
            DCoordinate3 pThisSym  = 2 * middle - pThis;
            DCoordinate3 pOtherSym = 2 * middle - pOther;

            firstPatch->SetData(x2t, y2t, 0.5 * (pThis + pOtherSym));
            secondPatch->SetData(x2o, y2o, 0.5 * (pOther + pThisSym));

            // Inner:
            //            firstPatch->GetData(x2t, y2t, pThis);
            //            secondPatch->GetData(x2o, y2o, pOther);

            //            DCoordinate3 middle = 0.5 * (pThis + pOther);
            //            firstPatch->SetData(x1t, y1t, middle);
            //            secondPatch->SetData(x1o, y1o, middle);
        });
}



// Test methods:
// =============
bool CompositeSurfaceElement::isNeighbor(const CompositeSurfaceElement &other,
                                         Direction directionThis) const
{
    return _neighbors[directionThis] == &other;
}


// Render methods:
// ===============
bool CompositeSurfaceElement::updateVBOs(GLuint divU, GLuint divV)
{
    if (_update_needed) {
        _surf_image.reset(_own_surface_ptr->GenerateImage(divU, divV));
        if (!_surf_image) {
            return false;
        }

        if (!_surf_image->UpdateVertexBufferObjects()) {
            _surf_image.reset();
            return false;
        }

        if (!_own_surface_ptr->UpdateVertexBufferObjectsOfData()) {
            return false;
        }

        _update_needed = false;
    }

    return true;
}

void CompositeSurfaceElement::renderMesh(GLenum flag)
{
    _material.Apply();
    if (_shader) {
        _shader->Enable();
        _surf_image->Render(flag);
        _shader->Disable();
    } else {
        _surf_image->Render(flag);
    }
}

void CompositeSurfaceElement::renderWireframe(GLenum flag) const
{
    glColor3f(_wireframe_red_component, _wireframe_green_component,
              _wireframe_blue_component);
    _own_surface_ptr->RenderData(flag);
}

void CompositeSurfaceElement::renderControlPoints(
    std::shared_ptr<TriangulatedMesh3> &controlPointMeshPtr) const
{
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            DCoordinate3 cp;
            _own_surface_ptr->GetData(row, col, cp);

            glPushMatrix();
            glTranslated(cp[0], cp[1], cp[2]);
            glScaled(0.2, 0.2, 0.2);

            MatFBBrass.Apply();
            controlPointMeshPtr->Render();

            glPopMatrix();
        }
    }
}

void CompositeSurfaceElement::renderControlPoints(
    std::shared_ptr<TriangulatedMesh3> &controlPointMeshPtr,
    GLuint                              startNumber) const
{
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            DCoordinate3 cp;
            _own_surface_ptr->GetData(row, col, cp);

            glLoadName(startNumber + 4 * row + col);

            glPushMatrix();
            glTranslated(cp[0], cp[1], cp[2]);
            glScaled(0.2, 0.2, 0.2);

            MatFBBrass.Apply();
            controlPointMeshPtr->Render();

            glPopMatrix();
        }
    }
}


// Utility methods:
// ================
SecondOrderHyperbolicPatch *CompositeSurfaceElement::releaseOwnSurface()
{
    for (int i = 0; i < DIR_COUNT; ++i) {
        splitFrom(Direction(i));
    }
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
    swap(first._shader, second._shader);
    swap(first._material, second._material);
    swap(first._wireframe_red_component, second._wireframe_red_component);
    swap(first._wireframe_green_component, second._wireframe_green_component);
    swap(first._wireframe_blue_component, second._wireframe_blue_component);
}
} // namespace cagd

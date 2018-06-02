#include "CompositeSurfaceElement.hpp"


namespace cagd {

double CompositeSurfaceElement::default_tension = 1.0;

CompositeSurfaceElement::CompositeSurfaceElement()
    : _use_count(0)
{
    _own_surface_ptr.reset(new SecondOrderHyperbolicPatch(
        CompositeSurfaceElement::default_tension));
}

CompositeSurfaceElement::CompositeSurfaceElement(
    SecondOrderHyperbolicPatch *patch_taken)
    : _use_count(0)
    , _own_surface_ptr(patch_taken)
{}

CompositeSurfaceElement::~CompositeSurfaceElement()
{
    for (int i = 0; i < DIR_COUNT; ++i) {
        splitFrom(Direction(i));
    }
}

CompositeSurfaceElement::CompositeSurfaceElement(
    const CompositeSurfaceElement &other)
    : _use_count(0)
    , _neighbors()
    , _neighbor_back_references()
    , _own_surface_ptr(
          new SecondOrderHyperbolicPatch(*other._own_surface_ptr.get()))
    , _surf_image()
{}

CompositeSurfaceElement &CompositeSurfaceElement::
                         operator=(CompositeSurfaceElement &other)
{
    swap(*this, other);

    return *this;
}

CompositeSurfaceElement::CompositeSurfaceElement(
    CompositeSurfaceElement &&other)
    : _use_count(0)
    , _surf_image()
{
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
void CompositeSurfaceElement::forceJoinCondition(Direction direction)
{
    auto &thisPatchPtr  = _own_surface_ptr;
    auto &otherPatchPtr = _neighbors[direction]->_own_surface_ptr;

    // TODO: Maybe create a class for binding?
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
    neighbor->_neighbors[otherDirection] = this;
    neighbor->_use_count += 1;

    _neighbor_back_references[otherDirection];
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


bool CompositeSurfaceElement::updateVBOs()
{
    _surf_image.reset(_own_surface_ptr->GenerateImage(4, 4));
    if (!_surf_image->UpdateVertexBufferObjects()) {
        _surf_image.reset();
        return false;
    }

    return true;
}

void CompositeSurfaceElement::renderMesh()
{
    // TODO: support render modes!
    _surf_image->Render();
}


SecondOrderHyperbolicPatch *CompositeSurfaceElement::releaseOwnSurface()
{
    return _own_surface_ptr.release();
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

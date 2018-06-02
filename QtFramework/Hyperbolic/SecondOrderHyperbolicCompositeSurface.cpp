#include "SecondOrderHyperbolicCompositeSurface.h"

namespace cagd {

SecondOrderHyperbolicCompositeSurface::SecondOrderHyperbolicCompositeSurface()
    : _current_id(0)
{}

bool SecondOrderHyperbolicCompositeSurface::join(
    SurfaceId olderSurfaceId, SecondOrderHyperbolicPatch *patch_taken,
    CompositeSurfaceElement::Direction olderDirection,
    CompositeSurfaceElement::Direction newerDirection)
{
    SurfaceId id  = _current_id++;
    auto inserted = _patches.emplace(id, CompositeSurfaceElement(patch_taken));

    if (!inserted.second) {
        return false;
    }

    auto &insertedPatch = (*inserted.first).second;
    _patches.at(olderSurfaceId)
        .joinWith(olderDirection, newerDirection, &insertedPatch);

    return true;
}

void SecondOrderHyperbolicCompositeSurface::updateVBOs()
{
    for (auto patch : _patches) {
        patch.second.updateVBOs();
    }
}

void SecondOrderHyperbolicCompositeSurface::renderSurface()
// TODO: Add mode support for wireframes!
{
    for (auto patch : _patches) {
        patch.second.renderMesh();
    }
}

} // namespace cagd

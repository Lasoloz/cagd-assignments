#include "SecondOrderHyperbolicCompositeSurface.h"

namespace cagd {

SecondOrderHyperbolicCompositeSurface::SecondOrderHyperbolicCompositeSurface()
    : _current_id(0)
{}


void SecondOrderHyperbolicCompositeSurface::add(
    SecondOrderHyperbolicPatch *patch_taken)
{
    SurfaceId id = _current_id++;
    _patches.emplace(id, std::move(CompositeSurfaceElement(patch_taken)));
}

bool SecondOrderHyperbolicCompositeSurface::join(
    SurfaceId olderSurfaceId, SecondOrderHyperbolicPatch *patch_taken,
    CompositeSurfaceElement::Direction olderDirection,
    CompositeSurfaceElement::Direction newerDirection)
{
    SurfaceId id = _current_id++;
    auto      inserted =
        _patches.emplace(id, std::move(CompositeSurfaceElement(patch_taken)));

    if (!inserted.second) {
        return false;
    }

    auto &insertedPatch = (*inserted.first).second;
    _patches.at(olderSurfaceId)
        .joinWith(olderDirection, newerDirection, &insertedPatch);

    return true;
}

bool SecondOrderHyperbolicCompositeSurface::updateVBOs(GLuint minDivU,
                                                       GLuint minDivV)
{
    for (auto &patch : _patches) {
        if (!patch.second.updateVBOs(minDivU, minDivV)) {
            return false;
        }
    }

    return true;
}

void SecondOrderHyperbolicCompositeSurface::renderSurface()
// TODO: Add mode support for wireframes!
{
    for (auto &patch : _patches) {
        patch.second.renderMesh();
    }
}

} // namespace cagd

#include "SecondOrderHyperbolicCompositeSurface.h"

namespace cagd {

SecondOrderHyperbolicCompositeSurface::SecondOrderHyperbolicCompositeSurface()
    : _current_id(0)
{}


SecondOrderHyperbolicCompositeSurface::SurfaceId
SecondOrderHyperbolicCompositeSurface::add(
    SecondOrderHyperbolicPatch *patch_taken)
{
    SurfaceId id = _current_id++;
    auto      inserted =
        _patches.emplace(id, std::move(CompositeSurfaceElement(patch_taken)));

    if (!inserted.second) {
        throw Exception("Failed to emplace new surface patch");
    }

    return id;
}


SecondOrderHyperbolicCompositeSurface::SurfaceId
SecondOrderHyperbolicCompositeSurface::join(
    SurfaceId surfaceIdA, SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA,
    CompositeSurfaceElement::Direction directionB)
{
    auto &surfaceA = _patches.at(surfaceIdA);
    auto &surfaceB = _patches.at(surfaceIdB);

    GLdouble alphaTension =
        (surfaceA.getAlphaTension() + surfaceB.getAlphaTension()) / 2;

    SecondOrderHyperbolicPatch *patchBetween =
        new SecondOrderHyperbolicPatch(alphaTension);

    SurfaceId id = joinToFirst(surfaceIdA, patchBetween, directionA,
                               CompositeSurfaceElement::NORTH);

    joinToFirst(surfaceIdB, id, directionB, CompositeSurfaceElement::SOUTH);

    return id;
}


SecondOrderHyperbolicCompositeSurface::SurfaceId
SecondOrderHyperbolicCompositeSurface::joinToFirst(
    SurfaceId olderSurfaceId, SecondOrderHyperbolicPatch *patch_taken,
    CompositeSurfaceElement::Direction olderDirection,
    CompositeSurfaceElement::Direction newerDirection)
{
    SurfaceId id = add(patch_taken);

    auto &insertedPatch = _patches.at(id);
    _patches.at(olderSurfaceId)
        .joinWith(olderDirection, newerDirection, &insertedPatch);

    return id;
}

void SecondOrderHyperbolicCompositeSurface::joinToFirst(
    SurfaceId surfaceIdA, SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA,
    CompositeSurfaceElement::Direction directionB)
{
    _patches.at(surfaceIdA)
        .joinWith(directionA, directionB, &_patches.at(surfaceIdB));
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

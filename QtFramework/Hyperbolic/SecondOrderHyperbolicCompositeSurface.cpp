#include "SecondOrderHyperbolicCompositeSurface.h"

namespace cagd {

SecondOrderHyperbolicCompositeSurface::SecondOrderHyperbolicCompositeSurface()
    : _current_id(0)
{}


CompositeSurfaceElement::SurfaceId SecondOrderHyperbolicCompositeSurface::add(
    SecondOrderHyperbolicPatch *patch_taken)
{
    CompositeSurfaceElement::SurfaceId id = _current_id++;
    auto                               inserted =
        _patches.emplace(id, std::move(CompositeSurfaceElement(patch_taken)));

    if (!inserted.second) {
        throw Exception("Failed to emplace new surface patch");
    }

    return id;
}


CompositeSurfaceElement::SurfaceId SecondOrderHyperbolicCompositeSurface::join(
    CompositeSurfaceElement::SurfaceId surfaceIdA,
    CompositeSurfaceElement::SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA,
    CompositeSurfaceElement::Direction directionB)
{
    auto &surfaceA = _patches.at(surfaceIdA);
    auto &surfaceB = _patches.at(surfaceIdB);

    GLdouble alphaTension =
        (surfaceA.getAlphaTension() + surfaceB.getAlphaTension()) / 2;

    SecondOrderHyperbolicPatch *patchBetween =
        new SecondOrderHyperbolicPatch(alphaTension);

    CompositeSurfaceElement::SurfaceId id;
    if (directionA % 2) {
        id = joinToFirst(surfaceIdA, patchBetween, directionA,
                         CompositeSurfaceElement::NORTH_EAST);
        joinToFirst(surfaceIdB, id, directionB,
                    CompositeSurfaceElement::SOUTH_WEST);
    } else {
        id = joinToFirst(surfaceIdA, patchBetween, directionA,
                         CompositeSurfaceElement::NORTH);
        joinToFirst(surfaceIdB, id, directionB, CompositeSurfaceElement::SOUTH);
    }

    return id;
}


CompositeSurfaceElement::SurfaceId
SecondOrderHyperbolicCompositeSurface::joinToFirst(
    CompositeSurfaceElement::SurfaceId olderSurfaceId,
    SecondOrderHyperbolicPatch *       patch_taken,
    CompositeSurfaceElement::Direction olderDirection,
    CompositeSurfaceElement::Direction newerDirection)
{
    CompositeSurfaceElement::SurfaceId id = add(patch_taken);

    auto &insertedPatch = _patches.at(id);
    _patches.at(olderSurfaceId)
        .joinWith(olderDirection, newerDirection, &insertedPatch);

    return id;
}

void SecondOrderHyperbolicCompositeSurface::joinToFirst(
    CompositeSurfaceElement::SurfaceId surfaceIdA,
    CompositeSurfaceElement::SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA,
    CompositeSurfaceElement::Direction directionB)
{
    _patches.at(surfaceIdA)
        .joinWith(directionA, directionB, &_patches.at(surfaceIdB));
}


bool SecondOrderHyperbolicCompositeSurface::areJoined(
    CompositeSurfaceElement::SurfaceId surfaceIdA,
    CompositeSurfaceElement::SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA) const
{
    auto &testedA = _patches.at(surfaceIdA);
    auto &testedB = _patches.at(surfaceIdB);
    return testedA.isNeighbor(testedB, directionA);
}


void SecondOrderHyperbolicCompositeSurface::merge(
    CompositeSurfaceElement::SurfaceId surfaceIdA,
    CompositeSurfaceElement::SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA,
    CompositeSurfaceElement::Direction directionB)
{
    auto &patchA = _patches.at(surfaceIdA);
    auto &patchB = _patches.at(surfaceIdB);

    patchA.mergeWith(directionA, directionB, &patchB);
}



void SecondOrderHyperbolicCompositeSurface::setShaderForAll(
    std::shared_ptr<ShaderProgram> shader)
{
    for (auto &patch : _patches) {
        CompositeSurfaceProvider access(patch.second, patch.first);
        access.setShader(shader);
    }
}

void SecondOrderHyperbolicCompositeSurface::setMaterialForAll(
    Material &material)
{
    for (auto &patch : _patches) {
        CompositeSurfaceProvider access(patch.second, patch.first);
        access.setMaterial(material);
    }
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

void SecondOrderHyperbolicCompositeSurface::renderSurface(GLenum flag)
{
    for (auto &patch : _patches) {
        patch.second.renderMesh(flag);
    }
}

void SecondOrderHyperbolicCompositeSurface::renderWireframe(GLenum flag)
{
    for (auto &patch : _patches) {
        patch.second.renderWireframe(flag);
    }
}

void SecondOrderHyperbolicCompositeSurface::renderControlPoints(
    std::shared_ptr<TriangulatedMesh3> pointMesh, bool named) const
{
    if (named) {
        GLuint startCount = 0;
        for (auto &patch : _patches) {
            patch.second.renderControlPoints(pointMesh, startCount);

            startCount += 16;
        }
    } else {
        for (auto &patch : _patches) {
            patch.second.renderControlPoints(pointMesh);
        }
    }
}

void SecondOrderHyperbolicCompositeSurface::renderUVParametricLines() const
{
    for (auto &patch : _patches) {
        patch.second.renderUVParametricLines();
    }
}


CompositeSurfaceProvider SecondOrderHyperbolicCompositeSurface::getProvider(
    CompositeSurfaceElement::SurfaceId id)
{
    return CompositeSurfaceProvider(_patches.at(id), id);
}

CompositeSurfaceProvider
SecondOrderHyperbolicCompositeSurface::getSelected(GLuint selectedIndex,
                                                   GLuint pointCount)
{
    GLuint index = 0;
    for (auto &patch : _patches) {
        if (index == selectedIndex) {
            CompositeSurfaceProvider access(patch.second, patch.first,
                                            pointCount);
            return access;
        }
        ++index;
    }

    throw Exception("Surface not found");
}

size_t SecondOrderHyperbolicCompositeSurface::getPatchCount() const
{
    return _patches.size();
}

} // namespace cagd

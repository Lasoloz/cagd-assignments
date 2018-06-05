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


bool SecondOrderHyperbolicCompositeSurface::areJoined(
    SurfaceId surfaceIdA, SurfaceId surfaceIdB,
    CompositeSurfaceElement::Direction directionA) const
{
    auto &testedA = _patches.at(surfaceIdA);
    auto &testedB = _patches.at(surfaceIdB);
    return testedA.isNeighbor(testedB, directionA);
}


void SecondOrderHyperbolicCompositeSurface::merge(
    SurfaceId surfaceIdA, SurfaceId surfaceIdB,
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
        CompositeSurfaceProvider access(patch.second);
        access.setShader(shader);
    }
}

void SecondOrderHyperbolicCompositeSurface::setMaterialForAll(
    Material &material)
{
    for (auto &patch : _patches) {
        CompositeSurfaceProvider access(patch.second);
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


CompositeSurfaceProvider
SecondOrderHyperbolicCompositeSurface::getProvider(SurfaceId id)
{
    return CompositeSurfaceProvider(_patches.at(id));
}

CompositeSurfaceProvider
SecondOrderHyperbolicCompositeSurface::getSelected(GLuint selectedIndex,
                                                   GLuint pointCount)
{
    GLuint index = 0;
    for (auto &patch : _patches) {
        if (index == selectedIndex) {
            CompositeSurfaceProvider access(patch.second, pointCount);
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

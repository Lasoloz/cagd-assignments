#pragma once

#include <array>
#include <map>
#include <memory>

#include <iostream>

#include "../Core/ShaderPrograms.h"
#include "SecondOrderHyperbolicPatch.h"
#include "util/CompositeSurfaceElement.hpp"
#include "util/CompositeSurfaceProvider.hpp"

namespace cagd {

class SecondOrderHyperbolicCompositeSurface
{
public:
    typedef unsigned SurfaceId;

private:
    SurfaceId                                    _current_id;
    std::map<SurfaceId, CompositeSurfaceElement> _patches;

public:
    SecondOrderHyperbolicCompositeSurface();

    SurfaceId add(SecondOrderHyperbolicPatch *patch_taken);

    SurfaceId join(SurfaceId surfaceIdA, SurfaceId surfaceIdB,
                   CompositeSurfaceElement::Direction directionA,
                   CompositeSurfaceElement::Direction directionB);

    SurfaceId joinToFirst(SurfaceId                          olderSurfaceId,
                          SecondOrderHyperbolicPatch *       patch_taken,
                          CompositeSurfaceElement::Direction olderDirection,
                          CompositeSurfaceElement::Direction newerDirection);
    void      joinToFirst(SurfaceId surfaceIdA, SurfaceId surfaceIdB,
                          CompositeSurfaceElement::Direction directionA,
                          CompositeSurfaceElement::Direction directionB);

    bool areJoined(SurfaceId surfaceIdA, SurfaceId surfaceIdB,
                   CompositeSurfaceElement::Direction directionA) const;

    void merge(SurfaceId surfaceIdA, SurfaceId surfaceIdB,
               CompositeSurfaceElement::Direction directionA,
               CompositeSurfaceElement::Direction directionB);

    void setShaderForAll(std::shared_ptr<ShaderProgram> shader);
    void setMaterialForAll(Material &material);
    bool updateVBOs(GLuint minDivU, GLuint minDivV);
    void renderSurface(GLenum flag = GL_TRIANGLES);
    void renderWireframe(GLenum flag = GL_LINE_STRIP);

    CompositeSurfaceProvider getProvider(SurfaceId id);
    size_t getPatchCount() const;
};

} // namespace cagd

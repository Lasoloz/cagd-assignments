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
private:
    CompositeSurfaceElement::SurfaceId _current_id;
    std::map<CompositeSurfaceElement::SurfaceId, CompositeSurfaceElement>
        _patches;

public:
    SecondOrderHyperbolicCompositeSurface();

    CompositeSurfaceElement::SurfaceId
    add(SecondOrderHyperbolicPatch *patch_taken);

    CompositeSurfaceElement::SurfaceId
    join(CompositeSurfaceElement::SurfaceId surfaceIdA,
         CompositeSurfaceElement::SurfaceId surfaceIdB,
         CompositeSurfaceElement::Direction directionA,
         CompositeSurfaceElement::Direction directionB);

    CompositeSurfaceElement::SurfaceId
         joinToFirst(CompositeSurfaceElement::SurfaceId olderSurfaceId,
                     SecondOrderHyperbolicPatch *       patch_taken,
                     CompositeSurfaceElement::Direction olderDirection,
                     CompositeSurfaceElement::Direction newerDirection);
    void joinToFirst(CompositeSurfaceElement::SurfaceId surfaceIdA,
                     CompositeSurfaceElement::SurfaceId surfaceIdB,
                     CompositeSurfaceElement::Direction directionA,
                     CompositeSurfaceElement::Direction directionB);

    bool areJoined(CompositeSurfaceElement::SurfaceId surfaceIdA,
                   CompositeSurfaceElement::SurfaceId surfaceIdB,
                   CompositeSurfaceElement::Direction directionA) const;

    void merge(CompositeSurfaceElement::SurfaceId surfaceIdA,
               CompositeSurfaceElement::SurfaceId surfaceIdB,
               CompositeSurfaceElement::Direction directionA,
               CompositeSurfaceElement::Direction directionB);

    void setShaderForAll(std::shared_ptr<ShaderProgram> shader);
    void setMaterialForAll(Material &material);
    bool updateVBOs(GLuint minDivU, GLuint minDivV);
    void renderSurface(GLenum flag = GL_TRIANGLES);
    void renderWireframe(GLenum flag = GL_LINE_STRIP);
    void renderControlPoints(std::shared_ptr<TriangulatedMesh3> pointMesh,
                             bool                               named,
                             GLuint                             offset = 0
                             ) const;

    CompositeSurfaceProvider getProvider(CompositeSurfaceElement::SurfaceId id);
    CompositeSurfaceProvider getSelected(GLuint selectedIndex,
                                         GLuint pointCount);

    size_t getPatchCount() const;
};

} // namespace cagd

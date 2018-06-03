#pragma once

#include <array>
#include <map>
#include <memory>

#include <iostream>

#include "SecondOrderHyperbolicPatch.h"
#include "util/CompositeSurfaceElement.hpp"

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

    void joinToFirst(SurfaceId surfaceIdA, SurfaceId surfaceIdB,
                     CompositeSurfaceElement::Direction directionA,
                     CompositeSurfaceElement::Direction directionB);

    bool updateVBOs(GLuint minDivU, GLuint minDivV);
    void renderSurface();
};

} // namespace cagd

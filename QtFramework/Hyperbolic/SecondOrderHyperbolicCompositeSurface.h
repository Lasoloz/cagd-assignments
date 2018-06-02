#pragma once

#include <array>
#include <map>
#include <memory>

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

    bool join(SurfaceId olderSurfaceId, SecondOrderHyperbolicPatch *patch_taken,
              CompositeSurfaceElement::Direction olderDirection,
              CompositeSurfaceElement::Direction newerDirection);

    void updateVBOs();
    void renderSurface();
};

} // namespace cagd

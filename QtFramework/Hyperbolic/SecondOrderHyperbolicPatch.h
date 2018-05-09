#pragma once

#include "../Core/TensorProductSurfaces3.h"
#include <cmath>

namespace cagd {

class SecondOrderHyperbolicPatch : public TensorProductSurface3
{
    GLdouble _alpha;


    GLdouble zerothOrderF2(GLdouble t) const;
    GLdouble zerothOrderF3(GLdouble t) const;
    GLdouble firstOrderF2(GLdouble t) const;
    GLdouble firstOrderF3(GLdouble t) const;

public:
    SecondOrderHyperbolicPatch(GLdouble alpha_tension);

    GLboolean
    UBlendingFunctionValues(GLdouble             u_knot,
                            RowMatrix<GLdouble> &blending_values) const;
    GLboolean
    VBlendingFunctionValues(GLdouble             v_knot,
                            RowMatrix<GLdouble> &blending_values) const;
    GLboolean
    CalculatePartialDerivatives(GLuint   maximum_order_of_partial_derivatives,
                                GLdouble u, GLdouble v,
                                PartialDerivatives &pd) const;
};

} // namespace cagd

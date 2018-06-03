#include "SecondOrderHyperbolicPatch.h"

namespace cagd {

SecondOrderHyperbolicPatch::SecondOrderHyperbolicPatch(GLdouble alpha_tension)
    : TensorProductSurface3(0, alpha_tension, 0, alpha_tension, 4, 4)
    , _alpha(alpha_tension)
{}


GLdouble SecondOrderHyperbolicPatch::zerothOrderF2(GLdouble t) const
{
    GLdouble w        = _alpha - t;
    GLdouble shhalft  = sinh(t / 2);
    GLdouble shhalfw  = sinh(w / 2);
    GLdouble shhalft2 = shhalft * shhalft;
    GLdouble shhalfw2 = shhalfw * shhalfw;
    GLdouble shhalft3 = shhalft2 * shhalft;

    GLdouble halfal    = _alpha / 2;
    GLdouble shhalfal4 = sinh(halfal);
    shhalfal4 *= shhalfal4;
    shhalfal4 *= shhalfal4;
    GLdouble chhalfal = cosh(halfal);
    GLdouble c1       = 4 * chhalfal / shhalfal4;
    GLdouble c2       = (1 + 2 * chhalfal * chhalfal) / shhalfal4;

    return c1 * shhalfw * shhalft3 + c2 * shhalfw2 * shhalft2;
}

GLdouble SecondOrderHyperbolicPatch::zerothOrderF3(GLdouble t) const
{
    GLdouble shhalft4 = sinh(t / 2);
    shhalft4 *= shhalft4;
    shhalft4 *= shhalft4;
    GLdouble c = sinh(_alpha / 2);
    c *= c;
    c *= c;
    return shhalft4 / c;
}

GLdouble SecondOrderHyperbolicPatch::firstOrderF2(GLdouble t) const
{
    GLdouble a2  = _alpha / 2.0;
    GLdouble t2  = t / 2.0;
    GLdouble at2 = a2 - t2;

    GLdouble cosha2 = cosh(a2);
    GLdouble sinha2 = sinh(a2);

    GLdouble cosha22 = 2 * pow(cosha2, 2) + 1;
    GLdouble csch3a2 = 1 / sinha2;
    GLdouble csch4a2 = csch3a2 * csch3a2;
    csch4a2 *= csch4a2;
    csch3a2 *= csch3a2 * csch3a2;

    GLdouble sinht2  = sinh(t2);
    GLdouble sinht22 = sinht2 * sinht2;
    GLdouble sinht23 = sinht22 * sinht2;
    GLdouble sinhat2 = sinh(at2);
    GLdouble cosht2  = cosh(t2);
    GLdouble coshat2 = cosh(at2);

    GLdouble cotha2 = cosha2 / sinha2;

    return -cosha22 * csch4a2 * sinht22 * sinhat2 * coshat2 +
           cosha22 * csch4a2 * sinht2 * cosht2 * pow(sinhat2, 2) -
           2 * cotha2 * csch3a2 * sinht23 * coshat2 +
           6 * cotha2 * csch3a2 * sinht22 * cosht2 * sinhat2;
}

GLdouble SecondOrderHyperbolicPatch::firstOrderF3(GLdouble t) const
{
    GLdouble c = 1 / sinh(_alpha / 2);
    c *= c;
    c *= c;

    GLdouble halft   = t / 2;
    GLdouble shhalft = sinh(halft);
    return 2 * cosh(halft) * c * shhalft * shhalft * shhalft;
}


GLboolean SecondOrderHyperbolicPatch::UBlendingFunctionValues(
    GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const
{
    if (u_knot < 0 || u_knot > _alpha) {
        return GL_FALSE;
    }

    blending_values.ResizeColumns(4);

    blending_values(0) = zerothOrderF3(_alpha - u_knot);
    blending_values(1) = zerothOrderF2(_alpha - u_knot);
    blending_values(2) = zerothOrderF2(u_knot);
    blending_values(3) = zerothOrderF3(u_knot);

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicPatch::VBlendingFunctionValues(
    GLdouble v_knot, RowMatrix<GLdouble> &blending_values) const
{
    return UBlendingFunctionValues(v_knot, blending_values);
}


GLboolean SecondOrderHyperbolicPatch::CalculatePartialDerivatives(
    GLuint maximum_order_of_partial_derivatives, GLdouble u, GLdouble v,
    PartialDerivatives &pd) const
{
    if (u < 0.0 || u > _alpha || v < 0.0 || v > _alpha ||
        maximum_order_of_partial_derivatives > 1) {
        return GL_FALSE;
    }

    RowMatrix<GLdouble> u_blending_values(4), v_blending_values(4),
        d1_u_blending_values(4), d1_v_blending_values(4);

    UBlendingFunctionValues(u, u_blending_values);
    VBlendingFunctionValues(v, v_blending_values);

    d1_u_blending_values(0) = -firstOrderF3(_alpha - u);
    d1_u_blending_values(1) = -firstOrderF2(_alpha - u);
    d1_u_blending_values(2) = firstOrderF2(u);
    d1_u_blending_values(3) = firstOrderF3(u);

    d1_v_blending_values(0) = -firstOrderF3(_alpha - v);
    d1_v_blending_values(1) = -firstOrderF2(_alpha - v);
    d1_v_blending_values(2) = firstOrderF2(v);
    d1_v_blending_values(3) = firstOrderF3(v);


    pd.ResizeRows(2);
    pd.LoadNullVectors();

    for (GLuint row = 0; row < 4; ++row) {
        DCoordinate3 aux_d0_v, aux_d1_v;
        for (GLuint column = 0; column < 4; ++column) {
            aux_d0_v += _data(row, column) * v_blending_values(column);
            aux_d1_v += _data(row, column) * d1_v_blending_values(column);
        }

        pd(0, 0) += aux_d0_v * u_blending_values(row);
        pd(1, 0) += aux_d0_v * d1_u_blending_values(row);
        pd(1, 1) += aux_d1_v * u_blending_values(row);
    }

    return GL_TRUE;
}


GLdouble SecondOrderHyperbolicPatch::getAlphaTension() const { return _alpha; }

} // namespace cagd

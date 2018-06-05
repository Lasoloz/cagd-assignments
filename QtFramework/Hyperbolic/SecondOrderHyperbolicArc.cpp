#include "SecondOrderHyperbolicArc.h"

namespace cagd {
SecondOrderHyperbolicArc::SecondOrderHyperbolicArc(GLdouble alpha)
    : LinearCombination3(0, alpha, 4)
    , _alpha(alpha)
{}

SecondOrderHyperbolicArc::SecondOrderHyperbolicArc(
    const SecondOrderHyperbolicArc &arc)
    : LinearCombination3(arc)
    , _alpha(arc._alpha)
{}

GLboolean SecondOrderHyperbolicArc::BlendingFunctionValues(
    GLdouble u, RowMatrix<GLdouble> &values) const
{
    if (u < 0 || u > _alpha) {
        return GL_FALSE;
    }

    values.ResizeColumns(4);

    values(0) = zerothOrderF3(_alpha - u);
    values(1) = zerothOrderF2(_alpha - u);
    values(2) = zerothOrderF2(u);
    values(3) = zerothOrderF3(u);

    return GL_TRUE;
}

GLboolean
SecondOrderHyperbolicArc::CalculateDerivatives(GLuint max_order_of_derivatives,
                                               GLdouble u, Derivatives &d) const
{
    if (u < 0.0 || u > _alpha || max_order_of_derivatives > 1) {
        return GL_FALSE;
    }

    d.ResizeRows(max_order_of_derivatives + 1);
    d.LoadNullVectors();

    RowMatrix<GLdouble> blending_values(4);
    Matrix<GLdouble>    derivatives(max_order_of_derivatives + 1, 4);

    if (!BlendingFunctionValues(u, blending_values)) {
        return GL_FALSE;
    }

    derivatives.SetRow(0, blending_values);
    derivatives(1, 0) = -firstOrderF3(_alpha - u);
    derivatives(1, 1) = -firstOrderF2(_alpha - u);
    derivatives(1, 2) = firstOrderF2(u);
    derivatives(1, 3) = firstOrderF3(u);

    for (GLuint order = 0; order <= 1; ++order) {
        for (GLuint i = 0; i < 4; ++i) {
            d[order] += _data[i] * derivatives(order, i);
        }
    }

    return GL_TRUE;
}

GLdouble SecondOrderHyperbolicArc::zerothOrderF2(GLdouble t) const
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

GLdouble SecondOrderHyperbolicArc::zerothOrderF3(GLdouble t) const
{
    GLdouble shhalft4 = sinh(t / 2);
    shhalft4 *= shhalft4;
    shhalft4 *= shhalft4;
    GLdouble c = sinh(_alpha / 2);
    c *= c;
    c *= c;
    return shhalft4 / c;
}

GLdouble SecondOrderHyperbolicArc::firstOrderF2(GLdouble t) const
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

GLdouble SecondOrderHyperbolicArc::firstOrderF3(GLdouble t) const
{
    GLdouble c = 1 / sinh(_alpha / 2);
    c *= c;
    c *= c;

    GLdouble halft   = t / 2;
    GLdouble shhalft = sinh(halft);
    return 2 * cosh(halft) * c * shhalft * shhalft * shhalft;
}



std::ostream &operator<<(std::ostream &lhs, const SecondOrderHyperbolicArc &rhs)
{
    lhs << rhs._data.GetRowCount() << " ";
    for (GLuint i = 0; i < rhs._data.GetRowCount(); ++i) {
        lhs << rhs._data[i][0] << " " << rhs._data[i][1] << " "
            << rhs._data[i][2] << std::endl;
    }

    return lhs;
}

std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicArc &rhs)
{
    GLuint size;
    lhs >> size;

    for (GLuint i = 0; i < size; ++i) {
        lhs >> rhs._data[i][0] >> rhs._data[i][1] >> rhs._data[i][2];
    }

    return lhs;
}
} // namespace cagd

#pragma once

#include "../Core/LinearCombination3.h"
#include "../Core/Matrices.h"

namespace cagd {
class SecondOrderHyperbolicArc;
std::ostream &operator<<(std::ostream &                  lhs,
                         const SecondOrderHyperbolicArc &rhs);
std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicArc &rhs);


class SecondOrderHyperbolicArc : public LinearCombination3
{
    friend std::ostream &cagd::operator<<(std::ostream &                  lhs,
                                          const SecondOrderHyperbolicArc &rhs);
    friend std::istream &cagd::operator>>(std::istream &            lhs,
                                          SecondOrderHyperbolicArc &rhs);

protected:
    GLdouble _alpha;

private:
    GLdouble zerothOrderF2(GLdouble t) const;
    GLdouble zerothOrderF3(GLdouble t) const;
    GLdouble firstOrderF2(GLdouble t) const;
    GLdouble firstOrderF3(GLdouble t) const;

public:
    SecondOrderHyperbolicArc(GLdouble alpha);
    SecondOrderHyperbolicArc(const SecondOrderHyperbolicArc &arc);

    // a values az Fi fuggvenyertekeket tartalmazza
    GLboolean BlendingFunctionValues(GLdouble             u,
                                     RowMatrix<GLdouble> &values) const;
    GLboolean CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u,
                                   Derivatives &d) const;
};


std::ostream &operator<<(std::ostream &                  lhs,
                         const SecondOrderHyperbolicArc &rhs);

std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicArc &rhs);
} // namespace cagd

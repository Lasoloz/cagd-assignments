#pragma once

#include "../Core/LinearCombination3.h"
#include "../Core/Matrices.h"

namespace cagd
{
    class SecondOrderHyperbolicArc;
    std::ostream &operator<<(std::ostream &lhs,
                             const SecondOrderHyperbolicArc &rhs);
    std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicArc &rhs);


    class SecondOrderHyperbolicArc: public LinearCombination3
    {
        friend std::ostream &cagd::operator<<(std::ostream &lhs,
                                              const SecondOrderHyperbolicArc &rhs);
        friend std::istream &cagd::operator>>(std::istream &lhs,
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
        GLboolean BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble>& values) const;
        GLboolean CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives &d) const;
    };


    inline std::ostream &operator<<(std::ostream &lhs,
                             const SecondOrderHyperbolicArc &rhs)
    {
        lhs << rhs._data.GetRowCount() << " ";
        for (GLuint i = 0; i < rhs._data.GetRowCount(); ++i) {
            lhs << rhs._data[i][0] << " " << rhs._data[i][1] << " " << rhs._data[i][2] << std::endl;
        }

        return lhs;
    }

    inline std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicArc &rhs)
    {
        GLuint size;
        lhs >> size;

        for (GLuint i = 0; i < size; ++i) {
            lhs >> rhs._data[i][0] >> rhs._data[i][1] >> rhs._data[i][2];
        }

        return lhs;
    }
}

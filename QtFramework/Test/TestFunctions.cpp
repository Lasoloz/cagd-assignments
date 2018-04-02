#include <cmath>
#include "TestFunctions.h"
#include "../Core/Constants.h"

using namespace cagd;
using namespace std;

// Spiral on cone:
// ===============
QString spiral_on_cone::curve_name = "Spiral on cone";
GLdouble spiral_on_cone::u_min = -TWO_PI;
GLdouble spiral_on_cone::u_max = +TWO_PI;
int spiral_on_cone::div = 100;

DCoordinate3 spiral_on_cone::d0(GLdouble u)
{
    return DCoordinate3(u * cos(u), u * sin(u), u);
}

DCoordinate3 spiral_on_cone::d1(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(c - u * s, s + u * c, 1.0);
}

DCoordinate3 spiral_on_cone::d2(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(-2.0 * s - u * c, 2.0 * c - u * s, 0);
}



// Cochleoid:
// ==========
QString cochleoid::curve_name = "Cochleoid";
GLdouble cochleoid::u_min = -10;
GLdouble cochleoid::u_max = 10;
int cochleoid::div = 400;

DCoordinate3 cochleoid::d0(GLdouble u) {
    GLdouble s = sin(u);
    return DCoordinate3(s * cos(u) / u, s*s / u, 0);
}

DCoordinate3 cochleoid::d1(GLdouble u) {
    GLdouble s = sin(u);
    GLdouble c = cos(u);
    GLdouble s2 = sin(2*u);
    GLdouble c2 = cos(2*u);
    return DCoordinate3(-(s2 - 2*u*c2)/(2*u*u), s*(2*u*c - s)/(u*u), 0);
}

DCoordinate3 cochleoid::d2(GLdouble u) {
    GLdouble s2 = sin(2 * u);
    GLdouble c2 = cos(2 * u);
    GLdouble x = ((1 - 2*u*u) * s2 - 2 * u * c2) / (u*u*u);
    GLdouble y = ((2*u*u - 1) * c2 - 2 * u * s2 + 1) / (u*u*u);
    return DCoordinate3(x, y, 0);
}


// Epicycloid
// ==========
QString epicycloid::curve_name = "Epicycloid";
GLdouble epicycloid::u_min = 0;
GLdouble epicycloid::u_max = TWO_PI;
GLdouble epicycloid::r = 1.0;
GLdouble epicycloid::R = 3.0;
int epicycloid::div = 200;

DCoordinate3 epicycloid::d0(GLdouble u) {
    GLdouble rSum = r + R;
    GLdouble rDivU = rSum / r * u;

    return DCoordinate3(rSum * cos(u) - r * cos(rDivU), rSum * sin(u) - r * sin(rDivU), 0);
}

DCoordinate3 epicycloid::d1(GLdouble u) {
    GLdouble rSum = r + R;
    GLdouble rSumU = u * rSum / r;

    return DCoordinate3(rSum * (sin(rSumU) - sin(u)), rSum * (cos(u) - cos(rSumU)), 0);
}

DCoordinate3 epicycloid::d2(GLdouble u) {
    GLdouble rSum = r + R;
    GLdouble rSumU = u * rSum / r;

    return DCoordinate3(rSum*(rSum*cos(rSumU)/r - cos(u)), rSum * (rSum * sin(rSumU) / r - sin(u)), 0);
}


// Viviani's curve:
QString viviani::curve_name = "Viviani's curve";
GLdouble viviani::u_min = -TWO_PI;
GLdouble viviani::u_max = TWO_PI;
int viviani::div = 100;
GLdouble viviani::a = 4;

DCoordinate3 viviani::d0(GLdouble u) {
    return DCoordinate3(
                a * (1 + cos(u)),
                a * sin(u),
                2 * a * sin(u / 2)
                );
}

DCoordinate3 viviani::d1(GLdouble u) {
    return DCoordinate3(
                -a * sin(u),
                a * cos(u),
                a * cos(u / 2)
                );
}

DCoordinate3 viviani::d2(GLdouble u) {
    return DCoordinate3(
                -a * cos(u),
                -a * sin(u),
                (-a / 2) * sin(u / 2)
                );
}


// Loxodrome:
QString loxodrome::curve_name = "Loxodrome";
GLdouble loxodrome::u_min = -10*TWO_PI;
GLdouble loxodrome::u_max = 10*TWO_PI;
int loxodrome::div = 1000;
GLdouble loxodrome::a = 0.1;


DCoordinate3 loxodrome::d0(GLdouble u) {
    GLdouble c = 1.0/sqrt(1 + a * a * u * u);
    return DCoordinate3(
                c * cos(u),
                c * sin(u),
                -c * a * u
                );
}

DCoordinate3 loxodrome::d1(GLdouble u) {
    GLdouble a2u = a*a*u;
    GLdouble a2u2 = a2u * u;
    GLdouble div = a2u2 + 1;
    div = sqrt(div * div * div);
    return DCoordinate3(
                -(a2u2 * sin(u) + a2u * cos(u) + sin(u)) / div,
                (a2u2 * cos(u) - a2u * sin(u) + cos(u)) / div,
                -a / div
                );
}

DCoordinate3 loxodrome::d2(GLdouble u) {
    GLdouble a2 = a * a;
    GLdouble u2 = u * u;
    GLdouble a2u = a2 * u;
    GLdouble a2u2 = a2u * u;
    GLdouble a4u2 = 3 * a2u2 * a2;
    GLdouble expr = sqrt(a2u2 + 1);
    GLdouble expr3 = expr * expr * expr;
    GLdouble expr5 = expr3 * expr * expr;
    GLdouble su = sin(u);
    GLdouble cu = cos(u);
    return DCoordinate3(
                2*a2u*su/expr3 - cu/expr + cu*(a4u2/expr5 - a2 / expr3),
                (-a2u*(a2u2 + 1)*cu - su * (a4u2*(u2 - 2) + a2*(2*u2 + 1) + 1)) / expr5,
                3*a2*a*u / expr5
                );
}



// Fermat spiral:
QString fermat::curve_name = "Fermat's spiral";
GLdouble fermat::u_min = -20;
GLdouble fermat::u_max =  20;
int fermat::div = 1000;
GLdouble fermat::a = 0.5;


DCoordinate3 fermat::d0(GLdouble u) {
    GLdouble su = sqrt(std::abs(u));
    GLdouble sign = (u < 0) ? (-1) : 1;
    return DCoordinate3(
                sign * a * su * cos(u),
                a * su * sin(u),
                0
                );
}

DCoordinate3 fermat::d1(GLdouble u) {
    GLdouble su = sin(u);
    GLdouble cu = cos(u);
    GLdouble div = 2 * sqrt(std::abs(u));
    GLdouble sign = (u < 0) ? (-1) : 1;
    return DCoordinate3(
                sign * a * (cu - 2 * u * su) / div,
                a * (su + 2 * u * cu) / div,
                0
                );
}

DCoordinate3 fermat::d2(GLdouble u) {
    GLdouble su = sin(u);
    GLdouble cu = cos(u);
    GLdouble fu = 4 * u;
    GLdouble fuu = fu * u;
    GLdouble sfuu = sqrt(abs(fuu * u));
    GLdouble sign = (u < 0) ? (-1) : 1;
    return DCoordinate3(
                -sign * a * (fuu * cu + fu * su + cu) / sfuu,
                -a * (fuu * su + su - fu * cu) / sfuu,
                0
                );
}



// Cyclic curve:
QString cyclic::curve_name = "Cyclic curve";
QString cyclic_interpolation::curve_name = "Cyclic curve with interpolation";

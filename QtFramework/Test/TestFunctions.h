#pragma once

#include "../Core/DCoordinates3.h"
#include <QString>

namespace cagd
{
    namespace spiral_on_cone
    {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace cochleoid
    {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace epicycloid {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble R;
        extern GLdouble r;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace viviani {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace loxodrome {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace fermat {
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }



    namespace cyclic {
        extern QString curve_name;
    }

    namespace cyclic_interpolation {
        extern QString curve_name;
    }
}

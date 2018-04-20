#pragma once

#include "../Core/DCoordinates3.h"
#include <QString>

namespace cagd
{
    enum class RendereableType {
        PARAMETRIC_CURVE,
        CYCLIC_CURVE,
        MESH_MOUSE,
        MESH_ELEPHANT
    };

    namespace spiral_on_cone
    {
        extern RendereableType type;
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace cochleoid
    {
        extern RendereableType type;
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace epicycloid {
        extern RendereableType type;
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
        extern RendereableType type;
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace loxodrome {
        extern RendereableType type;
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace fermat {
        extern RendereableType type;
        extern QString curve_name;
        extern GLdouble u_min, u_max;
        extern int div;
        extern GLdouble a;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }



    namespace cyclic {
        extern RendereableType type;
        extern QString curve_name;
    }

    namespace cyclic_interpolation {
        extern RendereableType type;
        extern QString curve_name;
    }


    namespace mouse {
        extern RendereableType type;
        extern QString mesh_name;
    }
}
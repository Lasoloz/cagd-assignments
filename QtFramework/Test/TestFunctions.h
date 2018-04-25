#pragma once

#include <cmath>

#include <QString>

#include "../Core/DCoordinates3.h"

namespace cagd {
namespace spiral_on_cone {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace spiral_on_cone

namespace cochleoid {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace cochleoid

namespace epicycloid {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;
extern GLdouble R;
extern GLdouble r;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace epicycloid

namespace viviani {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;
extern GLdouble a;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace viviani

namespace loxodrome {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;
extern GLdouble a;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace loxodrome

namespace fermat {
extern QString  curve_name;
extern GLdouble u_min, u_max;
extern int      div;
extern GLdouble a;

DCoordinate3 d0(GLdouble);
DCoordinate3 d1(GLdouble);
DCoordinate3 d2(GLdouble);
} // namespace fermat



namespace cyclic {
extern QString curve_name;
}

namespace cyclic_interpolation {
extern QString curve_name;
}



// Surfaces:
namespace hyperboloid {
extern QString surface_name;

extern GLdouble u_min, u_max, v_min, v_max;
extern GLuint   u_div_count, v_div_count;

extern DCoordinate3 d00(GLdouble u, GLdouble v);
extern DCoordinate3 d10(GLdouble u, GLdouble v);
extern DCoordinate3 d01(GLdouble u, GLdouble v);
} // namespace hyperboloid

namespace sphere {
extern QString surface_name;
extern GLdouble radius;

extern GLdouble u_min, u_max, v_min, v_max;
extern GLuint   u_div_count, v_div_count;

extern DCoordinate3 d00(GLdouble u, GLdouble v);
extern DCoordinate3 d10(GLdouble u, GLdouble v);
extern DCoordinate3 d01(GLdouble u, GLdouble v);
} // namespace perturbated_sphere

namespace seashell {
extern QString surface_name;

extern GLdouble u_min, u_max, v_min, v_max;
extern GLuint   u_div_count, v_div_count;

extern DCoordinate3 d00(GLdouble u, GLdouble v);
extern DCoordinate3 d10(GLdouble u, GLdouble v);
extern DCoordinate3 d01(GLdouble u, GLdouble v);
} // namespace seashell


namespace moebius {
extern QString surface_name;

extern GLdouble u_min, u_max, v_min, v_max;
extern GLuint   u_div_count, v_div_count;

extern DCoordinate3 d00(GLdouble u, GLdouble v);
extern DCoordinate3 d10(GLdouble u, GLdouble v);
extern DCoordinate3 d01(GLdouble u, GLdouble v);
} // namespace moebius


namespace klein_bootle {
extern QString surface_name;

extern GLdouble r;

extern GLdouble u_min, u_max, v_min, v_max;
extern GLuint   u_div_count, v_div_count;

extern DCoordinate3 d00(GLdouble u, GLdouble v);
extern DCoordinate3 d10(GLdouble u, GLdouble v);
extern DCoordinate3 d01(GLdouble u, GLdouble v);
} // namespace klein_bootle
} // namespace cagd

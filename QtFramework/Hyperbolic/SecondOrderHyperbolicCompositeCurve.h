#pragma once

#include "../Core/Colors4.h"
#include "../Core/DCoordinates3.h"
#include "../Core/GenericCurves3.h"
#include "../Core/TriangulatedMeshes3.h"
#include "SecondOrderHyperbolicArc.h"

namespace cagd {
class SecondOrderHyperbolicCompositeCurve
{
public:
    enum Direction
    {
        LEFT  = 0,
        RIGHT = 1
    }; // *1

    class ArcAttributes
    {
    public:
        SecondOrderHyperbolicArc *arc;
        GenericCurve3 *           image;
        Color4 *                  color;
        // projekt specifikus attributumok
        GLboolean selected;

        ArcAttributes *next;
        ArcAttributes *previous;

        ArcAttributes();
        ArcAttributes(SecondOrderHyperbolicArc *arc);
        ArcAttributes(const ArcAttributes &attribute);

        ArcAttributes &operator=(const ArcAttributes &attribute);

        ~ArcAttributes();
    };

private:
    SecondOrderHyperbolicArc *initCurve();

protected:
    std::vector<ArcAttributes> _attributes; // esetleg lehet list is
    GLdouble                   _alpha;
    GLuint                     _div_point_count;

    GLboolean _renderCurve;
    GLboolean _renderControlPoints;
    GLboolean _renderControlPolygon;
    GLboolean _renderFirstOrderDerivatives;

    TriangulatedMesh3 _sphere;
    GLdouble          _radius;

    Color4 _selectedColor;

    GLboolean preserveConstraints(ArcAttributes &arc, GLuint controlPointIndex,
                                  DCoordinate3 newValue);
    ArcAttributes *getAffected(ArcAttributes &selectedArc, GLuint controlPoint);

public:
    SecondOrderHyperbolicCompositeCurve(GLdouble alpha,
                                        GLuint   max_arc_count = 1000);

    GLboolean render();
    GLboolean renderClickable(GLboolean withNames);

    GLboolean insertIsolatedArc();
    GLboolean erease(GLuint index);

    GLboolean join(GLuint index1, Direction direction1, GLuint index2,
                   Direction direction2);
    GLboolean merge(GLuint index1, Direction direction1, GLuint index2,
                    Direction direction2);
    GLboolean continueExistingArc(GLuint index, Direction direction);
    GLboolean updateCurve(GLuint curveIndex, GLuint controlPointIndex,
                          DCoordinate3 value);


    GLuint    getCurveCount();
    GLboolean setSelected(GLuint index, GLuint controlPoint, GLboolean value);
    DCoordinate3 getPoint(GLuint curveIndex, GLuint controlPointIndex);

    GLvoid    setRenderCurve(GLboolean value);
    GLvoid    setRenderControlPolygon(GLboolean value);
    GLvoid    setRenderControlPoints(GLboolean value);
    GLvoid    setRenderFirstOrderDerivatives(GLboolean value);
    GLboolean setSelectedColor(GLfloat r, GLfloat g, GLfloat b,
                               GLfloat a = 1.0f);
    GLboolean setCurveColor(GLuint index, GLuint controlPoint, GLfloat r,
                            GLfloat g, GLfloat b, GLfloat a = 1.0f);
};
} // namespace cagd

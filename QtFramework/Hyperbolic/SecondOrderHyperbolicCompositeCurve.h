#pragma once

#include "SecondOrderHyperbolicArc.h"
#include "../Core/GenericCurves3.h"
#include "../Core/Colors4.h"

namespace cagd
{
    class SecondOrderHyperbolicCompositeCurve
    {
    public:
        enum Direction {LEFT = 0, RIGHT = 1}; // *1

        class ArcAttributes
        {
        public:
            SecondOrderHyperbolicArc 	*arc;
            GenericCurve3               *image;
            Color4                      *color;
//            ... // projekt specifikus attributumok

            ArcAttributes *next;
            ArcAttributes *previous;

            ArcAttributes(SecondOrderHyperbolicArc* arc, GenericCurve3 *image, Color4 *color);
            ArcAttributes(const ArcAttributes &attribute);

            ArcAttributes& operator=(const ArcAttributes &attribute);

            ~ArcAttributes();

            // konstruktor
            // copy konstruktor
            // destruktor
            // op =
            // setterek/getterek ha szuksegesek

        };
    protected:
        std::vector<ArcAttributes> 	_attributes; // esetleg lehet list is
        GLdouble 					_alpha;
    public:
         // max arc counttot reservelunk, mert hanem baj lehet a next es previous pointerek miatt
        SecondOrderHyperbolicCompositeCurve(GLdouble alpha, GLuint max_arc_count = 1000);
//            GLboolean insertIsolatedArc(SecondOrderHyperbolicArc* arc);

//            GLboolean render();
//            SecondOrderHyperbolicArc* getArc(GLint pos);
//            GLboolean insertIsolatedArc(GLint pos_x, GLint pos_y, GLfloat size);
//            GLboolean continueArc(GLuint index, Direction direction);
//            GLboolean eraseArc(GLuint pos);
//            GLboolean joinArcs(GLuint arc1_index, GLuint arc2_index, Direction arc1_direction, Direction arc2_direction);

        // insert new isolated arc
        // continue existing arc(index, direction) *1
            // kontroll poligon qk = p3 + k (p3 - p2) k = 0..3
        // join existing arcs(index1, index2, direction1, direction2)
            // ajanlatos kezdo es veg csucsokat szinesnek allitani, igy forgatasok es transzformaciok utan is latszik, hogy melyik-melyik
        // merge existing arcs(index1, index2, direction1, direction2)
            // p3' = q0' = 1/2 * (p2 + q1)
        // render all arcs
            // forciklussal vgigjarom az attributuokat, tesztelem, hogy leteznek-e a pointerek
            // a colort beallitom
            // imageket lehet renderelni
            // arc nak lehet a kontroll poligonjat rendelerni
        // erease exising arc
            // ismet problemas lesz a next es previous pointer
    };
}

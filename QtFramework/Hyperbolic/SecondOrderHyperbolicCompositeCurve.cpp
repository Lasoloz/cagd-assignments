#include "SecondOrderHyperbolicCompositeCurve.h"

namespace cagd
{
    SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(
            SecondOrderHyperbolicArc *arc,
            GenericCurve3 *image,
            Color4 *color)
    {
        this->arc = new SecondOrderHyperbolicArc(*arc);
        this->image = new GenericCurve3(*image);
        this->color = new Color4(*color);
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(const ArcAttributes &attribute)
    {
        this->arc = new SecondOrderHyperbolicArc(*attribute.arc);
        this->image = new GenericCurve3(*attribute.image);
        this->color = new Color4(*attribute.color);
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes& SecondOrderHyperbolicCompositeCurve::ArcAttributes::operator=(const ArcAttributes &attribute)
    {
        if (this != &attribute)
        {
            *(this->arc) = *attribute.arc;
            *(this->image) = *attribute.image;
            *(this->color) = *attribute.color;
        }

        return *this;
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes::~ArcAttributes()
    {
        delete arc;
        delete image;
        delete color;
    }

    SecondOrderHyperbolicCompositeCurve::SecondOrderHyperbolicCompositeCurve(
            GLdouble alpha, GLuint max_arc_count) :
        _alpha(alpha)
    {
        _attributes.reserve(max_arc_count);
    }


}

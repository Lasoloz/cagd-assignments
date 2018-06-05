#include "SecondOrderHyperbolicCompositeCurve.h"

using namespace std;

namespace cagd {
SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes()
{
    this->arc      = nullptr;
    this->image    = nullptr;
    this->color    = nullptr;
    this->next     = nullptr;
    this->previous = nullptr;
    this->selected = GL_FALSE;
}

SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(
    SecondOrderHyperbolicArc *arc)
{
    this->arc      = new SecondOrderHyperbolicArc(*arc);
    this->image    = this->arc->GenerateImage(1, 100); // default 100 div point
    this->color    = new Color4(0.3f, 1.0f, 0.0f, 1.0f); // default red color
    this->next     = nullptr;
    this->previous = nullptr;
    this->selected = GL_FALSE;
}

SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(
    const ArcAttributes &attribute)
{
    if (attribute.arc) {
        this->arc   = new SecondOrderHyperbolicArc(*attribute.arc);
        this->image = new GenericCurve3(*attribute.image);
    } else {
        this->arc   = nullptr;
        this->image = nullptr;
    }

    if (attribute.color)
        this->color = new Color4(*attribute.color);
    else
        this->color = nullptr;

    this->selected = attribute.selected;

    this->next     = attribute.next;
    this->previous = attribute.previous;
}

SecondOrderHyperbolicCompositeCurve::ArcAttributes &
SecondOrderHyperbolicCompositeCurve::ArcAttributes::
operator=(const ArcAttributes &attribute)
{
    if (this != &attribute) {
        *(this->arc)   = *attribute.arc;
        *(this->image) = *attribute.image;
        *(this->color) = *attribute.color;

        this->selected = attribute.selected;

        this->next     = attribute.next;
        this->previous = attribute.previous;
    }

    return *this;
}

SecondOrderHyperbolicCompositeCurve::ArcAttributes::~ArcAttributes()
{
    if (previous) {
        if (previous->next == this)
            previous->next = nullptr;
        else
            previous->previous = nullptr;
    }

    if (next) {
        if (next->previous == this)
            next->previous = nullptr;
        else
            next->next = nullptr;
    }

    if (arc != nullptr)
        delete arc;
    if (image != nullptr)
        delete image;
    if (color != nullptr)
        delete color;
}

SecondOrderHyperbolicCompositeCurve::SecondOrderHyperbolicCompositeCurve(
    GLdouble alpha, GLuint max_arc_count)
    : _alpha(alpha)
    , _div_point_count(100)
    , _renderCurve(GL_TRUE)
    , _renderControlPoints(GL_FALSE)
    , _renderControlPolygon(GL_FALSE)
    , _renderFirstOrderDerivatives(GL_FALSE)
    , _radius(0.05)
{
    _attributes.reserve(max_arc_count);

    _sphere.LoadFromOFF("Models/sphere.off");
    if (!_sphere.UpdateVertexBufferObjects()) {
        throw Exception("Could not update VBO's of sphere!");
    }

    _selectedColor.r() = 0.2f;
    _selectedColor.g() = 0.6f;
    _selectedColor.b() = 1.0f;
}

GLboolean SecondOrderHyperbolicCompositeCurve::insertIsolatedArc()
{
    SecondOrderHyperbolicArc *arc = initCurve();
    ArcAttributes             attribute(arc);

    _attributes.push_back(attribute);
    _attributes.back().arc =
        initCurve(); // new SecondOrderHyperbolicArc(_alpha);

    _attributes.back().image =
        _attributes.back().arc->GenerateImage(1, _div_point_count);

    if (!_attributes.back().image) {
        throw Exception(
            "Could not generate the image of the second order hyperbolic arc!");
    }

    if (!_attributes.back().image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Couldn't generate the VBO of the second order hyperbolic arc!");
    }

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::render()
{
    for (auto &i : _attributes) {
        if (i.arc && _renderControlPolygon) {
            glColor3f(1.f, 0.f, .0f);
            i.arc->RenderData(GL_LINE_STRIP);
        }

        if (i.image && _renderCurve) {
            if (!i.selected)
                glColor3f(i.color->r(), i.color->g(), i.color->b());
            else
                glColor3f(_selectedColor.r(), _selectedColor.g(),
                          _selectedColor.b());

            glLineWidth(2.f);
            i.image->RenderDerivatives(0, GL_LINE_STRIP);
            glLineWidth(1.f);
        }

        if (i.image && _renderFirstOrderDerivatives) {
            glColor3f(.2f, .5f, .7f);
            i.image->RenderDerivatives(1, GL_LINES);
        }

        renderClickable(false);
    }

    return GL_TRUE;
}

GLboolean
SecondOrderHyperbolicCompositeCurve::renderClickable(GLboolean withNames)
{
    if (_renderControlPoints) {
        GLuint count = -1;
        for (auto &i : _attributes) {
            ++count;

            if (_renderControlPoints) {
                for (GLuint j = 0; j < 4; ++j) {
                    DCoordinate3 &cp = (*i.arc)[j];

                    if (withNames)
                        glLoadName(count * 4 + j);

                    glPushMatrix();
                    glTranslated(cp[0], cp[1], cp[2]);
                    glScaled(_radius, _radius, _radius);
                    glColor3f(1.f, 1.f, 0.4f);
                    MatFBBrass.Apply();
                    //                        _shader.Enable();
                    _sphere.Render();
                    //                        _shader.Disable();
                    glPopMatrix();
                }
            }
        }
    }

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::join(GLuint    index1,
                                                    Direction direction1,
                                                    GLuint    index2,
                                                    Direction direction2)
{
    if (_attributes.size() <= index1 || _attributes.size() <= index2)
        return GL_FALSE;


    ArcAttributes &firstCurve  = _attributes[index1];
    ArcAttributes &secondCurve = _attributes[index2];

    if ((direction1 == LEFT && firstCurve.previous) ||
        (direction1 == RIGHT && firstCurve.next) ||
        (direction2 == LEFT && secondCurve.previous) ||
        (direction2 == RIGHT && secondCurve.next))
        return GL_FALSE;

    ArcAttributes newCurveObj = ArcAttributes();
    _attributes.push_back(newCurveObj);
    ArcAttributes &newCurve = _attributes.back();


    SecondOrderHyperbolicArc *arc =
        new (nothrow) SecondOrderHyperbolicArc(_alpha);
    if (!arc) {
        throw Exception("Could not create the second order hyperbolic arc!");
    }

    switch (direction1) {
    case LEFT:
        (*arc)[0] = (*firstCurve.arc)[0];
        (*arc)[1] = 2 * (*firstCurve.arc)[0] - (*firstCurve.arc)[1];

        firstCurve.previous = &newCurve;
        break;
    case RIGHT:
        (*arc)[0] = (*firstCurve.arc)[3];
        (*arc)[1] = 2 * (*firstCurve.arc)[3] - (*firstCurve.arc)[2];

        firstCurve.next = &newCurve;
    }

    newCurve.previous = &firstCurve;

    switch (direction2) {
    case LEFT:
        (*arc)[2] = 2 * (*secondCurve.arc)[0] - (*secondCurve.arc)[1];
        (*arc)[3] = (*secondCurve.arc)[0];

        secondCurve.previous = &newCurve;
        break;
    case RIGHT:
        (*arc)[2] = 2 * (*secondCurve.arc)[3] - (*secondCurve.arc)[2];
        (*arc)[3] = (*secondCurve.arc)[3];

        secondCurve.next = &newCurve;
    }

    newCurve.next = &secondCurve;

    newCurve.arc   = new SecondOrderHyperbolicArc(*arc);
    newCurve.image = newCurve.arc->GenerateImage(1, _div_point_count);
    newCurve.color = new Color4(*(secondCurve.color));

    if (!newCurve.arc->UpdateVertexBufferObjectsOfData()) {
        throw Exception("Could not update the VBOoD's hyperbolic arc!");
    }

    if (!newCurve.image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Could not update the VBO's of the hyperbolic arc image!");
    }

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::merge(GLuint    index1,
                                                     Direction direction1,
                                                     GLuint    index2,
                                                     Direction direction2)
{
    if (_attributes.size() < index1 || _attributes.size() < index2) {
        return GL_FALSE;
    }

    ArcAttributes &firstCurve  = _attributes[index1];
    ArcAttributes &secondCurve = _attributes[index2];

    if ((direction1 == LEFT && firstCurve.previous) ||
        (direction1 == RIGHT && firstCurve.next) ||
        (direction2 == LEFT && secondCurve.previous) ||
        (direction2 == RIGHT && secondCurve.next))
        return GL_FALSE;

    switch (direction1) {
    case LEFT:
        switch (direction2) {
        case LEFT:
            (*firstCurve.arc)[0] = (*secondCurve.arc)[0] =
                0.5 * ((*firstCurve.arc)[1] + (*secondCurve.arc)[1]);

            firstCurve.previous  = &secondCurve;
            secondCurve.previous = &firstCurve;
            break;
        case RIGHT:
            (*firstCurve.arc)[0] = (*secondCurve.arc)[3] =
                0.5 * ((*firstCurve.arc)[1] + (*secondCurve.arc)[2]);

            firstCurve.previous = &secondCurve;
            secondCurve.next    = &firstCurve;
        }

        break;
    case RIGHT:
        switch (direction2) {
        case LEFT:
            (*firstCurve.arc)[3] = (*secondCurve.arc)[0] =
                0.5 * ((*firstCurve.arc)[2] + (*secondCurve.arc)[1]);

            firstCurve.next      = &secondCurve;
            secondCurve.previous = &firstCurve;
            break;
        case RIGHT:
            (*firstCurve.arc)[3] = (*secondCurve.arc)[3] =
                0.5 * ((*firstCurve.arc)[2] + (*secondCurve.arc)[2]);

            firstCurve.next  = &secondCurve;
            secondCurve.next = &firstCurve;
        }
    }

    if (!firstCurve.arc->UpdateVertexBufferObjectsOfData()) {
        throw Exception("Could not update the VBOoD's hyperbolic arc!");
    }

    if (!secondCurve.arc->UpdateVertexBufferObjectsOfData()) {
        throw Exception("Could not update the VBOoD's hyperbolic arc!");
    }

    (*firstCurve.image) = (*firstCurve.arc->GenerateImage(1, _div_point_count));
    (*secondCurve.image) =
        (*secondCurve.arc->GenerateImage(1, _div_point_count));

    if (!firstCurve.image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Could not update the VBO's of the hyperbolic arc image!");
    }

    if (!secondCurve.image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Could not update the VBO's of the hyperbolic arc image!");
    }

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::erease(GLuint index)
{
    if (index >= _attributes.size())
        return GL_FALSE;

    ArcAttributes &arc = _attributes[index];

    if (arc.previous) {
        if (arc.previous->next == &arc)
            arc.previous->next = nullptr;
        else
            arc.previous->previous = nullptr;
    }
    if (arc.next) {
        if (arc.next->previous == &arc)
            arc.next->previous = nullptr;
        else
            arc.next->next = nullptr;
    }

    ArcAttributes &last = _attributes.back();
    if (&last != &arc) {
        if (last.next) {
            if (last.next->previous == &last)
                last.next->previous = &arc;
            else
                last.next->next = &arc;
        }
        if (last.previous) {
            if (last.previous->next == &last)
                last.previous->next = &arc;
            else
                last.previous->previous = &arc;
        }
    }
    arc.next     = nullptr;
    arc.previous = nullptr;

    std::swap(_attributes[index], _attributes.back());
    _attributes.pop_back();

    if (_attributes.size() > index) {
        if (!_attributes[index].image->UpdateVertexBufferObjects()) {
            throw Exception(
                "Could not update the VBO's of the hyperbolic arc image!");
        }
    }

    return GL_TRUE;
}

GLboolean
SecondOrderHyperbolicCompositeCurve::continueExistingArc(GLuint    index,
                                                         Direction direction)
{
    if (index >= _attributes.size())
        return GL_FALSE;

    ArcAttributes &existingCurve = _attributes[index];

    if ((direction == LEFT && existingCurve.previous != nullptr) ||
        (direction == RIGHT && existingCurve.next != nullptr))
        return GL_FALSE;

    ArcAttributes newCurveObj = ArcAttributes();
    _attributes.push_back(newCurveObj);
    ArcAttributes &newCurve = _attributes.back();

    SecondOrderHyperbolicArc *arc =
        new (nothrow) SecondOrderHyperbolicArc(_alpha);
    if (!arc) {
        throw Exception("Could not create the second order hyperbolic arc!");
    }

    switch (direction) {
    case LEFT:
        (*arc)[3] = (*existingCurve.arc)[0];
        (*arc)[2] = 2 * (*existingCurve.arc)[0] - (*existingCurve.arc)[1];
        (*arc)[1] = 2 * (*arc)[2] - (*existingCurve.arc)[0];
        (*arc)[0] = 2 * (*arc)[1] - (*arc)[2];

        existingCurve.previous = &newCurve;
        newCurve.next          = &existingCurve;
        break;
    case RIGHT:
        (*arc)[0] = (*existingCurve.arc)[3];
        (*arc)[1] = 2 * (*existingCurve.arc)[3] - (*existingCurve.arc)[2];
        (*arc)[2] = 2 * (*arc)[1] - (*existingCurve.arc)[3];
        (*arc)[3] = 2 * (*arc)[2] - (*arc)[1];

        existingCurve.next = &newCurve;
        newCurve.previous  = &existingCurve;
    }

    newCurve.arc   = new SecondOrderHyperbolicArc(*arc);
    newCurve.image = newCurve.arc->GenerateImage(1, _div_point_count);
    newCurve.color = new Color4(*(existingCurve.color));

    if (!newCurve.arc->UpdateVertexBufferObjectsOfData()) {
        throw Exception("Could not update the VBOoD's hyperbolic arc!");
    }

    if (!newCurve.image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Could not update the VBO's of the hyperbolic arc image!");
    }

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::updateCurve(
    GLuint curveIndex, GLuint controlPointIndex, DCoordinate3 value)
{
    if (curveIndex >= _attributes.size() || controlPointIndex > 3)
        return GL_FALSE;

    ArcAttributes &selectedCurve = _attributes[curveIndex];

    (*selectedCurve.arc)[controlPointIndex] = value;
    preserveConstraints(selectedCurve, controlPointIndex, value);

    if (!selectedCurve.arc->UpdateVertexBufferObjectsOfData()) {
        throw Exception("Could not update the VBOoD's hyperbolic arc!");
    }

    (*selectedCurve.image) =
        (*selectedCurve.arc->GenerateImage(1, _div_point_count));

    if (!selectedCurve.image->UpdateVertexBufferObjects()) {
        throw Exception(
            "Could not update the VBO's of the hyperbolic arc image!");
    }

    return GL_TRUE;
}

//    GLboolean
//    SecondOrderHyperbolicCompositeCurve::preserveConstraints(ArcAttributes&
//    arc, GLuint controlPointIndex, DCoordinate3 newValue)
//    {
//        if (controlPointIndex > 3)
//            return GL_FALSE;

//        if ((controlPointIndex < 2 && !arc.previous) || (controlPointIndex >=
//        2 && !arc.next))
//            return GL_TRUE;

//        DCoordinate3 delta;

//        switch (controlPointIndex) {
//        case 0:

//            if (arc.previous->next == &arc)
//            {
//                delta = (*arc.previous->arc)[3] - newValue;

//                (*arc.previous->arc)[3] = newValue;
//                (*arc.previous->arc)[2] += delta;
//                (*arc.arc)[1] += delta;
//            }
//            else
//            {
//                delta = (*arc.previous->arc)[0] - newValue;

//                (*arc.previous->arc)[0] = newValue;
//                (*arc.previous->arc)[1] += delta;
//            }

//            if (!arc.previous->arc->UpdateVertexBufferObjectsOfData())
//            {
//                throw Exception("Could not update the VBOoD's hyperbolic
//                arc!");
//            }

//            (*arc.previous->image) = (*arc.previous->arc->GenerateImage(1,
//            _div_point_count));

//            if (!arc.previous->image->UpdateVertexBufferObjects())
//            {
//                throw Exception("Could not update the VBO's of the hyperbolic
//                arc image!");
//            }

//            break;
//        case 1:
//            if (arc.previous->next == &arc)
//                (*arc.previous->arc)[2] = 2 * (*arc.arc)[0] - newValue;
//            else
//                (*arc.previous->arc)[2] = 2 * (*arc.arc)[0] - newValue;

//            if (!arc.previous->arc->UpdateVertexBufferObjectsOfData())
//            {
//                throw Exception("Could not update the VBOoD's hyperbolic
//                arc!");
//            }

//            (*arc.previous->image) = (*arc.previous->arc->GenerateImage(1,
//            _div_point_count));

//            if (!arc.previous->image->UpdateVertexBufferObjects())
//            {
//                throw Exception("Could not update the VBO's of the hyperbolic
//                arc image!");
//            }

//            break;
//        case 2:
//            if (arc.next->previous == &arc)
//                (*arc.next->arc)[1] = 2 * (*arc.arc)[3] - newValue;
//            else
//                (*arc.next->arc)[2] = 2 * (*arc.arc)[3] - newValue;

//            if (!arc.next->arc->UpdateVertexBufferObjectsOfData())
//            {
//                throw Exception("Could not update the VBOoD's hyperbolic
//                arc!");
//            }

//            (*arc.next->image) = (*arc.next->arc->GenerateImage(1,
//            _div_point_count));

//            if (!arc.next->image->UpdateVertexBufferObjects())
//            {
//                throw Exception("Could not update the VBO's of the hyperbolic
//                arc image!");
//            }

//            break;
//        case 3:
//            if (arc.next->previous == &arc)
//            {
//                delta = newValue - (*arc.next->arc)[0];

//                (*arc.next->arc)[0] = newValue;
//                (*arc.next->arc)[1] += delta;
//            }
//            else
//            {
//                delta = newValue - (*arc.next->arc)[3];

//                (*arc.next->arc)[3] = newValue;
//                (*arc.next->arc)[2] += delta;
//                (*arc.arc)[2] += delta;
//            }

//            if (!arc.next->arc->UpdateVertexBufferObjectsOfData())
//            {
//                throw Exception("Could not update the VBOoD's hyperbolic
//                arc!");
//            }

//            (*arc.next->image) = (*arc.next->arc->GenerateImage(1,
//            _div_point_count));

//            if (!arc.next->image->UpdateVertexBufferObjects())
//            {
//                throw Exception("Could not update the VBO's of the hyperbolic
//                arc image!");
//            }

//            break;
//        }

//        return GL_TRUE;
//    }

GLboolean SecondOrderHyperbolicCompositeCurve::preserveConstraints(
    ArcAttributes &arc, GLuint controlPointIndex, DCoordinate3 newValue)
{
    if (controlPointIndex > 3)
        return GL_FALSE;

    if ((controlPointIndex < 2 && !arc.previous) ||
        (controlPointIndex >= 2 && !arc.next))
        return GL_TRUE;

    DCoordinate3 delta;

    switch (controlPointIndex) {
    case 0:

        if (arc.previous->next == &arc) {
            delta = (*arc.previous->arc)[3] - newValue;

            (*arc.previous->arc)[3] = newValue;
            (*arc.previous->arc)[2] += delta;
            (*arc.arc)[1] += delta;
        } else {
            delta = (*arc.previous->arc)[0] - newValue;

            (*arc.previous->arc)[0] = newValue;
            (*arc.previous->arc)[1] += delta;
            (*arc.arc)[2] += delta;
        }

        if (!arc.previous->arc->UpdateVertexBufferObjectsOfData()) {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        (*arc.previous->image) =
            (*arc.previous->arc->GenerateImage(1, _div_point_count));

        if (!arc.previous->image->UpdateVertexBufferObjects()) {
            throw Exception(
                "Could not update the VBO's of the hyperbolic arc image!");
        }

        break;
    case 1:
        if (arc.previous->next == &arc)
            (*arc.previous->arc)[2] = 2 * (*arc.arc)[0] - newValue;
        else
            (*arc.previous->arc)[1] = 2 * (*arc.arc)[0] - newValue;

        if (!arc.previous->arc->UpdateVertexBufferObjectsOfData()) {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        (*arc.previous->image) =
            (*arc.previous->arc->GenerateImage(1, _div_point_count));

        if (!arc.previous->image->UpdateVertexBufferObjects()) {
            throw Exception(
                "Could not update the VBO's of the hyperbolic arc image!");
        }

        break;
    case 2:
        if (arc.next->previous == &arc)
            (*arc.next->arc)[1] = 2 * (*arc.arc)[3] - newValue;
        else
            (*arc.next->arc)[2] = 2 * (*arc.arc)[3] - newValue;

        if (!arc.next->arc->UpdateVertexBufferObjectsOfData()) {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        (*arc.next->image) =
            (*arc.next->arc->GenerateImage(1, _div_point_count));

        if (!arc.next->image->UpdateVertexBufferObjects()) {
            throw Exception(
                "Could not update the VBO's of the hyperbolic arc image!");
        }

        break;
    case 3:
        if (arc.next->previous == &arc) {
            delta = newValue - (*arc.next->arc)[0];

            (*arc.next->arc)[0] = newValue;
            (*arc.next->arc)[1] += delta;
            (*arc.arc)[1] += delta;
        } else {
            delta = newValue - (*arc.next->arc)[3];

            (*arc.next->arc)[3] = newValue;
            (*arc.next->arc)[2] += delta;
            (*arc.arc)[2] += delta;
        }

        if (!arc.next->arc->UpdateVertexBufferObjectsOfData()) {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        (*arc.next->image) =
            (*arc.next->arc->GenerateImage(1, _div_point_count));

        if (!arc.next->image->UpdateVertexBufferObjects()) {
            throw Exception(
                "Could not update the VBO's of the hyperbolic arc image!");
        }

        break;
    }

    return GL_TRUE;
}

SecondOrderHyperbolicArc *SecondOrderHyperbolicCompositeCurve::initCurve()
{
    GLuint                    _n = 4; // 4 kontroll pont
    SecondOrderHyperbolicArc *arc =
        new (nothrow) SecondOrderHyperbolicArc(_alpha);

    if (!arc) {
        throw Exception("Could not create the second order hyperbolic arc!");
    }

    try {
        GLdouble step = TWO_PI / (_n);
        for (GLuint i = 0; i < _n; ++i) {
            GLdouble      u  = i * step;
            DCoordinate3 &cp = (*arc)[i]; // ez a p(i) vektor

            cp[0] = cos(u);
            cp[1] = sin(u);
            cp[2] = -2.0 + 4.0 * (GLdouble)rand() / RAND_MAX; // kesobb
        }

        if (!arc->UpdateVertexBufferObjectsOfData()) {
            throw Exception("Could not update update the VBOs of the second "
                            "order hyperbolic arc's control polygon");
        }

        return arc;
    } catch (Exception &e) {
        cout << e << endl;
    }

    return nullptr;
}

DCoordinate3
SecondOrderHyperbolicCompositeCurve::getPoint(GLuint curveIndex,
                                              GLuint controlPointIndex)
{
    if (curveIndex >= _attributes.size() || controlPointIndex > 3) {
        throw Exception("Index out of bounds");
    }

    return (*_attributes[curveIndex].arc)[controlPointIndex];
}

GLboolean SecondOrderHyperbolicCompositeCurve::setSelected(GLuint index,
                                                           GLuint controlPoint,
                                                           GLboolean value)
{
    if (index >= _attributes.size())
        return GL_FALSE;

    ArcAttributes &selectedArc = _attributes[index];
    selectedArc.selected       = value;
    ArcAttributes *affectedArc = getAffected(selectedArc, controlPoint);
    if (affectedArc)
        affectedArc->selected = value;

    return GL_TRUE;
}

SecondOrderHyperbolicCompositeCurve::ArcAttributes *
SecondOrderHyperbolicCompositeCurve::getAffected(ArcAttributes &selectedArc,
                                                 GLuint         controlPoint)
{
    if (controlPoint > 3)
        return nullptr;

    if ((controlPoint < 2 && !selectedArc.previous) ||
        (controlPoint >= 2 && !selectedArc.next))
        return nullptr;

    if (controlPoint < 2)
        return selectedArc.previous;
    else
        return selectedArc.next;

    //        if (controlPoint < 2){
    //            if (selectedArc.previous)
    //                if (selectedArc.previous->next == &selectedArc ||
    //                selectedArc.previous->previous == &selectedArc)
    //                    return selectedArc.previous;
    //            if (selectedArc.next)
    //                if (selectedArc.next->next == &selectedArc ||
    //                selectedArc.next->previous == &selectedArc)
    //                    return selectedArc.next;
    //        }
    //        else
    //        {
    //            if (selectedArc.previous)
    //                if (selectedArc.previous->next == &selectedArc ||
    //                selectedArc.previous->previous == &selectedArc)
    //                    return selectedArc.previous;
    //            if (selectedArc.next)
    //                if (selectedArc.next->next == &selectedArc ||
    //                selectedArc.next->previous == &selectedArc)
    //                    return selectedArc.next;
    //        }
}

GLboolean SecondOrderHyperbolicCompositeCurve::setSelectedColor(GLfloat r,
                                                                GLfloat g,
                                                                GLfloat b,
                                                                GLfloat a)
{
    if (r < 0.0f || r > 1.f || g < 0.0f || g > 1.f || b < 0.0f || b > 1.f ||
        a < 0.0f || a > 1.f)
        return GL_FALSE;

    _selectedColor.r() = r;
    _selectedColor.g() = g;
    _selectedColor.b() = b;
    _selectedColor.a() = a;

    return GL_TRUE;
}

GLboolean SecondOrderHyperbolicCompositeCurve::setCurveColor(
    GLuint index, GLuint controlPoint, GLfloat r, GLfloat g, GLfloat b,
    GLfloat a)
{
    if (index >= _attributes.size())
        return GL_FALSE;

    if (r < 0.0f || r > 1.f || g < 0.0f || g > 1.f || b < 0.0f || b > 1.f ||
        a < 0.0f || a > 1.f)
        return GL_FALSE;

    ArcAttributes &selectedArc = _attributes[index];
    ArcAttributes *affectedArc = getAffected(selectedArc, controlPoint);
    Color4         newColor(r, g, b, a);

    if (affectedArc)
        (*affectedArc->color) = (*selectedArc.color) = newColor;
    else
        (*selectedArc.color) = newColor;

    //        Color4*& curveColor = _attributes[index].color;

    //        curveColor->r() = r;
    //        curveColor->g() = g;
    //        curveColor->b() = b;
    //        curveColor->a() = a;

    return GL_TRUE;
}

GLuint SecondOrderHyperbolicCompositeCurve::getCurveCount()
{
    return (GLuint)_attributes.size();
}

GLvoid SecondOrderHyperbolicCompositeCurve::setRenderCurve(GLboolean value)
{
    _renderCurve = value;
}

GLvoid
SecondOrderHyperbolicCompositeCurve::setRenderControlPolygon(GLboolean value)
{
    _renderControlPolygon = value;
}

GLvoid
SecondOrderHyperbolicCompositeCurve::setRenderControlPoints(GLboolean value)
{
    _renderControlPoints = value;
}

GLvoid SecondOrderHyperbolicCompositeCurve::setRenderFirstOrderDerivatives(
    GLboolean value)
{
    _renderFirstOrderDerivatives = value;
}

std::ostream &operator<<(std::ostream &lhs,
                         const SecondOrderHyperbolicCompositeCurve &rhs)
{
    lhs << rhs._alpha << " " << rhs._div_point_count << " " << rhs._attributes.size() << std::endl;
    lhs << rhs._selectedColor.r() << " "
        << rhs._selectedColor.g() << " "
        << rhs._selectedColor.b() << " "
        << rhs._selectedColor.a() << std::endl;

    for (auto& attribute : rhs._attributes) {
        lhs << attribute << std::endl;
    }

    const SecondOrderHyperbolicCompositeCurve::ArcAttributes* firstElement = &rhs._attributes[0];
    for(auto attribute : rhs._attributes) {
        lhs << (attribute.previous ? (attribute.previous - firstElement) : (-1)) << " ";
        lhs << (attribute.next ? (attribute.next - firstElement) : (-1)) << std::endl;
    }

    return lhs;
}

std::istream &operator>>(std::istream &lhs, SecondOrderHyperbolicCompositeCurve &rhs)
{
    GLuint size;
    lhs >> rhs._alpha >> rhs._div_point_count >> size;
    lhs >> rhs._selectedColor.r() >> rhs._selectedColor.g() >>
           rhs._selectedColor.b() >> rhs._selectedColor.a();

    rhs._attributes.clear();
    rhs._attributes.reserve(1024);

    for (GLuint i = 0; i < size; ++i) {
        SecondOrderHyperbolicArc* arc = new SecondOrderHyperbolicArc(rhs._alpha);
        GenericCurve3* image = new GenericCurve3(1, rhs._div_point_count);
        Color4* color = new Color4();
        lhs >> (*arc) >> (*image) >> color->r() >> color->g() >> color->b() >> color->a();

        SecondOrderHyperbolicCompositeCurve::ArcAttributes createAttrbibute;
        rhs._attributes.push_back(createAttrbibute);
        SecondOrderHyperbolicCompositeCurve::ArcAttributes& newAttribute = rhs._attributes.back();

        newAttribute.arc = arc;
        newAttribute.image = image;
        newAttribute.color = color;

        if (!arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update update the VBOs of the second "
                            "order hyperbolic arc's control polygon");
        }

        if (!image->UpdateVertexBufferObjects())
        {
            throw Exception(
                "Couldn't generate the VBO of the second order hyperbolic arc!");
        }
    }

    for (GLuint i = 0; i < size; ++i) {
        GLint index;
        lhs >> index;
        rhs._attributes[i].previous = index != -1 ? &rhs._attributes[index] : nullptr;

        lhs >> index;
        rhs._attributes[i].next = index != -1 ? &rhs._attributes[index] : nullptr;
    }

    return lhs;
}
} // namespace cagd

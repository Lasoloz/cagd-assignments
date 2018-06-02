#include "SecondOrderHyperbolicCompositeCurve.h"
#include "../Core/Materials.h"
#include "../Core/Constants.h"

#include <iostream>

using namespace std;

namespace cagd
{
    SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes()
    {
        this->arc = nullptr;
        this->image = nullptr;
        this->color = nullptr;
        this->next = nullptr;
        this->previous = nullptr;
        this->selected = GL_FALSE;
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(
            SecondOrderHyperbolicArc *arc)
    {
        this->arc = new SecondOrderHyperbolicArc(*arc);
        this->image = this->arc->GenerateImage(1, 100);   // default 100 div point
        this->color = new Color4(0.3f, 1.0f, 0.0f, 1.0f); // default red color
        this->next = nullptr;
        this->previous = nullptr;
        this->selected = GL_FALSE;
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes::ArcAttributes(const ArcAttributes &attribute)
    {
        if (attribute.arc)
        {
            this->arc = new SecondOrderHyperbolicArc(*attribute.arc);
            this->image = new GenericCurve3(*attribute.image);
        }
        else
        {
            this->arc = nullptr;
            this->image = nullptr;
        }

        if (attribute.color)
            this->color = new Color4(*attribute.color);
        else
            this->color = nullptr;

        this->selected = attribute.selected;

        this->next = attribute.next;
        this->previous = attribute.previous;
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes& SecondOrderHyperbolicCompositeCurve::ArcAttributes::operator=(const ArcAttributes &attribute)
    {
        if (this != &attribute)
        {
            *(this->arc) = *attribute.arc;
            *(this->image) = *attribute.image;
            *(this->color) = *attribute.color;

            this->selected = attribute.selected;

            this->next = attribute.next;
            this->previous = attribute.previous;
        }

        return *this;
    }

    SecondOrderHyperbolicCompositeCurve::ArcAttributes::~ArcAttributes()
    {
        if (previous)
        {
            if (previous->next == this)
                previous->next = nullptr;
            else
                previous->previous = nullptr;
        }

        if (next)
        {
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
            GLdouble alpha, GLuint max_arc_count) :
        _alpha(alpha),
        _radius(0.05),
        _div_point_count(100),
        _renderCurve(GL_TRUE),
        _renderControlPoints(GL_FALSE),
        _renderControlPolygon(GL_FALSE),
        _renderFirstOrderDerivatives(GL_FALSE)
    {
        _attributes.reserve(max_arc_count);

        _sphere.LoadFromOFF("Models/sphere.off");
        if (!_sphere.UpdateVertexBufferObjects())
        {
            throw Exception("Could not update VBO's of sphere!");
        }

        _selectedColor.r() = 0.2f;
        _selectedColor.g() = 0.6f;
        _selectedColor.b() = 1.0f;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::insertIsolatedArc()
    {
        SecondOrderHyperbolicArc* arc = initCurve();
        ArcAttributes attribute(arc);

        _attributes.push_back(attribute);
        _attributes.back().arc = initCurve();// new SecondOrderHyperbolicArc(_alpha);

        _attributes.back().image = _attributes.back().arc->GenerateImage(1, _div_point_count);

        if (!_attributes.back().image)
        {
            throw Exception("Could not generate the image of the second order hyperbolic arc!");
        }

        if (!_attributes.back().image->UpdateVertexBufferObjects())
        {
            throw Exception("Couldn't generate the VBO of the second order hyperbolic arc!");
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::render()
    {
        for (auto &i : _attributes)
        {
            if (i.arc && _renderControlPolygon)
            {
                glColor3f(1.f, 0.f, .0f);
                i.arc->RenderData(GL_LINE_STRIP);
            }

            if (i.image && _renderCurve)
            {
                if (!i.selected)
                    glColor3f(i.color->r(), i.color->g(), i.color->b());
                else
                    glColor3f(_selectedColor.r(), _selectedColor.g(), _selectedColor.b());

                glLineWidth(2.f);
                i.image->RenderDerivatives(0, GL_LINE_STRIP);
                glLineWidth(1.f);
            }

            if (i.image && _renderFirstOrderDerivatives)
            {
                glColor3f(.2f, .5f, .7f);
                i.image->RenderDerivatives(1, GL_LINES);
            }

            renderClickable(false);
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::renderClickable(GLboolean withNames)
    {
        if (_renderControlPoints)
        {
            GLuint count = -1;
            for (auto &i : _attributes)
            {
                ++count;

                if (_renderControlPoints)
                {
                    for (GLuint j = 0; j < 4; ++j)
                    {
                        DCoordinate3 &cp = (*i.arc)[j];

                        if (withNames)
                            glLoadName(count * 4 + j);

                        glPushMatrix();
                            glTranslated(cp[0], cp[1], cp[2]);
                            glScaled(_radius, _radius, _radius);
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

    GLboolean SecondOrderHyperbolicCompositeCurve::join(GLuint index1, Direction direction1, GLuint index2, Direction direction2)
    {
        if (_attributes.size() <= index1 || _attributes.size() <= index2)
            return GL_FALSE;


        ArcAttributes& firstCurve = _attributes[index1];
        ArcAttributes& secondCurve = _attributes[index2];

        if ((direction1 == LEFT && firstCurve.previous) || (direction1 == RIGHT && firstCurve.next) ||
            (direction2 == LEFT && secondCurve.previous) || (direction2 == RIGHT && secondCurve.next))
            return GL_FALSE;

        ArcAttributes newCurveObj = ArcAttributes();
        _attributes.push_back(newCurveObj);
        ArcAttributes& newCurve = _attributes.back();


        SecondOrderHyperbolicArc* arc = new (nothrow) SecondOrderHyperbolicArc(_alpha);
        if (!arc)
        {
            throw Exception("Could not create the second order hyperbolic arc!");
        }

        switch (direction1)
        {
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

        newCurve.arc = new SecondOrderHyperbolicArc(*arc);
        newCurve.image = newCurve.arc->GenerateImage(1, _div_point_count);
        newCurve.color = new Color4(*(secondCurve.color));

        if (!newCurve.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        if (!newCurve.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO's of the hyperbolic arc image!");
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::merge(GLuint index1, Direction direction1, GLuint index2, Direction direction2)
    {
        if (_attributes.size() < index1 || _attributes.size() < index2)
        {
            return GL_FALSE;
        }

        ArcAttributes& firstCurve = _attributes[index1];
        ArcAttributes& secondCurve = _attributes[index2];

        if ((direction1 == LEFT && firstCurve.previous) || (direction1 == RIGHT && firstCurve.next) ||
            (direction2 == LEFT && secondCurve.previous) || (direction2 == RIGHT && secondCurve.next))
            return GL_FALSE;

        switch (direction1)
        {
            case LEFT:
                switch (direction2) {
                    case LEFT:
                        (*firstCurve.arc)[0] = (*secondCurve.arc)[0] = 0.5 * ((*firstCurve.arc)[1] + (*secondCurve.arc)[1]);

                        firstCurve.previous = &secondCurve;
                        secondCurve.previous = &firstCurve;
                        break;
                    case RIGHT:
                        (*firstCurve.arc)[0] = (*secondCurve.arc)[3] = 0.5 * ((*firstCurve.arc)[1] + (*secondCurve.arc)[2]);

                        firstCurve.previous = &secondCurve;
                        secondCurve.next = &firstCurve;
                }

                break;
            case RIGHT:
                switch (direction2) {
                    case LEFT:
                        (*firstCurve.arc)[3] = (*secondCurve.arc)[0] = 0.5 * ((*firstCurve.arc)[2] + (*secondCurve.arc)[1]);

                        firstCurve.next = &secondCurve;
                        secondCurve.previous = &firstCurve;
                        break;
                    case RIGHT:
                        (*firstCurve.arc)[3] = (*secondCurve.arc)[3] = 0.5 * ((*firstCurve.arc)[2] + (*secondCurve.arc)[2]);

                        firstCurve.next = &secondCurve;
                        secondCurve.next = &firstCurve;
                }
        }

        if (!firstCurve.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        if (!secondCurve.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        delete firstCurve.image;
        firstCurve.image = nullptr;
        firstCurve.image = firstCurve.arc->GenerateImage(1, _div_point_count);
        if (!firstCurve.image)
        {
            throw Exception("Could not create the image of hyperbolic arc image!");
        }

        delete secondCurve.image;
        secondCurve.image = nullptr;
        secondCurve.image = secondCurve.arc->GenerateImage(1, _div_point_count);
        if (!secondCurve.image)
        {
            throw Exception("Could not create the image of hyperbolic arc image!");
        }

        if (!firstCurve.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO's of the hyperbolic arc image!");
        }

        if (!secondCurve.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO's of the hyperbolic arc image!");
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::erease(GLuint index)
    {
        if (index >= _attributes.size())
            return GL_FALSE;

        ArcAttributes& arc = _attributes[index];

        if (arc.previous)
        {
            if (arc.previous->next == &arc)
                arc.previous->next = nullptr;
            else
                arc.previous->previous = nullptr;
        }
        if (arc.next)
        {
            if (arc.next->previous == &arc)
                arc.next->previous = nullptr;
            else
                arc.next->next = nullptr;
        }

        ArcAttributes& last = _attributes.back();
        if (&last != &arc)
        {
            if (last.next)
            {
                if (last.next->previous == &last)
                    last.next->previous = &arc;
                else
                    last.next->next = &arc;
            }
            if (last.previous)
            {
                if(last.previous->next == &last)
                    last.previous->next = &arc;
                else
                    last.previous->previous = &arc;
            }
        }
        arc.next = nullptr;
        arc.previous = nullptr;

        std::swap(_attributes[index], _attributes.back());
        _attributes.pop_back();

        if (_attributes.size() > 0)
        {
            if (!_attributes[0].image->UpdateVertexBufferObjects())
            {
                throw Exception("Could not update the VBO's of the hyperbolic arc image!");
            }
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::continueExistingArc(GLuint index, Direction direction)
    {
        if (index >= _attributes.size())
            return GL_FALSE;

        ArcAttributes& existingCurve = _attributes[index];

        if ((direction == LEFT && existingCurve.previous != nullptr) || (direction == RIGHT && existingCurve.next != nullptr))
            return GL_FALSE;

        ArcAttributes newCurveObj = ArcAttributes();
        _attributes.push_back(newCurveObj);
        ArcAttributes& newCurve = _attributes.back();

        SecondOrderHyperbolicArc* arc = new (nothrow) SecondOrderHyperbolicArc(_alpha);
        if (!arc)
        {
            throw Exception("Could not create the second order hyperbolic arc!");
        }

        switch (direction)
        {
            case LEFT:
                (*arc)[0] = (*existingCurve.arc)[0];
                (*arc)[1] = 2 * (*existingCurve.arc)[0] - (*existingCurve.arc)[1];
                (*arc)[2] = 2 * (*arc)[1] - (*existingCurve.arc)[0];
                (*arc)[3] = 2 * (*arc)[2] - (*arc)[1];

                existingCurve.previous = &newCurve;
                newCurve.next = &existingCurve;
                break;
            case RIGHT:
                (*arc)[0] = (*existingCurve.arc)[3];
                (*arc)[1] = 2 * (*existingCurve.arc)[3] - (*existingCurve.arc)[2];
                (*arc)[2] = 2 * (*arc)[1] - (*existingCurve.arc)[3];
                (*arc)[3] = 2 * (*arc)[2] - (*arc)[1];

                existingCurve.next = &newCurve;
                newCurve.previous = &existingCurve;
        }

        newCurve.arc = new SecondOrderHyperbolicArc(*arc);
        newCurve.image = newCurve.arc->GenerateImage(1, _div_point_count);
        newCurve.color = new Color4(*(existingCurve.color));

        if (!newCurve.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        if (!newCurve.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO's of the hyperbolic arc image!");
        }

        return GL_TRUE;
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::updateCurve(GLuint curveIndex, GLuint controlPointIndex, DCoordinate3 value)
    {
        if (curveIndex >= _attributes.size() || controlPointIndex > 3)
            return GL_FALSE;

        (*_attributes[curveIndex].arc)[controlPointIndex] = value;

        _attributes[curveIndex].image = _attributes[curveIndex].arc->GenerateImage(1, _div_point_count);

        if (!_attributes[curveIndex].arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBOoD's hyperbolic arc!");
        }

        if (!_attributes[curveIndex].image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO's of the hyperbolic arc image!");
        }

        return GL_TRUE;
    }

    SecondOrderHyperbolicArc* SecondOrderHyperbolicCompositeCurve::initCurve()
    {
        GLuint _n = 4; // 4 kontroll pont
        SecondOrderHyperbolicArc* arc = new (nothrow) SecondOrderHyperbolicArc(_alpha);

        if (!arc)
        {
            throw Exception("Could not create the second order hyperbolic arc!");
        }

        try
        {
            GLdouble step = TWO_PI / (_n);
            for (GLuint i = 0; i < _n; ++i)
            {
                GLdouble u = i * step;
                DCoordinate3 &cp = (*arc)[i]; // ez a p(i) vektor

                cp[0] = cos(u);
                cp[1] = sin(u);
                cp[2] = -2.0 + 4.0 * (GLdouble)rand()/RAND_MAX;// kesobb
            }

            if (!arc->UpdateVertexBufferObjectsOfData())
            {
                throw Exception("Could not update update the VBOs of the second order hyperbolic arc's control polygon");
            }

            return arc;
        }
        catch (Exception &e)
        {
            cout << e << endl;
        }

        return nullptr;
    }

    DCoordinate3 SecondOrderHyperbolicCompositeCurve::getPoint(GLuint curveIndex, GLuint controlPointIndex)
    {
        if (curveIndex >= _attributes.size() || controlPointIndex > 3)
        {
            throw Exception("Index out of bounds");
        }

        return (*_attributes[curveIndex].arc)[controlPointIndex];
    }

    GLboolean SecondOrderHyperbolicCompositeCurve::setSelected(GLuint index, GLboolean value)
    {
        if (index >= _attributes.size())
            return GL_FALSE;

        _attributes[index].selected = value;

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

    GLvoid SecondOrderHyperbolicCompositeCurve::setRenderControlPolygon(GLboolean value)
    {
        _renderControlPolygon = value;
    }

    GLvoid SecondOrderHyperbolicCompositeCurve::setRenderControlPoints(GLboolean value)
    {
        _renderControlPoints = value;
    }

    GLvoid SecondOrderHyperbolicCompositeCurve::setRenderFirstOrderDerivatives(GLboolean value)
    {
        _renderFirstOrderDerivatives = value;
    }
}

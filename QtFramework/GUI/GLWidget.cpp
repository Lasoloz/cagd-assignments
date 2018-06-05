#include "GLWidget.h"
#include "../Core/Constants.h"
#include "QColorDialog"
#include "QFileDialog"
#include <Core/Exceptions.h>
#include <GL/glu.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace cagd {
//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent, const QGLFormat &format)
    : QGLWidget(format, parent)
{
    _comp_curve          = 0;


    _firstOrderDerivativeEnabled = false;

    // On-screen helpers:
    _named_object_clicked = GL_FALSE;
    _reposition_unit      = 0.05;
    // Arc helpers:
    _primitiveIndex = _controlPointIndex = 1024;    // dummy value
    _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1; // dummy value
    _join = _merge = GL_FALSE;
}

GLWidget::~GLWidget() { releaseResources(); }

void GLWidget::releaseResources()
{
    if (_comp_curve) {
        delete _comp_curve;
        _comp_curve = 0;
    }
}

//--------------------------------------------------------------------------------------
// this virtual function is called once before the first call to paintGL() or
// resizeGL()
//--------------------------------------------------------------------------------------
void GLWidget::initializeGL()
{
    // creating a perspective projection matrix
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    _aspect = (float)width() / (float)height();
    _z_near = 1.0;
    _z_far  = 1000.0;
    _fovy   = 45.0;

    gluPerspective(_fovy, _aspect, _z_near, _z_far);

    // setting the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    _eye[0] = _eye[1] = 0.0, _eye[2] = 6.0;
    _center[0] = _center[1] = _center[2] = 0.0;
    _up[0] = _up[2] = 0.0, _up[1] = 1.0;

    gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2],
              _up[0], _up[1], _up[2]);

    // enabling depth test
    glEnable(GL_DEPTH_TEST);

    // setting the color of background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // initial values of transformation parameters
    _angle_x = _angle_y = _angle_z = 0.0;
    _trans_x = _trans_y = _trans_z = 0.0;
    _zoom                          = 1.0;

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_DEPTH_TEST);

    try {
        // initializing the OpenGL Extension Wrangler library
        GLenum error = glewInit();

        if (error != GLEW_OK) {
            throw Exception(
                "Could not initialize the OpenGL Extension Wrangler Library!");
        }

        if (!glewIsSupported("GL_VERSION_2_0")) {
            throw Exception(
                "Your graphics card is not compatible with OpenGL 2.0+! "
                "Try to update your driver or buy a new graphics adapter!");
        }

        // create and store your geometry in display lists or vertex buffer
        // objects
        // ...
        _comp_curve = new (nothrow) SecondOrderHyperbolicCompositeCurve(10);

//        _comp_curve->insertIsolatedArc();
//        _comp_curve->insertIsolatedArc();
//        _comp_curve->insertIsolatedArc();
//        _comp_curve->insertIsolatedArc();

//        ofstream ofile(std::string("saveTry.txt"));
//        ofile << *_comp_curve;

    } catch (Exception &e) {
        cout << e << endl;
    }
}


//-----------------------
// the rendering function
//-----------------------
void GLWidget::paintGL()
{
    // clears the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // stores/duplicates the original model view matrix
    glPushMatrix();

    // applying transformations
    glRotatef(_angle_x, 1.0, 0.0, 0.0);
    glRotatef(_angle_y, 0.0, 1.0, 0.0);
    glRotatef(_angle_z, 0.0, 0.0, 1.0);
    glTranslated(_trans_x, _trans_y, _trans_z);
    glScaled(_zoom, _zoom, _zoom);

    // render your geometry (this is oldest OpenGL rendering technique, later we
    // will use some advanced methods)

    _comp_curve->render();

    // pops the current matrix stack, replacing the current matrix with the one
    // below it on the stack, i.e., the original model view matrix is restored
    glPopMatrix();
}


//----------------------------------------------------------------------------
// when the main window is resized one needs to redefine the projection matrix
//----------------------------------------------------------------------------
void GLWidget::resizeGL(int w, int h)
{
    // setting the new size of the rendering context
    glViewport(0, 0, w, h);

    // redefining the projection matrix
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    _aspect = (float)w / (float)h;

    gluPerspective(_fovy, _aspect, _z_near, _z_far);

    // switching back to the model view matrix
    glMatrixMode(GL_MODELVIEW);

    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();

    if (event->button() == Qt::LeftButton) {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLuint  size        = 4 * (4 * _comp_curve->getCurveCount());
        GLuint *pick_buffer = new GLuint[size];
        glSelectBuffer(size, pick_buffer);

        glRenderMode(GL_SELECT);

        glInitNames();
        glPushName(0);

        GLfloat projection_matrix[16];
        glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);

        glMatrixMode(GL_PROJECTION);

        glPushMatrix();

        glLoadIdentity();
        gluPickMatrix((GLdouble)event->x(),
                      (GLdouble)(viewport[3] - event->y()), 5.0, 5.0, viewport);

        glMultMatrixf(projection_matrix);

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        // rotating around the coordinate axes
        glRotatef(_angle_x, 1.0, 0.0, 0.0);
        glRotatef(_angle_y, 0.0, 1.0, 0.0);
        glRotatef(_angle_z, 0.0, 0.0, 1.0);

        // translate
        glTranslated(_trans_x, _trans_y, _trans_z);

        // scaling
        glScalef(_zoom, _zoom, _zoom);

        // render only the clickable geometries
        _comp_curve->renderClickable(true);

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

        GLint hit_count = glRenderMode(GL_RENDER);

        if (hit_count) {
            GLuint closest_selected = pick_buffer[3];
            GLuint closest_depth    = pick_buffer[1];


            for (GLint i = 1; i < hit_count; ++i) {
                GLuint offset = i * 4;
                if (pick_buffer[offset + 1] < closest_depth) {
                    closest_selected = pick_buffer[offset + 3];
                    closest_depth    = pick_buffer[offset + 1];
                }
            }

            _comp_curve->setSelected(_primitiveIndex, _controlPointIndex, GL_FALSE);
//            GLuint curveCount = _comp_curve->getCurveCount() * 4;
//            if (closest_selected < curveCount) {
                _primitiveIndex    = closest_selected / 4;
                _controlPointIndex = closest_selected % 4;
//            } else {
//                _primitiveIndex    = (closest_selected - curveCount) / 16;
//                _controlPointIndex = (closest_selected - curveCount) % 16;
//            }

//            cout << "patch index: " << _primitiveIndex <<
//                    "control pont idex: " << _controlPointIndex << endl;
            joinAndMergeHelper();

            _comp_curve->setSelected(_primitiveIndex, _controlPointIndex, GL_TRUE);

            _named_object_clicked = GL_TRUE;
        } else {
            _join = _merge = GL_FALSE;
            _comp_curve->setSelected(_primitiveIndex, _controlPointIndex, GL_FALSE);
            _primitiveIndex                  = _comp_curve->getCurveCount() + 1;
            _named_object_clicked = GL_FALSE;
        }

        delete pick_buffer;

        updateGL();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    event->accept();

    if (_named_object_clicked) {
        DCoordinate3 point = _comp_curve->getPoint(_primitiveIndex, _controlPointIndex);
        GLdouble &   x     = point[0];
        GLdouble &   y     = point[1];
        GLdouble &   z     = point[2];

        // wheel + Ctrl
        if (event->modifiers() & Qt::ControlModifier) {
            x += event->delta() / 120.0 * _reposition_unit;
        }

        // wheel + Alt
        if (event->modifiers() & Qt::AltModifier) {
            y += event->delta() / 120.0 * _reposition_unit;
        }

        if (event->modifiers() & Qt::ShiftModifier) {
            z += event->delta() / 120.0 * _reposition_unit;
        }

        _comp_curve->updateCurve(_primitiveIndex, _controlPointIndex, point);

        updateGL();
    }
}

GLvoid GLWidget::joinAndMergeHelper()
{
    if (_join || _merge) {
        if (_arc1[1] != -1) {
            _arc2[0] = _primitiveIndex;
            _arc2[1] = _controlPointIndex;

            if ((_arc1[1] == 0 || _arc1[1] == 3) &&
                (_arc2[1] == 0 || _arc2[1] == 3)) {
                SecondOrderHyperbolicCompositeCurve::Direction dir1, dir2;

                if (_arc1[1] == 0)
                    dir1 = SecondOrderHyperbolicCompositeCurve::Direction::LEFT;
                else if (_arc1[1] == 3)
                    dir1 =
                        SecondOrderHyperbolicCompositeCurve::Direction::RIGHT;

                if (_arc2[1] == 0)
                    dir2 = SecondOrderHyperbolicCompositeCurve::Direction::LEFT;
                else if (_arc2[1] == 3)
                    dir2 =
                        SecondOrderHyperbolicCompositeCurve::Direction::RIGHT;

                _join ? _comp_curve->join(_arc1[0], dir1, _arc2[0], dir2)
                      : _comp_curve->merge(_arc1[0], dir1, _arc2[0], dir2);
            }

            _join = _merge = GL_FALSE;
            _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
        } else {
            _arc1[0] = _primitiveIndex;
            _arc1[1] = _controlPointIndex;
        }
    }
}

//-----------------------------------
// implementation of the public slots
//-----------------------------------

void GLWidget::set_angle_x(int value)
{
    if (_angle_x != value) {
        _angle_x = value;
        updateGL();
    }
}

void GLWidget::set_angle_y(int value)
{
    if (_angle_y != value) {
        _angle_y = value;
        updateGL();
    }
}

void GLWidget::set_angle_z(int value)
{
    if (_angle_z != value) {
        _angle_z = value;
        updateGL();
    }
}

void GLWidget::set_zoom_factor(double value)
{
    if (_zoom != value) {
        _zoom = value;
        updateGL();
    }
}

void GLWidget::set_trans_x(double value)
{
    if (_trans_x != value) {
        _trans_x = value;
        updateGL();
    }
}

void GLWidget::set_trans_y(double value)
{
    if (_trans_y != value) {
        _trans_y = value;
        updateGL();
    }
}

void GLWidget::set_trans_z(double value)
{
    if (_trans_z != value) {
        _trans_z = value;
        updateGL();
    }
}

void GLWidget::set_firstOrderDerivative(bool value)
{
    _comp_curve->setRenderFirstOrderDerivatives(value);
    updateGL();
}

void GLWidget::set_control_points(bool value)
{
    _comp_curve->setRenderControlPoints(value);
    updateGL();
}

void GLWidget::set_control_polygon(bool value)
{
    _comp_curve->setRenderControlPolygon(value);
    updateGL();
}

void GLWidget::set_curve_image(bool value)
{
    _comp_curve->setRenderCurve(value);
    updateGL();
}

void GLWidget::insert_isolated_arc()
{
    _comp_curve->insertIsolatedArc();
    updateGL();
}

void GLWidget::remove_arc()
{
    _comp_curve->erease(_primitiveIndex);
    _primitiveIndex = _comp_curve->getCurveCount() + 1;
    updateGL();
}

void GLWidget::continue_arc()
{
    if (_controlPointIndex == 3 || _controlPointIndex == 0) {
        _comp_curve->continueExistingArc(
            _primitiveIndex, _controlPointIndex == 3
                      ? SecondOrderHyperbolicCompositeCurve::Direction::RIGHT
                      : SecondOrderHyperbolicCompositeCurve::Direction::LEFT);
        updateGL();
    }
}

void GLWidget::change_selected_color()
{
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid())
        _comp_curve->setSelectedColor(color.redF(), color.greenF(),
                                     color.blueF(), color.alphaF());
    updateGL();
}

void GLWidget::change_selected_arcs_color()
{
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid())
        _comp_curve->setCurveColor(_primitiveIndex, _controlPointIndex, color.redF(), color.greenF(),
                                  color.blueF(), color.alphaF());
    updateGL();
}

void GLWidget::join_arcs()
{
    _merge   = GL_FALSE;
    _join    = GL_TRUE;
    _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
}

void GLWidget::merge_arcs()
{
    _join    = GL_FALSE;
    _merge   = GL_TRUE;
    _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
}

void GLWidget::save()
{
    QString fileName = QFileDialog::getSaveFileName(
                        this,
                        tr("Save"),
                        "./Models/",
                        "OFF File (*.OUR_AWESOME_FORMAT_FOR_THE_FUCKING_PROJECT)");
    cout << fileName.toStdString() << endl;

    ofstream ofile(std::string(fileName.toStdString()));
    if (ofile.good()) {
        ofile << *_comp_curve;
    }
}

void GLWidget::load()
{
    QString fileName = QFileDialog::getOpenFileName(
                            this,
                            tr("Open"),
                            "./Models/",
                            "OFF File (*.OUR_AWESOME_FORMAT_FOR_THE_FUCKING_PROJECT)");
    cout << fileName.toStdString() << endl;

    fstream f(fileName.toStdString(), ios_base::in);
    if (f.good()) {
        f >> *_comp_curve;
    }
}
} // namespace cagd

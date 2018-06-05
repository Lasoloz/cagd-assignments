#include "GLWidget.h"
#include "../Core/Constants.h"
#include "QColorDialog"
#include "QFileDialog"
#include <Core/Exceptions.h>
#include <GL/glu.h>

#include <iostream>
#include <string>

using namespace std;

namespace cagd {
//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent, const QGLFormat &format)
    : QGLWidget(format, parent)
{
    _composite          = 0;


    _firstOrderDerivativeEnabled = false;

    // On-screen helpers:
    _named_object_clicked = GL_FALSE;
    _reposition_unit      = 0.05;
    // Arc helpers:
    _row = _column = 1024;                          // dummy value
    _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1; // dummy value
    _join = _merge = GL_FALSE;
}

GLWidget::~GLWidget() { releaseResources(); }

void GLWidget::releaseResources()
{
    if (_composite) {
        delete _composite;
        _composite = 0;
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
        _composite = new (nothrow) SecondOrderHyperbolicCompositeCurve(10);

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


    _composite->render();
    //            if (_interpolating_cyclic_curve || _cyclic_curve)
    //            {
    //                paintCyclicCurve();
    //            }
    //            else
    //            {
    //                if (_off_model)
    //                {
    //                    paintModel();
    //                }
    //                else
    //                {
    //                    if (_surfaceSelected)
    //                    {
    //                        paintHyperbolicSurface();
    //                    }
    //                }
    //            }


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

        GLuint  size        = 4 * 4 * _composite->getCurveCount();
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
        _composite->renderClickable(true);

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

            _composite->setSelected(_row, _column, GL_FALSE);

            _row    = closest_selected / 4;
            _column = closest_selected % 4;

            joinAndMergeHelper();

            _composite->setSelected(_row, _column, GL_TRUE);

            _named_object_clicked = GL_TRUE;
        } else {
            _join = _merge = GL_FALSE;
            _composite->setSelected(_row, _column, GL_FALSE);
            _row                  = _composite->getCurveCount() + 1;
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
        DCoordinate3 point = _composite->getPoint(_row, _column);
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

        _composite->updateCurve(_row, _column, point);

        updateGL();
    }
}

GLvoid GLWidget::joinAndMergeHelper()
{
    if (_join || _merge) {
        if (_arc1[1] != -1) {
            _arc2[0] = _row;
            _arc2[1] = _column;

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

                _join ? _composite->join(_arc1[0], dir1, _arc2[0], dir2)
                      : _composite->merge(_arc1[0], dir1, _arc2[0], dir2);
            }

            _join = _merge = GL_FALSE;
            _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
        } else {
            _arc1[0] = _row;
            _arc1[1] = _column;
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
    _composite->setRenderFirstOrderDerivatives(value);
    updateGL();
}

void GLWidget::set_control_points(bool value)
{
    _composite->setRenderControlPoints(value);
    updateGL();
}

void GLWidget::set_control_polygon(bool value)
{
    _composite->setRenderControlPolygon(value);
    updateGL();
}

void GLWidget::set_curve_image(bool value)
{
    _composite->setRenderCurve(value);
    updateGL();
}


void GLWidget::insert_isolated_arc()
{
    _composite->insertIsolatedArc();
    updateGL();
}

void GLWidget::remove_arc()
{
    _composite->erease(_row);
    _row = _composite->getCurveCount() + 1;
    updateGL();
}

void GLWidget::continue_arc()
{
    if (_column == 3 || _column == 0) {
        _composite->continueExistingArc(
            _row, _column == 3
                      ? SecondOrderHyperbolicCompositeCurve::Direction::RIGHT
                      : SecondOrderHyperbolicCompositeCurve::Direction::LEFT);
        updateGL();
    }
}

void GLWidget::change_selected_color()
{
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid())
        _composite->setSelectedColor(color.redF(), color.greenF(),
                                     color.blueF(), color.alphaF());
    updateGL();
}

void GLWidget::change_selected_arcs_color()
{
    QColor color = QColorDialog::getColor(Qt::red, this);
    if (color.isValid())
        _composite->setCurveColor(_row, _column, color.redF(), color.greenF(),
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
} // namespace cagd

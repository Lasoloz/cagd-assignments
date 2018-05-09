#include "GLWidget.h"
#include <GL/glu.h>

#include <iostream>
using namespace std;

#include <Core/Exceptions.h>

namespace cagd {
//--------------------------------
// special and default constructor
//--------------------------------
GLWidget::GLWidget(QWidget *parent, const QGLFormat &format)
    : QGLWidget(format, parent)
    , patch(1)
    , interpolatedPatch(1)
    , u_isoparam_curves(nullptr)
    , v_isoparam_curves(nullptr)
    , _show_original(true)
    , _show_interpolated(true)
{
    surface             = nullptr;
    interpolatedSurface = nullptr;
}

// ----------
// Destructor
// ----------
GLWidget::~GLWidget()
{
    if (surface) {
        delete surface;
    }

    if (interpolatedSurface) {
        delete interpolatedSurface;
    }

    if (u_isoparam_curves) {
        for (GLuint i = 0; i < u_isoparam_curves->GetColumnCount(); ++i) {
            delete (*u_isoparam_curves)(i);
        }

        delete u_isoparam_curves;
    }

    if (v_isoparam_curves) {
        for (GLuint i = 0; i < v_isoparam_curves->GetColumnCount(); ++i) {
            delete (*v_isoparam_curves)(i);
        }

        delete v_isoparam_curves;
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


        //        SecondOrderHyperbolicPatch patch(1);
        Matrix<DCoordinate3> data_points_to_interpolate =
            Matrix<DCoordinate3>(4, 4);
        RowMatrix<GLdouble>    u_knots = RowMatrix<GLdouble>(4);
        ColumnMatrix<GLdouble> v_knots = ColumnMatrix<GLdouble>(4);
        for (GLuint i = 0; i < 4; ++i) {
            u_knots(i) = v_knots(i) = (GLdouble)i / 3.0;
            for (GLuint j = 0; j < 4; ++j) {
                GLdouble     r = (GLdouble)i + 1.0;
                GLdouble     p = (GLdouble)j / 3.0 * PI;
                DCoordinate3 current =
                    DCoordinate3(r * cos(p), r * sin(p), sin(i + j * j * i));
                //                    DCoordinate3(i - 1.5, j - 1.5, cos(i - j *
                //                    i * i));
                patch.SetData(i, j, current);

                data_points_to_interpolate(i, j) = current;
            }
        }

        if (!interpolatedPatch.UpdateDataForInterpolation(
                u_knots, v_knots, data_points_to_interpolate)) {
            throw Exception("Failed to interpolate control net!");
        }

        if (!interpolatedPatch.UpdateVertexBufferObjectsOfData()) {
            throw Exception(
                "Failed to generate interpolated surface's control net's VBO!");
        }

        if (!patch.UpdateVertexBufferObjectsOfData()) {
            throw Exception("Failed to generate surface's control net's VBO");
        }

        surface             = patch.GenerateImage(100, 100);
        interpolatedSurface = interpolatedPatch.GenerateImage(100, 100);

        if (!surface) {
            throw Exception("Failed to generate surface's image");
        }

        if (!interpolatedSurface) {
            throw Exception("Failed to generate interpolated surface's image");
        }

        if (!surface->UpdateVertexBufferObjects()) {
            throw Exception("Failed to initialize surface's image's VBO!");
        }

        if (!interpolatedSurface->UpdateVertexBufferObjects()) {
            throw Exception(
                "Failed to initialize intepolated surface's image's VBO");
        }

        if (!shaderProgram.InstallShaders("Shaders/two_sided_lighting.vert",
                                          "Shaders/two_sided_lighting.frag",
                                          true)) {
            throw Exception("Failed to install shader program!");
        }


        // Isoparametric curves:
        u_isoparam_curves = patch.GenerateUIsoparametricLines(4, 1, 100);
        if (!u_isoparam_curves) {
            throw Exception("Failed to create U-isoparametric lines");
        }

        for (GLuint i = 0; i < u_isoparam_curves->GetColumnCount(); ++i) {
            if (!(*u_isoparam_curves)(i)->UpdateVertexBufferObjects()) {
                throw Exception("Failed to update VBO for isoparam. line");
            }
        }
        v_isoparam_curves = patch.GenerateVIsoparametricLines(4, 1, 100);
        if (!v_isoparam_curves) {
            throw Exception("Failed to create V-isoparametric lines");
        }

        for (GLuint i = 0; i < v_isoparam_curves->GetColumnCount(); ++i) {
            if (!(*v_isoparam_curves)(i)->UpdateVertexBufferObjects()) {
                throw Exception("Failed to update VBO for isoparam. line");
            }
        }

        transparentEmerald = MatFBEmerald;
        transparentEmerald.SetTransparency(0.6f);
    } catch (Exception &e) {
        cout << e << endl;
    }

    // Special lighting options:
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
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
    shaderProgram.Enable();

    // First surface:
    if (_show_original) {
        MatFBBrass.Apply();
        surface->Render();
    }

    // Interpolated surface:
    if (_show_interpolated) {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        transparentEmerald.Apply();
        interpolatedSurface->Render();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    shaderProgram.Disable();

    glLineWidth(2.f);
    // Isoparametric lines:
    if (_show_original) {
        glColor3f(0.f, 4.f, 1.f);
        for (GLuint i = 0; i < u_isoparam_curves->GetColumnCount(); ++i) {
            (*u_isoparam_curves)(i)->RenderDerivatives(0, GL_LINE_STRIP);
        }

        for (GLuint i = 0; i < v_isoparam_curves->GetColumnCount(); ++i) {
            (*v_isoparam_curves)(i)->RenderDerivatives(0, GL_LINE_STRIP);
        }
    }
    // Control net for first surface:
    glColor3f(1.f, 0.f, 0.f);
    patch.RenderData();
    glLineWidth(1.f);


    // pops the current matrix stack, replacing the current matrix with the one
    // below it on the stack, i.e., the original model view matrix is restored
    glPopMatrix();
}



// My methods:
// private:



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



void GLWidget::set_show_original_state(bool state)
{
    _show_original = state;
    updateGL();
}

void GLWidget::set_show_interpolated_state(bool state)
{
    _show_interpolated = state;
    updateGL();
}

} // namespace cagd

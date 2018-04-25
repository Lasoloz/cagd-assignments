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
    , _current_mesh(nullptr)
    , _parametric_surface(nullptr)
{}

// ----------
// Destructor
// ----------
GLWidget::~GLWidget()
{
    _free_mesh();
    _free_surface();
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

        // create and store your geometry in display lists or vertex buffer
        // objects
        QString meshName = "mouse";
        _set_current_mesh(meshName);
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
    glEnable(GL_LIGHTING);
    MatFBTurquoise.Apply();
    _current_mesh->Render();
    glDisable(GL_LIGHTING);


    // pops the current matrix stack, replacing the current matrix with the one
    // below it on the stack, i.e., the original model view matrix is restored
    glPopMatrix();
}



// My methods:
// private:
void GLWidget::_free_mesh()
{
    if (_current_mesh) {
        delete _current_mesh;
        _current_mesh = nullptr;
    }
}

void GLWidget::_free_surface()
{
    if (_parametric_surface) {
        delete _parametric_surface;
        _parametric_surface = nullptr;
    }
}

void GLWidget::_set_current_mesh(QString &mesh_name)
{
    if (_current_mesh_name == mesh_name) {
        return;
    }

    TriangulatedMesh3 *mesh = new (nothrow) TriangulatedMesh3();
    if (!mesh) {
        throw Exception("Failed to create triangular mesh");
    }

    std::string offFileName = "Models/" + mesh_name.toStdString() + ".off";

    if (!mesh->LoadFromOFF(offFileName, true)) {
        delete mesh;
        throw Exception("Failed to load off file: " + offFileName);
    }

    if (!mesh->UpdateVertexBufferObjects()) {
        delete mesh;
        throw Exception("Failed to update VBO for mesh");
    }

    _free_mesh();
    _current_mesh            = mesh;
    _current_mesh_name       = mesh_name;
    _parametric_surface_name = "";
    _is_mesh                 = true;
    updateGL();
}

void GLWidget::_set_current_parametric(QString &surface_name)
{
    if (_parametric_surface_name == surface_name) {
        return;
    }

    TriangularMatrix<ParametricSurface3::PartialDerivative> pd(2);
    GLdouble umin, umax, vmin, vmax;
    GLuint   uDivCount, vDivCount;
    if (surface_name == hyperboloid::surface_name) {
        pd(0, 0)  = hyperboloid::d00;
        pd(1, 0)  = hyperboloid::d01;
        pd(1, 1)  = hyperboloid::d10;
        umin      = hyperboloid::u_min;
        umax      = hyperboloid::u_max;
        vmin      = hyperboloid::v_min;
        vmax      = hyperboloid::v_max;
        uDivCount = hyperboloid::u_div_count;
        vDivCount = hyperboloid::v_div_count;
    } else if (surface_name == sphere::surface_name) {
        pd(0, 0)  = sphere::d00;
        pd(1, 0)  = sphere::d01;
        pd(1, 1)  = sphere::d10;
        umin      = sphere::u_min;
        umax      = sphere::u_max;
        vmin      = sphere::v_min;
        vmax      = sphere::v_max;
        uDivCount = sphere::u_div_count;
        vDivCount = sphere::v_div_count;
    } else if (surface_name == seashell::surface_name) {
        pd(0, 0)  = seashell::d00;
        pd(1, 0)  = seashell::d01;
        pd(1, 1)  = seashell::d10;
        umin      = seashell::u_min;
        umax      = seashell::u_max;
        vmin      = seashell::v_min;
        vmax      = seashell::v_max;
        uDivCount = seashell::u_div_count;
        vDivCount = seashell::v_div_count;
    } else if (surface_name == moebius::surface_name) {
        pd(0, 0)  = moebius::d00;
        pd(1, 0)  = moebius::d01;
        pd(1, 1)  = moebius::d10;
        umin      = moebius::u_min;
        umax      = moebius::u_max;
        vmin      = moebius::v_min;
        vmax      = moebius::v_max;
        uDivCount = moebius::u_div_count;
        vDivCount = moebius::v_div_count;
    } else if (surface_name == klein_bootle::surface_name) {
        pd(0, 0)  = klein_bootle::d00;
        pd(1, 0)  = klein_bootle::d10;
        pd(1, 1)  = klein_bootle::d01;
        umin      = klein_bootle::u_min;
        umax      = klein_bootle::u_max;
        vmin      = klein_bootle::v_min;
        vmax      = klein_bootle::v_max;
        uDivCount = klein_bootle::u_div_count;
        vDivCount = klein_bootle::v_div_count;
    } else {
        throw Exception("Invalid parametric surface name");
    }

    ParametricSurface3 *surf =
        new (nothrow) ParametricSurface3(pd, umin, umax, vmin, vmax);

    if (!surf) {
        throw Exception("Failed to create new parametric surface");
    }

    TriangulatedMesh3 *mesh = surf->GenerateImage(uDivCount, vDivCount);
    if (!mesh) {
        delete surf;
        throw Exception("Failed to create the image of the surface");
    }

    if (!mesh->UpdateVertexBufferObjects()) {
        delete mesh;
        delete surf;
        throw Exception("Failed to update VBO for surface image");
    }

    _free_mesh();
    _free_surface();

    _current_mesh            = mesh;
    _parametric_surface      = surf;
    _current_mesh_name       = "";
    _parametric_surface_name = surface_name;
    _is_mesh                 = false;
    updateGL();
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


void GLWidget::set_mesh(QString mesh_name)
{
    try {
        _set_current_mesh(mesh_name);
    } catch (Exception &ex) {
        std::cerr << "Failed to change mesh: " << ex << '\n';
    }
}

void GLWidget::set_parametric(QString surface_name)
{
    try {
        _set_current_parametric(surface_name);
    } catch (Exception &ex) {
        std::cerr << "Failed to change to parametric: " << ex << '\n';
    }
}

} // namespace cagd

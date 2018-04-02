#include "GLWidget.h"
#include <GL/glu.h>

#include <iostream>
using namespace std;

#include <Core/Exceptions.h>

namespace cagd
{
    //--------------------------------
    // special and default constructor
    //--------------------------------
    GLWidget::GLWidget(QWidget *parent, const QGLFormat &format): QGLWidget(format, parent)
    {
        _parametric_curve = nullptr;
        _curve_img = nullptr;
        _show_curve = true;
        _show_1st_deriv = _show_2nd_deriv = false;
        _cyclic_curve = nullptr;
    }

    // ----------
    // Destructor
    // ----------
    GLWidget::~GLWidget() {
        clearParametricCurve();
    }


    // Clear up curve resources:
    void GLWidget::clearCurveImg() {
        if (_curve_img != nullptr) {
            delete _curve_img;
            _curve_img = nullptr;
        }
    }
    void GLWidget::clearParametricCurve() {
        if (_parametric_curve != nullptr) {
            delete _parametric_curve;
            _parametric_curve = nullptr;
        }
    }
    void GLWidget::clearCyclicCurve() {
        if (_cyclic_curve != nullptr) {
            delete _cyclic_curve;
            _cyclic_curve = nullptr;
        }
    }

    //--------------------------------------------------------------------------------------
    // this virtual function is called once before the first call to paintGL() or resizeGL()
    //--------------------------------------------------------------------------------------
    void GLWidget::initializeGL()
    {
        // creating a perspective projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = (float)width() / (float)height();
        _z_near = 1.0;
        _z_far = 1000.0;
        _fovy = 45.0;

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // setting the model view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        _eye[0] = _eye[1] = 0.0, _eye[2] = 6.0;
        _center[0] = _center[1] = _center[2] = 0.0;
        _up[0] = _up[2] = 0.0, _up[1] = 1.0;

        gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2], _up[0], _up[1], _up[2]);

        // enabling depth test
        glEnable(GL_DEPTH_TEST);

        // setting the color of background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // initial values of transformation parameters
        _angle_x = _angle_y = _angle_z = 0.0;
        _trans_x = _trans_y = _trans_z = 0.0;
        _zoom = 1.0;

        try
        {
            // initializing the OpenGL Extension Wrangler library
            GLenum error = glewInit();

            if (error != GLEW_OK)
            {
                throw Exception("Could not initialize the OpenGL Extension Wrangler Library!");
            }

            if (!glewIsSupported("GL_VERSION_2_0"))
            {
                throw Exception("Your graphics card is not compatible with OpenGL 2.0+! "
                                "Try to update your driver or buy a new graphics adapter!");
            }

            // create and store your geometry in display lists or vertex buffer objects
            // lab02 - parametric curves:
            set_curve_type(spiral_on_cone::curve_name);
        }
        catch (Exception &e)
        {
            cout << e << endl;
        }
    }


    void GLWidget::setParametricCurve(QString &curve_name) {
        // Set up new curve type:
        RowMatrix<ParametricCurve3::Derivative> derivatives;
        GLdouble u_min = 0, u_max = 0;
        derivatives.ResizeColumns(3);
        int divs = 100;
        if (curve_name == spiral_on_cone::curve_name) {
            derivatives[0] = spiral_on_cone::d0;
            derivatives[1] = spiral_on_cone::d1;
            derivatives[2] = spiral_on_cone::d2;
            u_min = spiral_on_cone::u_min;
            u_max = spiral_on_cone::u_max;
            divs = spiral_on_cone::div;
        } else if (curve_name == cochleoid::curve_name) {
            derivatives[0] = cochleoid::d0;
            derivatives[1] = cochleoid::d1;
            derivatives[2] = cochleoid::d2;
            u_min = cochleoid::u_min;
            u_max = cochleoid::u_max;
            divs = cochleoid::div;
        } else if (curve_name == epicycloid::curve_name) {
            derivatives[0] = epicycloid::d0;
            derivatives[1] = epicycloid::d1;
            derivatives[2] = epicycloid::d2;
            u_min = epicycloid::u_min;
            u_max = epicycloid::u_max;
            divs = epicycloid::div;
        } else if (curve_name == viviani::curve_name) {
            derivatives[0] = viviani::d0;
            derivatives[1] = viviani::d1;
            derivatives[2] = viviani::d2;
            u_min = viviani::u_min;
            u_max = viviani::u_max;
            divs = viviani::div;
        } else if (curve_name == loxodrome::curve_name) {
            derivatives[0] = loxodrome::d0;
            derivatives[1] = loxodrome::d1;
            derivatives[2] = loxodrome::d2;
            u_min = loxodrome::u_min;
            u_max = loxodrome::u_max;
            divs = loxodrome::div;
        } else if (curve_name == fermat::curve_name) {
            derivatives[0] = fermat::d0;
            derivatives[1] = fermat::d1;
            derivatives[2] = fermat::d2;
            u_min = fermat::u_min;
            u_max = fermat::u_max;
            divs = fermat::div;
        } else {
            throw Exception("Invalid curve name!");
        }

        // Create new curve:
        ParametricCurve3 *newCurve = nullptr;
        newCurve = new(nothrow) ParametricCurve3(derivatives, u_min, u_max);

        if (newCurve == nullptr) {
            throw Exception("Couldn't create parametric curve!");
        }

        // Create new curve's image:
        GenericCurve3 *newCurveImg = nullptr;
        newCurveImg = newCurve->GenerateImage(divs);

        if (newCurveImg == nullptr) {
            delete newCurve;
            throw Exception("Couldn't create the image of the parametric curve!");
        }

        if (!newCurveImg->UpdateVertexBufferObjects()) {
            delete newCurveImg;
            delete newCurve;
            throw Exception("Couldn't generate the VBO of the parametric curve!");
        }

        // Delete old curve, and set to the new:
        clearParametricCurve();
        clearCurveImg();
        _parametric_curve = newCurve;
        _curve_img = newCurveImg;
        _curve_name = curve_name;

        parametricCurveType = true;
        updateGL();
    }


    CyclicCurve3 * GLWidget::createCyclicCurve(GLuint n) {
        CyclicCurve3 * result = new CyclicCurve3(n);
        if (result == nullptr) {
            throw Exception("Couldn't create cyclic curve!");
        }

        const GLuint nn = 2 * n + 1;
        // Generate knot vector and control points:
        GLdouble step = TWO_PI / nn;
        ColumnMatrix<DCoordinate3> dataPointsToInterpolate(nn);
        for (GLuint i = 0; i < nn; ++i)
        {
            GLdouble u = i * step;
            DCoordinate3 & cp = (*result)[i];

            cp[0] = cos (u);
            cp[1] = sin (u);
            cp[2] = -2 + 4 * (GLdouble)rand () / RAND_MAX;
        }

        result->SetDefinitionDomain(0, TWO_PI);

        // Update VBO:
        if (!result->UpdateVertexBufferObjectsOfData()) {
            delete result;
            throw Exception("Couldn't update VBO of cyclic curve!");
        }

        return result;
    }



    CyclicCurve3 * GLWidget::createInterpolatedCyclicCurve(GLuint n) {
        std::cerr << "Itt vagyok!\n";
        CyclicCurve3 * result = new CyclicCurve3(n);
        if (result == nullptr) {
            throw Exception("Couldn't create cyclic curve!");
        }

        const GLuint nn = 2 * n + 1;
        // Generate knot vector and control points:
        GLdouble step = TWO_PI / nn;
        ColumnMatrix<GLdouble> knotVector(nn);
        ColumnMatrix<DCoordinate3> dataPointsToInterpolate(nn);
        for (GLuint i = 0; i < nn; ++i)
        {
            GLdouble u = i * step;
            knotVector[i] = u;
            DCoordinate3 cp;

            cp[0] = cos (u);
            cp[1] = sin (u);
            cp[2] = -2 + 4 * (GLdouble)rand () / RAND_MAX;

            dataPointsToInterpolate[i] = cp;
        }

        result->SetDefinitionDomain(0, TWO_PI);

        // Update data with interpolation:
        if (!result->UpdateDataForInterpolation(knotVector, dataPointsToInterpolate)) {
            delete result;
            throw Exception("Couldn't update data of cyclic curve!");
        }
        // Update VBO:
        if (!result->UpdateVertexBufferObjectsOfData()) {
            delete result;
            throw Exception("Couldn't update VBO of cyclic curve!");
        }

        return result;
    }

    void GLWidget::setCyclicCurve(CyclicCurve3 * newCurve) {
        std::cerr << "Itt1!\n";
        GenericCurve3 * newCurveImg = newCurve->GenerateImage(2, 100);
        if (newCurveImg == nullptr) {
            delete newCurve;
            throw Exception("Couldn't generate image of cyclic curve!");
        }
        if (!newCurveImg->UpdateVertexBufferObjects()) {
            delete newCurveImg;
            delete newCurve;
            throw Exception("Couldn't generate VBO of cyclic curve's image!");
        }

        clearCyclicCurve();
        clearCurveImg();
        _cyclic_curve = newCurve;
        _curve_img = newCurveImg;

        parametricCurveType = false;
        updateGL();
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

            // render your geometry (this is oldest OpenGL rendering technique, later we will use some advanced methods)

//            glColor3f(1.0f, 1.0f, 1.0f);
//            glBegin(GL_LINES);
//                glVertex3f(0.0f, 0.0f, 0.0f);
//                glVertex3f(1.1f, 0.0f, 0.0f);

//                glVertex3f(0.0f, 0.0f, 0.0f);
//                glVertex3f(0.0f, 1.1f, 0.0f);

//                glVertex3f(0.0f, 0.0f, 0.0f);
//                glVertex3f(0.0f, 0.0f, 1.1f);
//            glEnd();

//            glBegin(GL_TRIANGLES);
//                // attributes
//                glColor3f(1.0f, 0.0f, 0.0f);
//                // associated with position
//                glVertex3f(1.0f, 0.0f, 0.0f);

//                // attributes
//                glColor3f(0.0, 1.0, 0.0);
//                // associated with position
//                glVertex3f(0.0, 1.0, 0.0);

//                // attributes
//                glColor3f(0.0f, 0.0f, 1.0f);
//                // associated with position
//                glVertex3f(0.0f, 0.0f, 1.0f);
//            glEnd();

            if (_show_curve) {
                glColor3f(.2f, 1.f, .2f);
                glLineWidth(4.f);
                _curve_img->RenderDerivatives(0, GL_LINE_STRIP);
                glLineWidth(1.f);
            }

            if (_show_1st_deriv) {
                glColor3f(.2f, .5f, .7f);
                _curve_img->RenderDerivatives(1, GL_LINES);
            }

            if (_show_2nd_deriv) {
                glColor3f(.2f, .2f, 1.f);
                _curve_img->RenderDerivatives(2, GL_LINES);
            }

            if (!parametricCurveType) {
                glColor3f(1.f, 1.f, .2f);
                glPointSize(10.f);
                _cyclic_curve->RenderData(GL_POINTS);
                glPointSize(1.f);
                _cyclic_curve->RenderData(GL_LINE_LOOP);
            }

        // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
        // i.e., the original model view matrix is restored
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

    //-----------------------------------
    // implementation of the public slots
    //-----------------------------------

    void GLWidget::set_angle_x(int value)
    {
        if (_angle_x != value)
        {
            _angle_x = value;
            updateGL();
        }
    }

    void GLWidget::set_angle_y(int value)
    {
        if (_angle_y != value)
        {
            _angle_y = value;
            updateGL();
        }
    }

    void GLWidget::set_angle_z(int value)
    {
        if (_angle_z != value)
        {
            _angle_z = value;
            updateGL();
        }
    }

    void GLWidget::set_zoom_factor(double value)
    {
        if (_zoom != value)
        {
            _zoom = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_x(double value)
    {
        if (_trans_x != value)
        {
            _trans_x = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_y(double value)
    {
        if (_trans_y != value)
        {
            _trans_y = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_z(double value)
    {
        if (_trans_z != value)
        {
            _trans_z = value;
            updateGL();
        }
    }


    void GLWidget::set_curve_type(QString curve_name) {
        try {
            std::cout << "Setting to `" << curve_name.toStdString() << "`...\n";
            // Check if old curve is the same or not
            if (curve_name == _curve_name) {
                return;
            }

            if (curve_name == cyclic::curve_name) {
                CyclicCurve3 * curve = createCyclicCurve(2);
                setCyclicCurve(curve);
                _curve_name = cyclic::curve_name;
            } else if (curve_name == cyclic_interpolation::curve_name) {
                CyclicCurve3 * curve = createInterpolatedCyclicCurve(2);
                setCyclicCurve(curve);
                _curve_name = cyclic_interpolation::curve_name;
            } else {
                setParametricCurve(curve_name);
            }
        } catch (Exception ex) {
            std::cerr << "Failed to create parametric or cyclic curve from selection!\n";
            std::cerr << "Exception: " << ex << '\n';
        }
    }

    void GLWidget::set_show_curve(bool state) {
        _show_curve = state;
        updateGL();
    }

    void GLWidget::set_show_1st_deriv(bool state) {
        _show_1st_deriv = state;
        updateGL();
    }

    void GLWidget::set_show_2nd_deriv(bool state) {
        _show_2nd_deriv = state;
        updateGL();
    }
}

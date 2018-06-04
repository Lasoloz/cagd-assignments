#include "GLWidget.h"
#include "../Core/Constants.h"
#include <GL/glu.h>
#include "QFileDialog"
#include "QColorDialog"
#include <Core/Exceptions.h>

#include <string>
#include <iostream>

using namespace std;

namespace cagd{
    //--------------------------------
    // special and default constructor
    //--------------------------------
    GLWidget::GLWidget(QWidget *parent, const QGLFormat &format): QGLWidget(format, parent)
    {
        _cc = 0;
        _img_cc = 0;
        _surface = 0;
        _surface_img = 0;
        _model = 0;
        _patch = 0;
        _interpolated_patch = 0;
        _u_dir = 0;
        _v_dir = 0;
        _composite = 0;

        _color[0] = _color[1] = _color[2] = _color[3] = 0.f;
        _shading = _smoothing = _scaleFactor = 0.1f;

        _firstOrderDerivativeEnabled = _secondOrderDerivativeEnabled = _control_polygon = false;
        _interpolating_cyclic_curve = _cyclic_curve = _off_model = false;
        _surfaceSelected = _grid = _mesh = _points = _interpolate = false;

        _named_object_clicked = GL_FALSE;
        _reposition_unit     = 0.05;
        _row = _column = 1024; // dummy value
        _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1; // dummy value
        _join = _merge = GL_FALSE;

        _isoLineCount = 10;
        _gridDivPointCount = 10;

        _timer = new QTimer(this);
        _timer->setInterval(0);
        connect(_timer, SIGNAL(timeout()), this, SLOT(animate()));
    }

    GLWidget::~GLWidget()
    {
        releaseResources();
    }

    void GLWidget::releaseResources() {
        if (_cc)
        {
            delete _cc;
            _cc = 0;
        }

        if (_img_cc)
        {
            delete _img_cc;
            _img_cc = 0;
        }

        if (_surface)
        {
            delete _surface;
            _surface = 0;
        }

        if (_surface_img)
        {
            delete _surface_img;
            _surface_img = 0;
        }

        if (_model)
        {
            _timer->stop();
            delete _model;
            _model = 0;
        }

        if (_u_dir)
        {
            for (GLuint i = 0; i < _u_dir->GetColumnCount(); ++i) {
                delete (*_u_dir)[i];
            }

            delete _u_dir;
            _u_dir = 0;
        }

        if (_v_dir)
        {
            for (GLuint i = 0; i < _v_dir->GetColumnCount(); ++i) {
                delete (*_v_dir)[i];
            }

            delete _v_dir;
            _v_dir = 0;
        }

        if (_patch)
        {
            delete _patch;
            _patch = 0;
        }

        if (_interpolated_patch)
        {
            delete _interpolated_patch;
            _interpolated_patch = 0;
        }

        if (_composite)
        {
            delete _composite;
            _composite = 0;
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

        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glEnable(GL_DEPTH_TEST);

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
            // ...
            _shader.InstallShaders("Shaders/two_sided_lighting.vert",
                                   "Shaders/two_sided_lighting.frag",
                                   GL_TRUE);

            _composite = new (nothrow)SecondOrderHyperbolicCompositeCurve(10);
//            _composite->insertIsolatedArc();
//            _composite->insertIsolatedArc();
            _composite->join(0, SecondOrderHyperbolicCompositeCurve::Direction::RIGHT, 1, SecondOrderHyperbolicCompositeCurve::Direction::RIGHT);
        }
        catch (Exception &e)
        {
            cout << e << endl;
        }
    }

    void GLWidget::initModel(const string &fileName)
    {
        releaseResources();
        _model = new (nothrow) TriangulatedMesh3;
        if (!_model) {
            throw Exception("Could not allocate memory for TriangulatedMesh3");
        }

        _angle = 0.0;

        if (!_model->LoadFromOFF(fileName, GL_TRUE))
        {
            throw Exception("Could not load model from OFF file.");
        }

        if (!_model->UpdateVertexBufferObjects())
        {
            throw Exception("Could now update VBO's of the model.");
        }

        _timer->start();
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


        // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
        // i.e., the original model view matrix is restored
        glPopMatrix();
    }

    void GLWidget::paintModel()
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHT0);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        MatFBEmerald.Apply();

//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        _shader.Enable();
        _model->Render(GL_TRIANGLES);
        _shader.Disable();

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
        glDisable(GL_LIGHTING);
    }

    void GLWidget::animate()
    {
        _angle += (GLfloat) DEG_TO_RADIAN;
        if (_angle >= TWO_PI)
        {
            _angle -= (GLfloat) TWO_PI;
        }

        GLdouble cool_off = 3000.0;
        GLdouble t = sin(_angle)/ cool_off;

        GLfloat* vertex = _model->MapVertexBuffer(GL_READ_WRITE);
        GLfloat* normal = _model->MapNormalBuffer(GL_READ_ONLY);

        if (vertex && normal)
        {
            for (GLuint i = 0; i < _model->VertexCount(); ++i)
            {
                for (GLuint j = 0; j < 3; ++j, vertex++, normal++)
                {
                    *vertex += t * (*normal);
                }
            }
        }

        _model->UnmapVertexBuffer();
        _model->UnmapNormalBuffer();

        updateGL();
    }

    void GLWidget::paintCyclicCurve()
    {
        if (_cc && _control_polygon)
        {
            glColor3f(1.f, 0.f, .0f);
            _cc->RenderData(GL_LINE_LOOP);
        }

        if (_img_cc)
        {
            glColor3f(.2f, 1.f, .2f);
            glLineWidth(4.f);
            _img_cc->RenderDerivatives(0, GL_LINE_LOOP);
            glLineWidth(1.f);

            if (_firstOrderDerivativeEnabled)
            {
                glColor3f(.2f, .5f, .7f);
                _img_cc->RenderDerivatives(1, GL_LINES);
            }

            if (_secondOrderDerivativeEnabled)
            {
                glColor3f(.2f, .2f, 1.f);
                _img_cc->RenderDerivatives(2, GL_LINES);
            }
        }
    }

    void GLWidget::paintHyperbolicSurface()
    {
        if (_patch && _mesh)
        {
            glColor3f(1.0f, 0.8f, 0.0f);
            _patch->RenderData();
        }

        if (_beforeInterpolation && _show_surface)
        {
            MatFBRuby.Apply();
            _shader.Enable();
            _beforeInterpolation->Render();
            _shader.Disable();
        }

        if (_grid)
        {
            if (_u_dir->GetColumnCount() != _isoLineCount)
            {
                if (_u_dir)
                {
                    for (GLuint i = 0; i < _u_dir->GetColumnCount(); ++i) {
                        delete (*_u_dir)[i];
                    }

                    delete _u_dir;
                }

                _u_dir = _patch->GenerateUIsoparametricLines(_isoLineCount, 1, _gridDivPointCount);

                for (GLuint i = 0; i < _u_dir->GetColumnCount(); ++i) {
                    (*_u_dir)[i]->UpdateVertexBufferObjects();
                }
            }

            if (_v_dir->GetColumnCount() != _isoLineCount) {
                if (_v_dir)
                {
                    for (GLuint i = 0; i < _v_dir->GetColumnCount(); ++i) {
                        delete (*_v_dir)[i];
                    }

                    delete _v_dir;
                }

                _v_dir = _patch->GenerateVIsoparametricLines(_isoLineCount, 1, _gridDivPointCount);

                for (GLuint i = 0; i < _v_dir->GetColumnCount(); ++i) {
                    (*_v_dir)[i]->UpdateVertexBufferObjects();
                }
            }


            for (GLuint j = 0; j < _u_dir->GetColumnCount(); ++j)
            {
                glColor3f(0.0f, 0.0f, 1.0f);
                (*_u_dir)[j]->RenderDerivatives(0, GL_LINE_STRIP);
                if (_firstOrderDerivativeEnabled)
                {
                    glColor3f(0.0f, 1.0f, 1.0f);
                    (*_u_dir)[j]->RenderDerivatives(1, GL_LINES);
                }
            }

            for (GLuint j = 0; j < _v_dir->GetColumnCount(); ++j)
            {
                glColor3f(0.0f, 0.0f, 1.0f);
                (*_v_dir)[j]->RenderDerivatives(0, GL_LINE_STRIP);
                if (_firstOrderDerivativeEnabled)
                {
                    glColor3f(0.0f, 1.0f, 1.0f);
                    (*_u_dir)[j]->RenderDerivatives(1, GL_LINES);
                }
            }
        }

        if (_afterInterpolation && _interpolate)
        {
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                MatFBTurquoise.Apply();
                _shader.Enable();
                _afterInterpolation->Render();
                _shader.Disable();
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        if (_points)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                for (GLuint j = 0; j < 4; ++j)
                {
                    DCoordinate3 &cp = (*_patch)(i, j);

                    glPushMatrix();
                        glTranslated(cp[0], cp[1], cp[2]);
                        glScaled(_radius, _radius, _radius);
                        MatFBBrass.Apply();
                        _shader.Enable();
                        _sphere.Render();
                        _shader.Disable();
                    glPopMatrix();
                }
            }
        }
    }

    void GLWidget::initHyperbolicSurface()
    {
        RowMatrix<GLdouble> u_knot_vector(4);
        ColumnMatrix<GLdouble> v_knot_vector(4);
        _patch = new SecondOrderHyperbolicPatch(1);

        for (GLuint i = 0; i < 4; ++i) {
            u_knot_vector(i) = v_knot_vector(i) = (GLdouble) i / 3.0;
            for (GLuint j = 0; j < 4; ++j) {
                GLdouble r = (GLdouble) i + 1.0;
                GLdouble p = (GLdouble) j / 3.0 * PI;
                _patch->SetData(i, j, DCoordinate3(r * cos(p), r * sin(p), sin(i + j * j * i)));
            }
        }

        if (!_patch->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update VBOoD's");
        }

        _beforeInterpolation = 0;
        _beforeInterpolation = _patch->GenerateImage(100, 100, GL_STATIC_DRAW);

        if (_beforeInterpolation)
            _beforeInterpolation->UpdateVertexBufferObjects();

        Matrix<DCoordinate3> data_points_to_interpolate(4, 4);
        for(GLuint row = 0; row < 4; ++row)
        {
            for (GLuint column = 0; column < 4; ++column)
            {
                _patch->GetData(row, column, data_points_to_interpolate(row, column));
            }
        }

        _interpolated_patch = new SecondOrderHyperbolicPatch(*_patch);

        if (_interpolated_patch->UpdateDataForInterpolation(u_knot_vector, v_knot_vector, data_points_to_interpolate))
        {
            _afterInterpolation = _interpolated_patch->GenerateImage(30, 30, GL_STATIC_DRAW);

            if (_afterInterpolation)
            {
                _afterInterpolation->UpdateVertexBufferObjects();
            }
        }

        _u_dir = _patch->GenerateUIsoparametricLines(_isoLineCount, 1, _gridDivPointCount);
        _v_dir = _patch->GenerateVIsoparametricLines(_isoLineCount, 1, _gridDivPointCount);

        _sphere.LoadFromOFF("Models/sphere.off");
        if (!_sphere.UpdateVertexBufferObjects())
        {
            releaseResources();
            throw Exception("Could not update VBO's of sphere!");
        }

        for (GLuint i = 0; i < _u_dir->GetColumnCount(); ++i) {
            (*_u_dir)[i]->UpdateVertexBufferObjects();
        }

        for (GLuint i = 0; i < _v_dir->GetColumnCount(); ++i) {
            (*_v_dir)[i]->UpdateVertexBufferObjects();
        }

        _radius = 0.05;
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

        if (event->button() == Qt::LeftButton)
        {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            GLuint size = 4 * 4 * _composite->getCurveCount();
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
            gluPickMatrix((GLdouble)event->x(), (GLdouble)(viewport[3] - event->y()), 5.0, 5.0, viewport);

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

            if (hit_count)
            {
                GLuint closest_selected = pick_buffer[3];
                GLuint closest_depth    = pick_buffer[1];


                for (GLint i = 1; i < hit_count; ++i)
                {
                    GLuint offset = i * 4;
                    if (pick_buffer[offset + 1] < closest_depth)
                    {
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
            }
            else
            {
                _join = _merge = GL_FALSE;
                _composite->setSelected(_row, _column, GL_FALSE);
                _row = _composite->getCurveCount() + 1;
                _named_object_clicked = GL_FALSE;
            }

            delete pick_buffer;

            updateGL();
        }
    }

    void GLWidget::wheelEvent(QWheelEvent *event)
    {
        event->accept();

        if (_named_object_clicked)
        {
            DCoordinate3 point = _composite->getPoint(_row, _column);
            GLdouble     &x     = point[0];
            GLdouble     &y     = point[1];
            GLdouble     &z     = point[2];

            // wheel + Ctrl
            if (event->modifiers() & Qt::ControlModifier)
            {
                x += event->delta() / 120.0 * _reposition_unit;
            }

            // wheel + Alt
            if (event->modifiers() & Qt::AltModifier)
            {
                y += event->delta() / 120.0 * _reposition_unit;
            }

            if (event->modifiers() & Qt::ShiftModifier)
            {
                z += event->delta() / 120.0 * _reposition_unit;
            }

            _composite->updateCurve(_row, _column, point);

            updateGL();
        }
    }

    GLvoid GLWidget::joinAndMergeHelper()
    {
        if (_join || _merge)
        {
            if (_arc1[1] != -1)
            {
                _arc2[0] = _row;
                _arc2[1] = _column;

                if ((_arc1[1] == 0 || _arc1[1] == 3) && (_arc2[1] == 0 || _arc2[1] == 3))
                {
                    SecondOrderHyperbolicCompositeCurve::Direction dir1, dir2;

                    if (_arc1[1] == 0)
                        dir1 = SecondOrderHyperbolicCompositeCurve::Direction::LEFT;
                    else if (_arc1[1] == 3)
                        dir1 = SecondOrderHyperbolicCompositeCurve::Direction::RIGHT;

                    if (_arc2[1] == 0)
                        dir2 = SecondOrderHyperbolicCompositeCurve::Direction::LEFT;
                    else if (_arc2[1] == 3)
                        dir2 = SecondOrderHyperbolicCompositeCurve::Direction::RIGHT;

                    _join ? _composite->join(_arc1[0], dir1, _arc2[0], dir2) : _composite->merge(_arc1[0], dir1, _arc2[0], dir2);
                }

                _join = _merge = GL_FALSE;
                _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
            }
            else
            {
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

    void GLWidget::set_off_model_selected(bool value)
    {
        if (!value)
        {
            _timer->stop();
        }
    }

    void GLWidget::browseFile(bool)
    {
        QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "./Models/",
                    "OFF File (*.OFF)");

        try
        {
            if (QString::compare("", fileName))
            {
                releaseResources();

                initModel(fileName.toStdString());

                _firstOrderDerivativeEnabled = _secondOrderDerivativeEnabled = _control_polygon = false;
                _interpolating_cyclic_curve = _cyclic_curve = false;
                _surfaceSelected = false;
                _off_model = true;

                updateGL();
            }
        }
        catch (Exception e)
        {
            cout << e << endl;
        }
    }

    void GLWidget::set_grid(bool value)
    {
        _grid = value;
        updateGL();
    }

    void GLWidget::set_mesh(bool value)
    {
        _mesh = value;
        updateGL();
    }

    void GLWidget::set_points(bool value)
    {
        _points = value;
        updateGL();
    }

    void GLWidget::set_show_surface(bool value)
    {
        _show_surface = value;
        updateGL();
    }

    void GLWidget::set_interpolate(bool value)
    {
        _interpolate = value;
        updateGL();
    }

    void GLWidget::set_iso_line_count(int value)
    {
        _isoLineCount = value;
        updateGL();
    }

    void GLWidget::set_grid_div_point_count(int value)
    {
        _gridDivPointCount = value;
        updateGL();
    }

    void GLWidget::set_shader(int shaderIndex)
    {
        _shader.Disable();

        QString shaderVertFilePath = "";
        QString shaderFragFilePath = "";

        switch (shaderIndex)
        {
            case 0:
                shaderVertFilePath = "Shaders/two_sided_lighting.vert";
                shaderFragFilePath = "Shaders/two_sided_lighting.frag";
                break;
            case 1:
                shaderVertFilePath = "Shaders/directional_light.vert";
                shaderFragFilePath = "Shaders/directional_light.frag";
                break;
            case 2:
                shaderVertFilePath = "Shaders/reflection_lines.vert";
                shaderFragFilePath = "Shaders/reflection_lines.frag";
                break;
            case 3:
                shaderVertFilePath = "Shaders/toon.vert";
                shaderFragFilePath = "Shaders/toon.frag";
        }

        _shader.InstallShaders(shaderVertFilePath.toUtf8().constData(),
                               shaderFragFilePath.toUtf8().constData(),
                               GL_TRUE);
        updateGL();
    }

    void GLWidget::set_color()
    {
        QColor color = QColorDialog::getColor(Qt::red, this);

        _color[0] = (GLfloat) color.red() / 255.0;
        _color[1] = (GLfloat) color.green() / 255.0;
        _color[2] = (GLfloat) color.blue()/ 255.0;

        _shader.Enable();
        _shader.SetUniformVariable4f("default_outline_color", _color[0], _color[1], _color[2], _color[3]);
        _shader.Disable();

        updateGL();
    }

    void GLWidget::set_alpha(double value)
    {
        _color[3] = (GLfloat) value;
        _shader.Enable();
        _shader.SetUniformVariable4fv("default_outline_color", 4, _color);
        _shader.Disable();

        updateGL();
    }

    void GLWidget::set_scale_factor(double value)
    {
        _scaleFactor = (GLfloat) value;
        _shader.Enable();
        _shader.SetUniformVariable1f("scale_factor", _scaleFactor);
        _shader.Disable();

        updateGL();
    }

    void GLWidget::set_smoothing(double value)
    {
        _smoothing = (GLfloat) value;
        _shader.Enable();
        _shader.SetUniformVariable1f("smoothing", _smoothing);
        _shader.Disable();

        updateGL();
    }

    void GLWidget::set_shading(double value)
    {
        _shading = (GLfloat) value;
        _shader.Enable();
        _shader.SetUniformVariable1f("shading", _shading);
        _shader.Disable();

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
        if (_column == 3 || _column == 0)
        {
            _composite->continueExistingArc(_row, _column == 3 ? SecondOrderHyperbolicCompositeCurve::Direction::RIGHT : SecondOrderHyperbolicCompositeCurve::Direction::LEFT);
            updateGL();
        }
    }

    void GLWidget::change_selected_color()
    {
        QColor color = QColorDialog::getColor(Qt::red, this);
        if (color.isValid())
            _composite->setSelectedColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
        updateGL();
    }

    void GLWidget::change_selected_arcs_color()
    {
        QColor color = QColorDialog::getColor(Qt::red, this);
        if (color.isValid())
            _composite->setCurveColor(_row, _column, color.redF(), color.greenF(), color.blueF(), color.alphaF());
         updateGL();
    }

    void GLWidget::join_arcs()
    {
        _merge = GL_FALSE;
        _join = GL_TRUE;
        _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
    }

    void GLWidget::merge_arcs()
    {
        _join = GL_FALSE;
        _merge = GL_TRUE;
        _arc1[0] = _arc1[1] = _arc2[0] = _arc2[1] = -1;
    }
}

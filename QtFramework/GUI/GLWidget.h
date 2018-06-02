#pragma once

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFormat>
#include <Parametric/ParametricCurves3.h>
#include <Parametric/ParametricSurfaces3.h>
#include <Core/GenericCurves3.h>
#include <Test/TestFunctions.h>
#include <Core/TriangulatedMeshes3.h>
#include <Core/Lights.h>
#include <Core/Materials.h>
#include <Core/ShaderPrograms.h>
#include <Hyperbolic/SecondOrderHyperbolicPatch.h>
#include <Hyperbolic/SecondOrderHyperbolicArc.h>
#include <Hyperbolic/SecondOrderHyperbolicCompositeCurve.h>

#include <string>
#include <QTimer>
#include <QWheelEvent>

#include "../Cyclic/CyclicCurves3.h"

namespace cagd
{
    class GLWidget: public QGLWidget
    {
        Q_OBJECT

    private:

        // variables defining the projection matrix
        float       _aspect;            // aspect ratio of the rendering window
        float       _fovy;              // field of view in direction y
        float       _z_near, _z_far;    // distance of near and far clipping planes

        // variables defining the model-view matrix
        float       _eye[3], _center[3], _up[3];

        // variables needed by transformations
        int         _angle_x, _angle_y, _angle_z;
        double      _zoom;
        double      _trans_x, _trans_y, _trans_z;

        // your other declarations
        void releaseResources();

        GLboolean _firstOrderDerivativeEnabled;
        GLboolean _secondOrderDerivativeEnabled;

        // - for cylic curve
        GLuint          _n;
        GLuint          _mod; // maximum ordef or derivatives
        GLuint          _div;
        CyclicCurve3    *_cc;
        GenericCurve3   *_img_cc;

        ColumnMatrix<GLdouble>      _knot_vector;
        ColumnMatrix<DCoordinate3>  _data_points_to_interpolate;

        GLboolean _control_polygon;
        GLboolean _cyclic_curve;
        GLboolean _interpolating_cyclic_curve;
        GLboolean _off_model;

        void paintCyclicCurve();

        // - for surfaces
        QTimer  *_timer;
        GLfloat _angle;

        TriangulatedMesh3 *_model;

        void initModel(const std::string& fileName);
        void paintModel();

        // - for parametric surfaces
        ParametricSurface3      *_surface;
        TriangulatedMesh3       *_surface_img;

        // tensor products
        GLdouble _alpha_u, _alpha_v;
        GLuint _n_u, _n_v;
        GLuint _u_div_point_count, _v_div_point_count;

        GLuint _isoLineCount, _gridDivPointCount;
        RowMatrix<GenericCurve3*> *_u_dir;
        RowMatrix<GenericCurve3*> *_v_dir;

        void initHyperbolicSurface();
        void paintHyperbolicSurface();

        GLboolean _surfaceSelected, _interpolate;
        GLboolean _grid, _mesh, _points, _show_surface;

        // shere for the controll polygon
        TriangulatedMesh3 _sphere;
        GLdouble _radius;

        // Shaders
        ShaderProgram _shader;
        GLfloat _scaleFactor;
        GLfloat _smoothing;
        GLfloat _shading;
        GLfloat _color[4];

        // Hyperboloic patch
        SecondOrderHyperbolicPatch *_patch, *_interpolated_patch;
        TriangulatedMesh3 *_beforeInterpolation, *_afterInterpolation;

//        SecondOrderHyperbolicArc    *_arc;
//        GenericCurve3               *_img_arc;
        SecondOrderHyperbolicCompositeCurve* _composite;

        GLuint               _row_count, _column_count;
        Matrix<DCoordinate3> _positions;

        GLboolean            _named_object_clicked;
        GLuint                _row;
        GLuint                _column;
        GLdouble             _reposition_unit;


//        GenericCurve3* initHyperbolicArc(SecondOrderHyperbolicArc*& arc);
//        void renderHyperbolicArc(SecondOrderHyperbolicArc* arc, GenericCurve3* img_arc);
    public:
        // special and default constructor
        // the format specifies the properties of the rendering window
        GLWidget(QWidget* parent = 0, const QGLFormat& format = QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer);

        // redeclared virtual functions
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);

        void mousePressEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

        virtual ~GLWidget();

    public slots:
        // public event handling methods/slots
        void set_angle_x(int value);
        void set_angle_y(int value);
        void set_angle_z(int value);

        void set_zoom_factor(double value);

        void set_trans_x(double value);
        void set_trans_y(double value);
        void set_trans_z(double value);

        // your other declarations
        void set_off_model_selected(bool value);

        void animate();
        void browseFile(bool value);

        void set_grid(bool value);
        void set_mesh(bool value);
        void set_points(bool value);
        void set_show_surface(bool value);
        void set_interpolate(bool value);

        void set_iso_line_count(int value);
        void set_grid_div_point_count(int value);

        void set_shader(int shaderIndex);
        void set_color();
        void set_alpha(double value);
        void set_scale_factor(double value);
        void set_smoothing(double value);
        void set_shading(double value);

        void set_firstOrderDerivative(bool value);
        void set_control_polygon(bool value);
        void set_control_points(bool value);
        void set_curve_image(bool value);

        void insert_isolated_arc();
        void remove_arc();
    };
}

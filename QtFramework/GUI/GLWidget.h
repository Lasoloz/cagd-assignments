#pragma once

#include <Core/GenericCurves3.h>
#include <Core/Lights.h>
#include <Core/Materials.h>
#include <Core/ShaderPrograms.h>
#include <Core/TriangulatedMeshes3.h>
#include <GL/glew.h>
#include <Hyperbolic/SecondOrderHyperbolicArc.h>
#include <Hyperbolic/SecondOrderHyperbolicCompositeCurve.h>
#include <Hyperbolic/SecondOrderHyperbolicCompositeSurface.h>
#include <Hyperbolic/SecondOrderHyperbolicPatch.h>
#include <Parametric/ParametricCurves3.h>
#include <Parametric/ParametricSurfaces3.h>
#include <QGLFormat>
#include <QGLWidget>
#include <Test/TestFunctions.h>

#include <QTimer>
#include <QWheelEvent>

#include <memory>
#include <string>

#include "../Cyclic/CyclicCurves3.h"

namespace cagd {
class GLWidget : public QGLWidget
{
    Q_OBJECT

private:
    // variables defining the projection matrix
    float _aspect;         // aspect ratio of the rendering window
    float _fovy;           // field of view in direction y
    float _z_near, _z_far; // distance of near and far clipping planes

    // variables defining the model-view matrix
    float _eye[3], _center[3], _up[3];

    // variables needed by transformations
    int    _angle_x, _angle_y, _angle_z;
    double _zoom;
    double _trans_x, _trans_y, _trans_z;

    // your other declarations
    void releaseResources();

    GLboolean _firstOrderDerivativeEnabled;

    // Shaders:
    std::shared_ptr<ShaderProgram> _directional_light;
    std::shared_ptr<ShaderProgram> _two_sided_light;
    std::shared_ptr<ShaderProgram> _toon;
    std::shared_ptr<ShaderProgram> _reflection_lines;


    SecondOrderHyperbolicCompositeCurve * _comp_curve;
    SecondOrderHyperbolicCompositeSurface _comp_surface;


    // On-screen events:
    GLboolean _named_object_clicked;
    GLuint    _primitiveIndex;
    GLuint    _controlPointIndex;
    GLdouble  _reposition_unit;

    GLvoid    joinAndMergeHelper();
    GLboolean _join;
    GLboolean _merge;
    GLint     _arc1[2];
    GLint     _arc2[2];


public:
    // special and default constructor
    // the format specifies the properties of the rendering window
    GLWidget(QWidget *parent = 0, const QGLFormat &format = QGL::Rgba |
                                                            QGL::DepthBuffer |
                                                            QGL::DoubleBuffer);

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
    // Both:
    void set_firstOrderDerivative(bool value);

    // Arc:
    void set_control_polygon(bool value);
    void set_control_points(bool value);
    void set_curve_image(bool value);

    void insert_isolated_arc();
    void remove_arc();
    void continue_arc();
    void change_selected_color();
    void change_selected_arcs_color();
    void join_arcs();
    void merge_arcs();
};

} // namespace cagd

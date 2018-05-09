#pragma once

#include <iostream>

#include <GL/glew.h>
#include <QGLFormat>
#include <QGLWidget>
#include <QString>

#include "Core/Constants.h"
#include "Core/Materials.h"
#include "Core/ShaderPrograms.h"
#include "Core/TriangulatedMeshes3.h"
#include "Hyperbolic/SecondOrderHyperbolicPatch.h"

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
    TriangulatedMesh3 *        surface;
    TriangulatedMesh3 *        interpolatedSurface;
    SecondOrderHyperbolicPatch patch;
    SecondOrderHyperbolicPatch interpolatedPatch;
    ShaderProgram              shaderProgram;
    Material                   transparentEmerald;

    RowMatrix<GenericCurve3 *> *u_isoparam_curves;
    RowMatrix<GenericCurve3 *> *v_isoparam_curves;

    bool _show_original;
    bool _show_interpolated;

public:
    // special and default constructor
    // the format specifies the properties of the rendering window
    GLWidget(QWidget *parent = 0, const QGLFormat &format = QGL::Rgba |
                                                            QGL::DepthBuffer |
                                                            QGL::DoubleBuffer);
    ~GLWidget();

    // redeclared virtual functions
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

public slots:
    // public event handling methods/slots
    void set_angle_x(int value);
    void set_angle_y(int value);
    void set_angle_z(int value);

    void set_zoom_factor(double value);

    void set_trans_x(double value);
    void set_trans_y(double value);
    void set_trans_z(double value);

    // Defined by me:
    void set_show_original_state(bool state);
    void set_show_interpolated_state(bool state);
};
} // namespace cagd

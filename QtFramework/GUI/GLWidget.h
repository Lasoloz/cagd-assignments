#pragma once

#include <iostream>

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFormat>
#include <QString>

#include "Core/GenericCurves3.h"
#include "Core/Constants.h"
#include "Parametric/ParametricCurves3.h"
#include "Cyclic/CyclicCurves3.h"
#include "Test/TestFunctions.h"

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


        // Generic curve image:
        GenericCurve3 * _curve_img;
        void clearCurveImg();

        bool parametricCurveType;

        // your other declarations
        ParametricCurve3 * _parametric_curve;
        QString _curve_name;
        bool _show_curve;
        bool _show_1st_deriv;
        bool _show_2nd_deriv;


        void clearParametricCurve();
        void setParametricCurve(QString &curve_name);

        // Cyclic curve:
        CyclicCurve3 * _cyclic_curve;
        CyclicCurve3 * createCyclicCurve(uint n);
        CyclicCurve3 * createInterpolatedCyclicCurve(uint n);
        void clearCyclicCurve();
        void setCyclicCurve(CyclicCurve3 * newCurve);

    public:
        // special and default constructor
        // the format specifies the properties of the rendering window
        GLWidget(QWidget* parent = 0, const QGLFormat& format = QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer);
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

        // Choose curve type:
        void set_curve_type(QString curve_name);
        void set_show_curve(bool state);
        void set_show_1st_deriv(bool state);
        void set_show_2nd_deriv(bool state);
    };
}

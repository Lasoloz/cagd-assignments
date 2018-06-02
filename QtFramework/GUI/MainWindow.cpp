#include "MainWindow.h"

namespace cagd
{
    MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    {
        setupUi(this);

    /*

      the structure of the main window's central widget

     *---------------------------------------------------*
     |                 central widget                    |
     |                                                   |
     |  *---------------------------*-----------------*  |
     |  |     rendering context     |   scroll area   |  |
     |  |       OpenGL widget       | *-------------* |  |
     |  |                           | | side widget | |  |
     |  |                           | |             | |  |
     |  |                           | |             | |  |
     |  |                           | *-------------* |  |
     |  *---------------------------*-----------------*  |
     |                                                   |
     *---------------------------------------------------*

    */
        _side_widget = new SideWidget(this);

        _scroll_area = new QScrollArea(this);
        _scroll_area->setWidget(_side_widget);
        _scroll_area->setSizePolicy(_side_widget->sizePolicy());
        _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        _gl_widget = new GLWidget(this);

        centralWidget()->setLayout(new QHBoxLayout());
        centralWidget()->layout()->addWidget(_gl_widget);
        centralWidget()->layout()->addWidget(_scroll_area);

        // creating a signal slot mechanism between the rendering context and the side widget
        connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_x(int)));
        connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_y(int)));
        connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_z(int)));

        connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_zoom_factor(double)));

        connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_x(double)));
        connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_y(double)));
        connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_z(double)));


        connect(_side_widget->loadButton, SIGNAL(clicked(bool)), _gl_widget, SLOT(browseFile(bool)));

//        connect(_side_widget->hyperbolicRButton, SIGNAL(toggled(bool)), _gl_widget, SLOT(select_surface(bool)));

        connect(_side_widget->grid, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_grid(bool)));
        connect(_side_widget->mesh, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_mesh(bool)));
        connect(_side_widget->points, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_points(bool)));
        connect(_side_widget->surface, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_show_surface(bool)));
        connect(_side_widget->interpolate, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_interpolate(bool)));

        connect(_side_widget->isoLineCountspinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_iso_line_count(int)));
        connect(_side_widget->gridDivPointCountSpinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_grid_div_point_count(int)));

        connect(_side_widget->shadersComboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_shader(int)));
        connect(_side_widget->colorPickerButton, SIGNAL(released()), _gl_widget, SLOT(set_color()));

        connect(_side_widget->scaleFactorSpinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_scale_factor(double)));
        connect(_side_widget->smoothingSpinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_smoothing(double)));
        connect(_side_widget->shadingSpinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_shading(double)));
        connect(_side_widget->alphaSpinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_alpha(double)));


        connect(_side_widget->firstOrderDerivatives, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_firstOrderDerivative(bool)));
        connect(_side_widget->controlPolygon, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_control_polygon(bool)));
        connect(_side_widget->controlPoints, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_control_points(bool)));
        connect(_side_widget->curveImage, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_curve_image(bool)));

        connect(_side_widget->insertNewArcButton, SIGNAL(pressed()), _gl_widget, SLOT(insert_isolated_arc()));
        connect(_side_widget->ereaseArcButton, SIGNAL(pressed()), _gl_widget, SLOT(remove_arc()));
    }

    //--------------------------------
    // implementation of private slots
    //--------------------------------
    void MainWindow::on_action_Quit_triggered()
    {
        qApp->exit(0);
    }
}

#include "MainWindow.h"

namespace cagd {
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    // creating a signal slot mechanism between the rendering context and the
    // side widget
    connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)),
            _gl_widget, SLOT(set_angle_x(int)));
    connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)),
            _gl_widget, SLOT(set_angle_y(int)));
    connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)),
            _gl_widget, SLOT(set_angle_z(int)));

    connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_zoom_factor(double)));

    connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_trans_x(double)));
    connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_trans_y(double)));
    connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_trans_z(double)));


    // Both:
    connect(_side_widget->firstOrderDerivatives, SIGNAL(clicked(bool)),
            _gl_widget, SLOT(set_firstOrderDerivative(bool)));
    connect(_side_widget->saveButton, SIGNAL(pressed()), _gl_widget,
            SLOT(save()));
    connect(_side_widget->loadButton, SIGNAL(pressed()), _gl_widget,
            SLOT(load()));

    connect(_side_widget->dirLight, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_directional_light(bool)));
    connect(_side_widget->pointLight, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_point_light(bool)));
    connect(_side_widget->spotLight, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_spotlight(bool)));

    // Curve:
    connect(_side_widget->secondOrderDerivatives, SIGNAL(toggled(bool)),
            _gl_widget, SLOT(set_secondOrderDerivative(bool)));
    connect(_side_widget->controlPolygon, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_control_polygon(bool)));
    connect(_side_widget->controlPoints, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_control_points(bool)));
    connect(_side_widget->curveImage, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_curve_image(bool)));

    connect(_side_widget->insertNewArcButton, SIGNAL(pressed()), _gl_widget,
            SLOT(insert_isolated_arc()));
    connect(_side_widget->ereaseArcButton, SIGNAL(pressed()), _gl_widget,
            SLOT(remove_arc()));
    connect(_side_widget->continueArcButton, SIGNAL(pressed()), _gl_widget,
            SLOT(continue_arc()));
    connect(_side_widget->selectedArcColorButton, SIGNAL(pressed()), _gl_widget,
            SLOT(change_selected_arcs_color()));
    connect(_side_widget->selectedColorButton, SIGNAL(pressed()), _gl_widget,
            SLOT(change_selected_color()));
    connect(_side_widget->joinArcsButton, SIGNAL(pressed()), _gl_widget,
            SLOT(join()));
    connect(_side_widget->mergeArcsButton, SIGNAL(pressed()), _gl_widget,
            SLOT(merge()));


    // Patches:
    connect(_side_widget->controlPolygonPatch, SIGNAL(toggled(bool)),
            _gl_widget, SLOT(set_patch_wireframe_shown(bool)));
    connect(_side_widget->controlPointsPatch, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_patch_control_points_shown(bool)));
    connect(_side_widget->showSurface, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_patch_image_shown(bool)));
    connect(_side_widget->insertNewPatchButton, SIGNAL(pressed()), _gl_widget,
            SLOT(insert_isolated_surface()));
    connect(_side_widget->showNormals, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_normals_shown(bool)));
    connect(_side_widget->showIsoparametric, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_isoparametric_lines_shown(bool)));
    connect(_side_widget->uvDerivativesPatch, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_uv_derivatives_shown(bool)));
    connect(_side_widget->loadTextureButton, SIGNAL(pressed()), _gl_widget,
            SLOT(load_texture()));
    connect(_side_widget->enableTexture, SIGNAL(toggled(bool)), _gl_widget,
            SLOT(set_texture_shown(bool)));

    connect(_side_widget->joinPatchButton, SIGNAL(pressed()), _gl_widget,
            SLOT(join()));
    connect(_side_widget->mergePatchButton, SIGNAL(pressed()), _gl_widget,
            SLOT(merge()));

    connect(_side_widget->continuePatchButton, SIGNAL(pressed()), _gl_widget,
            SLOT(continue_patch()));

    connect(_side_widget->shadersComboBox, SIGNAL(activated(QString)),
            _gl_widget, SLOT(set_shader(QString)));

    connect(_side_widget->scaleFactorSpinBox, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_scale_factor(double)));
    connect(_side_widget->smoothingSpinBox, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_smoothing(double)));
    connect(_side_widget->shadingSpinBox, SIGNAL(valueChanged(double)),
            _gl_widget, SLOT(set_shading(double)));
    connect(_side_widget->colorPickerButton, SIGNAL(pressed()), _gl_widget,
            SLOT(set_color()));
}

//--------------------------------
// implementation of private slots
//--------------------------------
void MainWindow::on_action_Quit_triggered() { qApp->exit(0); }
} // namespace cagd

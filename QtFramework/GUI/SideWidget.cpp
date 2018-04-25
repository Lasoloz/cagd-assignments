#include "SideWidget.h"

#include "Test/TestFunctions.h"

namespace cagd {
SideWidget::SideWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    QPalette p = rotate_x_slider->palette();

    p.setColor(QPalette::Highlight, QColor(255, 50, 10).lighter());

    rotate_x_slider->setPalette(p);

    p = rotate_y_slider->palette();

    p.setColor(QPalette::Highlight, QColor(50, 255, 10).lighter());

    rotate_y_slider->setPalette(p);



    // Other stuff:
    mesh_selector->addItem("mouse");
    mesh_selector->addItem("elephant");
    mesh_selector->addItem("sphere");

    parametric_selector->addItem(hyperboloid::surface_name);
    parametric_selector->addItem(sphere::surface_name);
    parametric_selector->addItem(seashell::surface_name);
    parametric_selector->addItem(moebius::surface_name);
    parametric_selector->addItem(klein_bootle::surface_name);
}
} // namespace cagd

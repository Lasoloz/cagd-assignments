#include "SideWidget.h"

#include "Test/TestFunctions.h"

namespace cagd
{
    SideWidget::SideWidget(QWidget *parent): QWidget(parent)
    {
        setupUi(this);

        QPalette p = rotate_x_slider->palette();

        p.setColor(QPalette::Highlight, QColor(255,50,10).lighter());

        rotate_x_slider->setPalette(p);

        p = rotate_y_slider->palette();

        p.setColor(QPalette::Highlight, QColor(50,255,10).lighter());

        rotate_y_slider->setPalette(p);

        curve_selector->addItem(spiral_on_cone::curve_name);
        curve_selector->addItem(cochleoid::curve_name);
        curve_selector->addItem(epicycloid::curve_name);
        curve_selector->addItem(viviani::curve_name);
        curve_selector->addItem(loxodrome::curve_name);
        curve_selector->addItem(fermat::curve_name);
        curve_selector->addItem(cyclic::curve_name);
        curve_selector->addItem(cyclic_interpolation::curve_name);

        cb_curve->setChecked(true);
    }
}

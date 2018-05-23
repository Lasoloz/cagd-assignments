#pragma once

#include "ui_SideWidget.h"
#include <QWidget>

namespace cagd {
class SideWidget : public QWidget, public Ui::SideWidget
{
public:
    // special and default constructor
    SideWidget(QWidget *parent = 0);
};
} // namespace cagd

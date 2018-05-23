#pragma once

#include "GLWidget.h"
#include "SideWidget.h"
#include "ui_MainWindow.h"
#include <QComboBox>
#include <QMainWindow>
#include <QScrollArea>

namespace cagd {
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

private:
    SideWidget *_side_widget; // pointer to our side widget
    GLWidget *  _gl_widget;   // pointer to our rendering OpenGL widget

    QScrollArea *_scroll_area; // provides a scrolling view onto our side widget

public:
    // special and default constructor
    MainWindow(QWidget *parent = 0);

private slots:
    // private event handling methods/slots
    void on_action_Quit_triggered();
};
} // namespace cagd

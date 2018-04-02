#include <QApplication>
#include "GUI/MainWindow.h"

#include <Core/Matrices.h>
#include <Core/RealSquareMatrices.h>
#include <iostream>
#include <fstream>

using namespace cagd;


void testMatrix() {
    Matrix<int> m(3, 5);
    std::cout << m << '\n';

    for (GLuint i = 0; i < m.GetRowCount(); ++i) {
        for (GLuint j = 0; j < m.GetColumnCount(); ++j) {
            m(i, j) = i * m.GetColumnCount() + j;
        }
    }

    std::ofstream fout("log");
    fout << m << '\n';

    fout.close();

    std::ifstream fin("log");

    Matrix<int> n;
    fin >> n;

    std::cout << n << '\n';
    n.ResizeColumns(7);
    std::cout << n << '\n';

    n(1, 6) = -1;
    std::cout << n << '\n';

    fin.close();
}

void testTriangular() {
    TriangularMatrix<int> m(3);
    std::cout << m << '\n';

    m(2,2) = 4;

    std::ofstream fout("log");
    fout << m << '\n';

    fout.close();

    std::ifstream fin("log");

    TriangularMatrix<int> n;
    fin >> n;

    std::cout << n << '\n';
    n.ResizeRows(5);
    std::cout << n << '\n';

    n(4, 3) = -1;
    std::cout << n << '\n';

    fin.close();
}


int main(int argc, char **argv)
{
    // creating an application object and setting one of its attributes
    QApplication app(argc, argv);
    testMatrix();
    testTriangular();

    // if you have installed a different version of Qt, it may happen that
    // the application attribute Qt::AA_UseDesktopOpenGL is not recognized
    // on Windows its existence is critical for our applications
    // on Linux or Mac you can uncomment this line
    app.setAttribute(Qt::AA_UseDesktopOpenGL, true);

    // creating a main window object
    MainWindow mwnd;
    mwnd.showMaximized();


    // running the application
    return app.exec();
}

QT += core gui widgets opengl

win32 {
    message("Windows platform...")

    INCLUDEPATH += $$PWD/Dependencies/Include
    DEPENDPATH += $$PWD/Dependencies/Include

    LIBS += -lopengl32 -lglu32

    CONFIG(release, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x86_64 (i.e., 64-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86_64/" -lglew32
        }
    } else: CONFIG(debug, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x86_64 (i.e., 64-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86_64" -lglew32
        }
    }

    msvc {
      QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
      QMAKE_CXXFLAGS_RELEASE *= -O2
    }
}

mac {
    # for GLEW installed into /usr/lib/libGLEW.so or /usr/lib/glew.lib
    LIBS += -lGLEW -lGLU
}

unix {
    # for GLEW installed into /usr/lib/libGLEW.so or /usr/lib/glew.lib
    LIBS += -lGLEW -lGLU
}

FORMS += \
    GUI/MainWindow.ui \
    GUI/SideWidget.ui

HEADERS += \
    GUI/GLWidget.h \
    GUI/MainWindow.h \
    GUI/SideWidget.h \
    Core/Exceptions.h \
    Core/RealSquareMatrices.h \
    Core/Matrices.h \
    Core/DCoordinates3.h \
    Core/LinearCombination3.h \
    Core/GenericCurves3.h \
    Core/Constants.h \
    Parametric/ParametricCurves3.h \
    Test/TestFunctions.h \
    Cyclic/CyclicCurves3.h \
    Core/TriangulatedMeshes3.h \
    Core/TriangularFaces.h \
    Core/TCoordinates4.h \
    Core/Materials.h \
    Core/Lights.h \
    Core/HCoordinates3.h \
    Core/Colors4.h

SOURCES += \
    GUI/GLWidget.cpp \
    GUI/MainWindow.cpp \
    GUI/SideWidget.cpp \
    main.cpp \
    Core/RealSquareMatrices.cpp \
    Core/LinearCombination3.cpp \
    Core/GenericCurves3.cpp \
    Parametric/ParametricCurves3.cpp \
    Test/TestFunctions.cpp \
    Cyclic/CyclicCurves3.cpp \
    Core/TriangulatedMeshes3.cpp \
    Core/Materials.cpp \
    Core/Lights.cpp

#CONFIG += console

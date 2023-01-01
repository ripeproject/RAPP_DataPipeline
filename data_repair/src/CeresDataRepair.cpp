

#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);

    cMainWindow mainWin;
    mainWin.initialize();
    mainWin.show();

	return app.exec();
}


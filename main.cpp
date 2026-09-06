#include <QApplication>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Auto Clicker");
    app.setWindowIcon(QIcon(":/assets/icons/icon_512.png"));

    MainWindow w;
    w.show();
    return app.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application properties
    a.setApplicationName("Peg Solitaire");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Peg Solitaire Game");
    a.setWindowIcon(QIcon(":/icon.svg"));
    
    MainWindow w;
    w.show();
    return a.exec();
}

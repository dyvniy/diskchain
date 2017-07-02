#include "pch.h"
#include "mydebug.h"
#include "mainwindow.h"
#include <windows.h>
using namespace std;
#include <QApplication>
#include <iostream>


int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    qInstallMessageHandler(messageToFile);
#endif

    qDebug() << "======================================";
    qDebug() << "Build date:"
             << QLibraryInfo::buildDate().toString("yyyy/MM-dd");
    qWarning() << "Мой текст";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

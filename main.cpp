#ifdef _WIN32
#include <QtWidgets/QApplication>
#include <Windows.h>
#else
#include <QApplication>
#endif

#include <QWidget>
#include "bridgecontroller.h"
#include "mainwindow.h"
#include <QSharedMemory>
#include <QMessageBox>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("EGOTEC_AG");
    a.setOrganizationDomain("egotec");
    a.setApplicationName("SaaS-Tasksymbol");

#ifndef __APPLE__
    QSharedMemory sharedMemory("{--SaaS.deTimerecordingApplication--}");
    if (!sharedMemory.create(sizeof(int))) {
        QMessageBox msgBox;
        msgBox.setText("This application is already running!");
        msgBox.exec();

        return 1;
    } else {
        BridgeControllerWindow w;
        w.setVisible(false);

        int result = a.exec();
        sharedMemory.unlock();
        return result;
    }
#endif
#ifdef __APPLE__
    BridgeControllerWindow w;
    w.setVisible(false);

    int result = a.exec();
    return result;
#endif
}

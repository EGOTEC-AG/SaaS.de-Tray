#ifndef BRIDGECONTROLLER_H
#define BRIDGECONTROLLER_H

#include <QObject>
#include <QWebEngineSettings>

#include "mainwindow.h"

class BridgeControllerWindow : public MainWindow
{
        Q_OBJECT
    public:
        BridgeControllerWindow(QWidget *parent = 0);

        Q_INVOKABLE void setUserData(QString employeeKey, QString version);
        Q_INVOKABLE void setEmployeeState(QString state);
        Q_INVOKABLE void openSaas();
        Q_INVOKABLE void openRegisterPage();
    private:
        void createWebkitFrame(bool display);
    private slots:
        void onLoadFinished();
};

#endif // BRIDGECONTROLLER_H

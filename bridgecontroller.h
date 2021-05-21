#ifndef BRIDGECONTROLLER_H
#define BRIDGECONTROLLER_H

#include <QObject>
#include <QWebEngineSettings>
#include <QPushButton>
#include <QLabel>

#include "mainwindow.h"

class BridgeControllerWindow : public MainWindow
{
    Q_OBJECT
public:
    BridgeControllerWindow(QWidget *parent = 0);

    Q_INVOKABLE void setUserData(QString employeeKey, QString version);
    Q_INVOKABLE void setEmployeeState(QString state);
    //Q_INVOKABLE void openSaas();
    //Q_INVOKABLE void openRegisterPage();
    Q_INVOKABLE void changeUrl(QString url);
    QDialog *qDialog;
    QPushButton *button;
    QLabel *label;
    int counter;

private:
    void createDialog();
    void createWebkitFrame();
public slots:
    void onLoadFinished();
    void checkOnline();
};

#endif // BRIDGECONTROLLER_H

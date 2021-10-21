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
    Q_INVOKABLE void setGoTimestamp();
    //Q_INVOKABLE void openSaas();
    //Q_INVOKABLE void openRegisterPage();
    Q_INVOKABLE void changeUrl(QString url);
    Q_INVOKABLE void pingDebug(QString log);

    int counter;
    bool dialog = false;

private:
    QDialog *qDialog;
    QPushButton *button;
    QLabel *label;
    QTimer *timer;
    void createDialog();
    void createWebkitFrame();

public slots:
    void onLoadFinished();
    void checkOnline();
    void ping();
};

#endif // BRIDGECONTROLLER_H

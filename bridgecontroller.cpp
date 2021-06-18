#include "bridgecontroller.h"
#include "mainwindow.h"

#include "helper/logger.h"

#ifdef _WIN32
#include <windows.h>
#include <WinInet.h>
#elif __linux__
#include <signal.h>
#include <unistd.h>
#endif

#include <QWebChannel>

#include <QDir>
#include <QHostInfo>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QProcess>

#include <QVBoxLayout>

#include <QDebug>


BridgeControllerWindow::BridgeControllerWindow(QWidget *parent) : MainWindow(parent) {
    createWebkitFrame();
    checkOnline();
}

void BridgeControllerWindow::createWebkitFrame() {
    // Einstellungen für den Browser setzen (LocalStrage/Session Storage/OfflineDatabase)
    QWebEngineSettings *s = QWebEngineSettings::globalSettings();
    s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    webView = new WebView(parent);
    webView->resize(UI_WIDTH, UI_HEIGHT);
    webView->setWindowTitle(appName);
    webView->setContextMenuPolicy(Qt::NoContextMenu);
    webView->setVisible(true);
    webView->setFixedSize(UI_WIDTH, UI_HEIGHT);

#ifdef _WIN32
    webView->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
#endif
    mainFrame = webView->page();
    connect(mainFrame, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished()));
}

void BridgeControllerWindow::onLoadFinished() {
    QString hostName = QHostInfo::localHostName();
    mainFrame->runJavaScript("window.loginComponentRef.setDeviceName('"+ hostName +"');");
    mainFrame->runJavaScript("window.loginComponentRef.taskAppCome();");

    QWebChannel* channel = new QWebChannel(mainFrame);
    mainFrame->setWebChannel(channel);
    channel->registerObject("api", this);
}

void BridgeControllerWindow::checkOnline() {
    QString ping = "ping -c 1 ";

    #ifdef _WIN32
         ping = "ping -n 1 ";
    #endif

     ping += URL;
     ping.replace("https://" , "");

    if(QProcess::execute(ping) == 0)
    {
        if(dialog == true) {
            qDialog->close();
            dialog = false;
        }
        counter = 0;
        changeUrl(URL);
        webView->show();
        createTrayicon();
    } else {
        if (counter == 0) {
             createDialog();
             dialog = true;
        }
        qDialog->show();
        webView->close();
        counter++;
        QString s = QString::number(counter);
        label->setText("Keine Verbindung zu "+ URL +" moeglich. Bitte uerpruefen Sie Ihre Internet-Verbindung oder melden sich beim Support. Versuch: " + s + " von 5.");
        if(counter >= 5) {
            button->setText("Beenden");
            connect(button, SIGNAL(clicked()), this, SLOT(onQuit()));
        }
    }
}

void BridgeControllerWindow::createDialog() {
    qDialog = new QDialog(parent);
    qDialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    label = new QLabel(qDialog);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(label, Qt::AlignCenter);
    button = new QPushButton(qDialog);
    button->setText("Nochmal Versuchen");
    vLayout->addWidget(button, Qt::AlignCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(checkOnline()));
    qDialog->setWindowTitle(appName);
    qDialog->setLayout(vLayout);
    qDialog->open();
}

void BridgeControllerWindow::setUserData(QString employeeKey, QString version) {
    userKey = employeeKey;
    saasversion = version;

    Logger("setUserData");
}

void BridgeControllerWindow::setEmployeeState(QString state) {
    if(MainWindow::state != state){
        MainWindow::changeEmployeeState(state);
        MainWindow::state = state;
    }

    Logger("setEmployeeState");
}

void BridgeControllerWindow::changeUrl(QString url) {
    webView->load(QUrl(url + "/timerecordingv2/#/?api=1"));
    MainWindow::saveSettings(url);

    Logger("changeUrl");
}


/*void BridgeControllerWindow::openSaas() {
    QDesktopServices::openUrl(QUrl(URL));

    Logger("openSaas");
}

void BridgeControllerWindow::openRegisterPage() {
    QDesktopServices::openUrl(QUrl(URL + "/?register=1"));

    Logger("openRegisterPage");
}*/

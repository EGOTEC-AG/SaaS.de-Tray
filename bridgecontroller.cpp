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
#include <QTimer>
#include <QSettings>

#include <QVBoxLayout>


BridgeControllerWindow::BridgeControllerWindow(QWidget *parent) : MainWindow(parent) {
    createWebkitFrame();
    checkOnline();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(ping()));
    timer->setInterval(120000);
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
    QWebChannel* channel = new QWebChannel(mainFrame);
    mainFrame->setWebChannel(channel);
    channel->registerObject("api", this);

    QString hostName = QHostInfo::localHostName();
    mainFrame->runJavaScript("window.loginComponentRef.setDeviceName('"+ hostName +"');");
    mainFrame->runJavaScript("window.loginComponentRef.taskAppCome();");


    timer->start();
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

void BridgeControllerWindow::ping()
{
    QSettings settings;
    long long ts = settings.value("TS").toULongLong();
    const auto p1 = std::chrono::system_clock::now();
    long long timenow = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
    if(timenow - ts > 28800) { // 28800 sekunden = 8h
        mainFrame->runJavaScript("window.loginComponentRef.taskPing(true);");
        Logger("ping true timenow: " + std::to_string(timenow) + " lastGo: " + std::to_string(ts));
    } else {
        mainFrame->runJavaScript("window.loginComponentRef.taskPing(false);");
        Logger("ping false timenow: " + std::to_string(timenow) + " lastGo: " + std::to_string(ts));
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


void BridgeControllerWindow::pingDebug(QString log) {
    Logger("DebugPing " + log.toStdString());
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

void BridgeControllerWindow::setGoTimestamp()
{
    QSettings settings;
    const auto p1 = std::chrono::system_clock::now();
    settings.setValue("TS", std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count());
    //timer->stop();
    Logger("setGoTimestamp() " + settings.value("TS").toString().toStdString());
}

void BridgeControllerWindow::changeUrl(QString url) {
    webView->load(QUrl(url + "/timerecordingv2/#/?api=1"));
    MainWindow::saveSettings(url);

    Logger("changeUrl");
}

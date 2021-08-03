#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bridgecontroller.h"

#include "helper/logger.h"

#include <QLocale>

#include <QApplication>
#include <QDesktopWidget>
#include <QWebEngineSettings>
#include <QSystemTrayIcon>

#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QHostInfo>
#include <QUrl>
#include <QDir>
#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QProcess>

#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

//#include <QDebug>

#ifdef _WIN32
#include <Windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Settings
#ifdef _WIN32
    MainWindow::versionURL = "http://files.saas.de/tasksymbol_2021/windows/version";
    MainWindow::setupFileName = "SaaS.de.Setup.exe";
#elif __APPLE__
    MainWindow::versionURL = "http://files.saas.de/saasde/tray/mac/version";
    MainWindow::setupFileName = "false";    // TODO ????
#endif

    qApp->setWindowIcon(QIcon(":/icon/tray.png"));

    this->loadSettings();
    this->parent = parent;

    UI_WIDTH  = 450;
    UI_HEIGHT  = 750;

    QDesktopWidget *desktop = QApplication::desktop();
    int screenHeight = desktop->height();

    if ((UI_HEIGHT + 100) >= screenHeight) {
        UI_HEIGHT = screenHeight - 100;
    }

    setGeometry(
                (QApplication::desktop()->width() / 2) - (UI_WIDTH / 2),
                (QApplication::desktop()->height() / 2) - (UI_HEIGHT / 2),
                UI_WIDTH, UI_HEIGHT
                );
    setFixedSize(UI_WIDTH, UI_HEIGHT);

    setLocalizedStrings();

    ui->setupUi(this);

    logNetworkIFace();

    Logger("OS Info: " + QSysInfo::prettyProductName().toStdString() + " " + QSysInfo::kernelVersion().toStdString());
    Logger("App Version: " + getLocalVersion().toStdString());

    downloadFile(versionURL);
}

void MainWindow::loadSettings() {
    QSettings settings;
    URL = settings.value("URL", "https://desktop.saas.de").toString();
    Logger("loadSettings " + URL.toStdString());

}

void MainWindow::saveSettings(QString url) {
    QSettings settings;
    settings.setValue("URL", url);
    URL = url;
    Logger("saveSettings " + url.toStdString());
}

void MainWindow::setLocalizedStrings() {
    if (getOSLanguage().contains("de")) {
        appName = "SaaS.de - Zeiterfassung";
        appRunningMinimized = "Die Application arbeitet nun minimiert!";
        come = "Kommen";
        comeMessage = "Sie sind nun anwesend.";
        go = "Gehen";
        goMessage = "Sie sind nun abwesend.";
        quit = "Beenden";
    } else {
        appName = "SaaS.de - Timerecording";
        appRunningMinimized = "The application is now running minimized!";
        come = "Come";
        comeMessage = "You are now present.";
        go = "Go";
        goMessage = "You are now absent.";
        quit = "Quit";
    }
}

void MainWindow::createTrayicon() {
    showHideAction = new QAction(appName, this);
    showHideAction->setIcon(QIcon(":/icon/window_size.png"));
    quitAction = new QAction(quit, this);
    quitAction->setIcon(QIcon(":/icon/exit.png"));

    connect (showHideAction, SIGNAL(triggered()), this, SLOT(showHideWindow()));
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showHideAction);
    trayIconMenu->addAction(quitAction);

    systray = new QSystemTrayIcon(this);
    systray->setIcon(QIcon(":/icon/tray.png"));
    //#ifndef __APPLE__
    systray->setContextMenu (trayIconMenu);
    //#endif
    systray->show();

    connect(systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(comeGo(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::showHideWindow() {
    if (webView->isVisible()) {
        webView->setVisible(false);
    } else {
        webView->setVisible(true);
    }
}

void MainWindow::startSetup()
{
    QProcess* process = new QProcess();
    process->setProgram(setupFileName);
    QStringList arguments;
    arguments << "-update";
    process->setArguments(arguments);
    process->start();

    qApp->closeAllWindows();
    qApp->quit();
}

void MainWindow::changeEmployeeState(QString userState) {
    if (userState == "true") {
        systray->setIcon(QIcon(":/icon/clock_stop.png"));
        qApp->setWindowIcon(QIcon(":/icon/clock_stop.png"));
        //  systray->showMessage(appName, comeMessage);
    } else {
        systray->setIcon(QIcon(":/icon/tray.png"));
        qApp->setWindowIcon(QIcon(":/icon/tray.png"));
        //  systray->showMessage(appName, goMessage);
    }
}

void MainWindow::comeGo(QSystemTrayIcon::ActivationReason e) {
    if (e == 3 && userKey != "") { // e == 3 == leftmouseclick
        if(state == "true") {
            mainFrame->runJavaScript("window.loginComponentRef.taskAppGo();");
        } else {
            mainFrame->runJavaScript("window.loginComponentRef.taskAppCome();");
        }
        Logger("leftclick trayicon comeGo");
    }
}

void MainWindow::onQuit() {
    sendGoRequest();
#ifdef _WIN32
    Sleep(2000);
    std::exit(EXIT_SUCCESS);
#endif
#ifdef __APPLE__
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::exit(EXIT_SUCCESS);
#endif
}

void MainWindow::sendGoRequest() {
    QNetworkRequest req(QUrl(URL + "/rest/apps/simplego"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString hostName = QHostInfo::localHostName();
    QJsonObject param;
    param.insert("uuid", QJsonValue::fromVariant(userKey));
    param.insert("device", QJsonValue::fromVariant(hostName));

    QNetworkAccessManager http;
    QNetworkReply *netReply = http.post(req, QJsonDocument(param).toJson(QJsonDocument::Compact));

    QEventLoop loop;
    connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray bytes = netReply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    int statusCode = netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    Logger("sendGoRequest " + URL.toStdString() + " StatusCode: " + std::to_string(statusCode) + " Response: " + str.toStdString());
}

QString MainWindow::getOSLanguage() {
    QLocale* sysInfo = new QLocale();
    return sysInfo->name();
}

QString MainWindow::getLocalVersion() {
    QFile localVersion(qApp->applicationDirPath() + "/version");
    if(!localVersion.open(QIODevice::ReadOnly)) {
       return "Version File not found";
    }
    QTextStream in(&localVersion);

    return in.readLine().replace(QString("\n"), QString(""));
}

void MainWindow::downloadFile(QUrl fileUrl) {
    m_downloadCtrl = new FileDownloader(fileUrl, this);

    connect(m_downloadCtrl, SIGNAL(connectionError()), this, SLOT(offlineStart()));
    connect(m_downloadCtrl, SIGNAL(downloaded()), this, SLOT(checkForUpdate()));
}

void MainWindow::createDialog()
{
    qDialog = new QDialog();
  //qDialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    qDialog->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    label = new QLabel(qDialog);
    label->setText("Eine neue Version (" + onlineVersion  + ") ist verfügbar. Installierte Version: "  + getLocalVersion());
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(label, Qt::AlignCenter);
    button = new QPushButton(qDialog);
    button->setText("Update");
    vLayout->addWidget(button, Qt::AlignCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(startSetup()));   // TODO Change behavior on Mac
    qDialog->setWindowTitle(appName);
    qDialog->setLayout(vLayout);
    qDialog->open();
}

void MainWindow::checkForUpdate() {
    onlineVersion = m_downloadCtrl->downloadedData().replace("\n", "");
     if (getLocalVersion() != onlineVersion) {
         createDialog();
         Logger("New Version found " + onlineVersion.toStdString());
     }
}


void MainWindow::logNetworkIFace(){
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface iFace, allInterfaces) {
            if((iFace.flags() & QNetworkInterface::IsUp) && (iFace.flags() & QNetworkInterface::IsRunning) && !(iFace.type() & QNetworkInterface::Loopback)) {
                QString temp = QVariant::fromValue(iFace.type()).toString();
                Logger("NetworkInterace: " + iFace.humanReadableName().toStdString() + " " + temp.toStdString());
            }
        }
}


void MainWindow::deleteOldWindow() {
    delete webView;
    delete showHideAction;
    delete quitAction;
    delete systray;
}

MainWindow::~MainWindow() {
    delete ui;
}

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
#include <QDesktopServices>

#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>


#ifdef _WIN32
#include <Windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Read local Version file
    this->getLocalVersion();
    // Settings
#ifdef _WIN32
    versionURL = "http://files.saas.de/tasksymbol_2021/windows/version";
    setupFileName = "SaaS.de.Setup.exe";
#elif __APPLE__
    versionURL = "http://files.saas.de/tasksymbol_2021/mac/version";
    setupFileName = "false";    // TODO ????
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
    Logger("App Version: " + localVersion.toStdString());
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
    systray->setContextMenu (trayIconMenu);
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
#ifdef _WIN32
    QProcess* process = new QProcess();
    process->setProgram(setupFileName);
    QStringList arguments;
    arguments << "-update";
    process->setArguments(arguments);
    process->start();
#elif __APPLE__
    QDesktopServices::openUrl(QUrl("https://files.saas.de/tasksymbol_2021/mac/Timerecording.dmg.zip"));
#endif
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

void MainWindow::onQuit(QString w) {
    Logger("onQuit "  + w.toStdString());
}

QString MainWindow::getOSLanguage() {
    QLocale* sysInfo = new QLocale();
    return sysInfo->name();
}

void MainWindow::getLocalVersion() {
    QFile localVersionFilePath(qApp->applicationDirPath() + "/version");
    if(!localVersionFilePath.open(QIODevice::ReadOnly)) {
        Logger("No Version File found");
    }
    QTextStream in(&localVersionFilePath);
    localVersion = in.readLine();
}

void MainWindow::downloadFile(QUrl fileUrl) {
    m_downloadCtrl = new FileDownloader(fileUrl, this);
    connect(m_downloadCtrl, SIGNAL(downloaded()), this, SLOT(checkForUpdate()));
}

void MainWindow::createDialog() {
    qDialog = new QDialog();
    //qDialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    qDialog->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    label = new QLabel(qDialog);
    label->setText("Eine neue Version (" + onlineVersion  + ") ist verfügbar. Installierte Version: "  + localVersion);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(label, Qt::AlignCenter);
    button = new QPushButton(qDialog);
    button->setText("Update");
    vLayout->addWidget(button, Qt::AlignCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(startSetup()));
    qDialog->setWindowTitle(appName);
    qDialog->setLayout(vLayout);
    qDialog->open();
}

void MainWindow::checkForUpdate() {
    onlineVersion = m_downloadCtrl->downloadedData().replace("\n", "");
    if (localVersion != onlineVersion) {
        createDialog();
        Logger("New Version found " + onlineVersion.toStdString());
    }
}


void MainWindow::logNetworkIFace() {
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

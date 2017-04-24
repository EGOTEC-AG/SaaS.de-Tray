#include "bridgecontroller.h"
#include "mainwindow.h"

#include "helper/logger.h"

#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <signal.h>
    #include <unistd.h>
#endif

// #include <QWebInspector>

#include <QWebChannel>

#include <QDir>
#include <QHostInfo>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>


BridgeControllerWindow::BridgeControllerWindow(QWidget *parent) : MainWindow(parent) {
    createWebkitFrame(true);
}

void BridgeControllerWindow::createWebkitFrame(bool display) {
    // Einstellungen für den Browser setzen (LocalStrage/Session Storage/OfflineDatabase)
       QWebEngineSettings *s = QWebEngineSettings::globalSettings();
       //s->setAttribute(QWebEngineSettings::OfflineStorageDatabaseEnabled, true);
       //s->setOfflineStoragePath(qApp->applicationDirPath() + "/cache/" );
       //s->setAttribute(QWebEngineSettings::OfflineWebApplicationCacheEnabled, true);
       //s->setOfflineWebApplicationCachePath(qApp->applicationDirPath() + "/.saasde_cache/");
       s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
       //s->setLocalStoragePath(QDir::homePath() + "/.saasde_cache/");
       s->setAttribute(QWebEngineSettings::XSSAuditingEnabled, true);
       webView = new WebView(parent);
       webView->resize(UI_WIDTH, UI_HEIGHT);
       webView->load(QUrl(URL + "/timerecording/index.html?api=1"));
       webView->setWindowTitle(appName + " " + version);
       webView->setContextMenuPolicy(Qt::NoContextMenu);
       webView->setVisible(display);
       webView->setFixedSize(UI_WIDTH, UI_HEIGHT);

#ifdef _WIN32
       webView->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
#endif

       mainFrame = webView->page();

       connect(mainFrame, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished()));

       // webView->page()->settings()->setAttribute(QWebEngineSettings::DeveloperExtrasEnabled, true);

       // QWebInspector* in = new QWebInspector;
       // in->setPage(webView->page());
       // in->show();

       webView->show();

       createTrayicon();

#ifndef __APPLE__
       if(doesUserCacheExist == true) {
          webView->setVisible(false);
          systray->showMessage(appName, appRunningMinimized);
       }
#endif
}

void BridgeControllerWindow::onLoadFinished() {
    QString hostName = QHostInfo::localHostName();
    mainFrame->runJavaScript("window.Time.setDeviceName(\""+hostName+"\");");
    if (firstLogin == true) {
        mainFrame->runJavaScript("window.Time.startUpLogin()");
        firstLogin = false;

        Logger("startUpLogin (IMPORTANT)");
    }

    if (checkForChangedUrl()) {
        URL = saasversion;
    }

    QWebChannel* channel = new QWebChannel(mainFrame);
    mainFrame->setWebChannel(channel);
    channel->registerObject("api", this);
}

void BridgeControllerWindow::setUserData(QString employeeKey, QString version) {
    userKey = employeeKey;
    saasversion = version;

    Logger("setUserData");
}

void BridgeControllerWindow::setEmployeeState(QString state) {
    MainWindow::changeEmployeeState(state);
    MainWindow::state = state;

    Logger("setEmployeeState");
}

void BridgeControllerWindow::openSaas() {
    QDesktopServices::openUrl(QUrl(URL));

    Logger("openSaas");
}

void BridgeControllerWindow::openRegisterPage() {
    QDesktopServices::openUrl(QUrl(URL + "/?register=1"));

    Logger("openRegisterPage");
}

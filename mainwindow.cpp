#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bridgecontroller.h"

#include "json/JSON.h"
#include "json/JSONValue.h"

#include "helper/logger.h"

#include <QLocale>

#include <QApplication>
#include <QDesktopWidget>
#include <QWebEngineSettings>
#include <QSystemTrayIcon>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QByteArray>
#include <QHostInfo>
#include <QUrl>
#include <QDir>

#include <QString>
#include <QCloseEvent>
#include <iostream>

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#ifdef _WIN32
    #include <Windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qApp->setWindowIcon(QIcon(":/icon/tray.png"));

    URL = "https://desktop.saas.de";

    firstLogin = true;

    QDir userCache = QDir::homePath() + "/.saasde_cache/";
    if (userCache.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() != 0) {
        doesUserCacheExist = true;
    } else {
        doesUserCacheExist = false;
    }

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
#ifndef __APPLE__
    systray->setContextMenu (trayIconMenu);
#endif
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

void MainWindow::changeEmployeeState(QString userState) {
    if (userState == "true") {
        systray->setIcon(QIcon(":/icon/clock_stop.png"));
        qApp->setWindowIcon(QIcon(":/icon/clock_stop.png"));
        if (userState != state) {
            systray->showMessage(appName, comeMessage);
        }
    } else {
        systray->setIcon(QIcon(":/icon/tray.png"));
        qApp->setWindowIcon(QIcon(":/icon/tray.png"));
        if (userState != state) {
            systray->showMessage(appName, goMessage);
        }
    }
}

void MainWindow::comeGo(QSystemTrayIcon::ActivationReason e) {
    if (e == 3 && userKey != "") { // e == 3 == leftmouseclick
        //mainFrame->runJavaScript("window.Time.comeGo();");
         mainFrame->runJavaScript("window.loginComponentRef.taskAppCome();");
        Logger("leftclick trayicon comeGo (IMPORTANT)");
    }
}

void MainWindow::onQuit() {
    // sendQueue();
    mainFrame->runJavaScript("window.loginComponentRef.taskAppGo();");
    #ifdef _WIN32
        Sleep(1000);
        std::exit(EXIT_SUCCESS);
    #endif
}

 /*void MainWindow::sendQueue() {
    std::string postData = getQueue();

    QNetworkRequest req(QUrl(URL + "/rest/app/queue"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    req.setHeader(QNetworkRequest::ContentLengthHeader, (int) postData.size());

    QNetworkAccessManager http;
    QNetworkReply *netReply = http.post(req, QByteArray(postData.c_str()));

    QEventLoop loop;
    connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    Logger("sendQueue (IMPORTANT)");
}

std::string MainWindow::getQueue() {
    JSONArray queueList;
    JSONObject queueListItem;
    queueListItem[L"method"] = new JSONValue(L"go");
    queueListItem[L"device"] = new JSONValue(QHostInfo::localHostName().toStdWString());
    queueListItem[L"employee"] = new JSONValue(userKey.toStdWString());
    queueListItem[L"time"] = new JSONValue((double) std::time(0));
    queueList.push_back(new JSONValue(queueListItem));

    JSONValue* queueValue = new JSONValue(queueList);
    std::wstring str = queueValue->Stringify();
    delete queueValue;

    JSONObject data;
    data[L"queue"] = new JSONValue(str);
    data[L"time"] = new JSONValue((double) std::time(0));
    data[L"isTasksymbol"] = new JSONValue(true);

    JSONValue * value = new JSONValue(data);
    std::wstring JsonWStringData = value->Stringify();
    std::string JsonStringData(JsonWStringData.begin(), JsonWStringData.end());
    delete value;

    return JsonStringData;
} */

bool MainWindow::checkForChangedUrl() {
    if (saasversion != URL && saasversion != "null" && saasversion != "") {
        return true;
    } else {
        return false;
    }
}

QString MainWindow::getOSLanguage() {
    QLocale* sysInfo = new QLocale();
    return sysInfo->name();
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

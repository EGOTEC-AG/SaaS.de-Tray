#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef _WIN32
    #include <QtWidgets/QMainWindow>
#else
    #include <QMainWindow>
#endif

#include <QWebEnginePage>
#include <QWebEngineView>
#include <QSystemTrayIcon>

#ifdef _WIN32
#include <windows.h>
#endif

#include "webview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void changeEmployeeState(QString userState);

        void createTrayicon();
    protected:
        QString userKey;
        QString saasversion;
        QString state;

    protected:
        int UI_WIDTH;
        int UI_HEIGHT;

        QString URL;

        // Localized Strings
        QString appName;
        QString appRunningMinimized;
        QString come;
        QString comeMessage;
        QString go;
        QString goMessage;
        QString quit;

        // System Vars
        Ui::MainWindow *ui;
        WebView* webView;
        QWebEnginePage* mainFrame;
        QWidget *parent;

        bool firstLogin;
        bool doesUserCacheExist;

        QMenu* trayIconMenu;
        QAction* showHideAction;
        QAction* quitAction;
        QSystemTrayIcon* systray;

        // Methods
        void setLocalizedStrings();
        std::string getQueue();
        void sendQueue();
        bool checkForChangedUrl();
        QString getOSLanguage();
        void deleteOldWindow();
    private slots:
        void comeGo(QSystemTrayIcon::ActivationReason e);
        void showHideWindow();
    public slots:
        void onQuit();
};

#endif // MAINWINDOW_H

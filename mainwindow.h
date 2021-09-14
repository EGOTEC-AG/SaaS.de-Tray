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
#include <QSharedMemory>
#include <filedownloader.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "webview.h"
#include <QPushButton>
#include <QLabel>
#include <QDialog>

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
    void getLocalVersion();
    void logNetworkIFace();
protected:
    QString userKey;
    QString saasversion;
    QString state;
    QString localVersion;

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

    QMenu* trayIconMenu;
    QAction* showHideAction;
    QAction* quitAction;
    QSystemTrayIcon* systray;


    // Methods
    void setLocalizedStrings();
    QString getOSLanguage();
    void deleteOldWindow();
    void loadSettings();
    void saveSettings(QString url);

private:
    QDialog *qDialog;
    QPushButton *button;
    QLabel *label;
    FileDownloader* m_downloadCtrl;
    QString versionURL;
    QString onlineVersion;
    void downloadFile(QUrl fileUrl);
    void createDialog();
    QString setupFileName;


private slots:
    void comeGo(QSystemTrayIcon::ActivationReason e);
    void showHideWindow();
    void startSetup();
public slots:
    void onQuit(QString w);
    void checkForUpdate();

};

#endif // MAINWINDOW_H

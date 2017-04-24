#include "mainwindow.h"
#include <QApplication>

#include <QtWidgets/QApplication>
#include <Windows.h>

#include <QWidget>
#include "QSemaphore"
#include "bridgecontroller.h"
#include <QSharedMemory>
#include <QMessageBox>

#include <fstream>

#include <QNetworkProxy>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
const wchar_t g_szClassName[] = L"myWindowClass";

BridgeControllerWindow* w;
QSharedMemory sharedMemory;

int main(int argc, char *argv[]) {
    HINSTANCE hInstance = (HINSTANCE) 0;

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    g_szClassName,
    L"SaaSde Timerecording",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
    NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);

    QApplication a(argc, argv);
    sharedMemory.setNativeKey("{--SaaS.deTimerecordingApplication--}");
    if (!sharedMemory.create(sizeof(int))) {
        QMessageBox msgBox;
        msgBox.setText("This application is already running!");
        msgBox.exec();

        return 1;
    } else {
        w = new BridgeControllerWindow();
        w->setVisible(false);

        a.exec();
    }

    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        sharedMemory.unlock();
        w->onQuit();

        return Msg.wParam;
    }
    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_QUERYENDSESSION:
            sharedMemory.unlock();
            w->onQuit();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

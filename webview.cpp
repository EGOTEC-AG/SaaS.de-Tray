#include "webview.h"

WebView::WebView(QWidget *parent) :
    QWebEngineView(parent)
{
}

void WebView::changeEvent(QEvent* event) {
    //#ifndef __APPLE__
    event->ignore();
    if(event->type() == QEvent::WindowStateChange) {
        if(this->isVisible() && this->isMinimized()) {
            this->showNormal();
            this->setVisible(false);
        }
    }
    //#endif
}

void WebView::closeEvent(QCloseEvent * event) {
#ifdef __linux__
    event->ignore();
    this->setVisible(false);
#endif
}

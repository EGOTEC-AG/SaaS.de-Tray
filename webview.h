#ifndef TESTQWEBVIEW_H
#define TESTQWEBVIEW_H

#include <QWebEngineView>
#include <QEvent>
#include <QCloseEvent>

class WebView : public QWebEngineView {
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);
protected:
    void changeEvent(QEvent * event);
    void closeEvent(QCloseEvent * event);
};

#endif // TESTQWEBVIEW_H

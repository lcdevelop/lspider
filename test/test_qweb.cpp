/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 24 May 2015 06:01:49 PM CST
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <QApplication>
#include <QWebView>
#include <QWebElement>
#include <QWebFrame>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QUrl& url) {
        //QNetworkProxyFactory::setUseSystemConfiguration(true);
        view = new QWebView(this);
        view->load(url);
        connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    }
    
    virtual ~MainWindow(){}

protected slots:

    void adjustLocation() {
        printf("haha\n");
    }

private:
    QWebView *view;
};

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    QUrl url;
    if (argc > 1)
        url = QUrl(argv[1]);
    else
        url = QUrl("http://www.google.com/ncr");
    MainWindow *browser = new MainWindow(url);
    browser->show();
    return app.exec();
}

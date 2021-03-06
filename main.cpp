#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuickControls2/QQuickStyle>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QQmlContext>
#include "request.h"
#include "history.h"


int main(int argc, char** argv)
{
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::QueryPeer);
    config.setProtocol(QSsl::TlsV1_2);
    QSslConfiguration::setDefaultConfiguration(config);

    QGuiApplication app(argc, argv);
    app.setApplicationName("requester");
    app.setOrganizationName("eaton");

    QQuickStyle::setStyle("Fusion");
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    QQmlApplicationEngine engine;
    HistoryModel model;
    Request req(model);
    engine.rootContext()->setContextProperty("request", &req);
    engine.rootContext()->setContextProperty("history", &model);
    engine.addImportPath("qrc:/qml/");
    engine.load("qrc:/qml/main.qml");

    QQuickWindow* wind = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));
    if (!wind) {
        qWarning() << "No root window";
        exit(1);
    }
    wind->show();
    return app.exec();
}

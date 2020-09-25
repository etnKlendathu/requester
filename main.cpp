#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuickControls2/QQuickStyle>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QQmlContext>
#include "request.h"


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

    Request req;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("request", &req);
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

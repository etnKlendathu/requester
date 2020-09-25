#pragma once
#include <QString>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class Request: public QObject
{
    Q_OBJECT
public slots:
    void run(const QString& url, const QString& req, const QString& method, const QString& body, const QString& user, const QString& password);

signals:
    void success(const QString& url, const QString& method, const QString& status, const QString& text);
    void failure(const QString& url, const QString& method, const QString& status, const QString& text, const QString& errTxt);

private:
    bool auth(const QString& url, const QString& user, const QString& password);
    bool check(const QString& url);
    void onFinished(QNetworkReply* reply);
private:
    QString m_auth;
    QNetworkAccessManager* m_manager = nullptr;
};


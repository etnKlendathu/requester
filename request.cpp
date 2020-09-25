#include "request.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkDiskCache>

void Request::run(const QString& url, const QString& req, const QString& method, const QString& body, const QString& user, const QString& password)
{
    if (!m_manager) {
        m_manager = new QNetworkAccessManager(this);
    }

    if (!check(url)) {
        if (!auth(url, user, password)) {
            emit failure(url + req, method, "-1", "Cannot authenticate", "Cannot authenticate");
        }
    }

    QNetworkRequest request(url+req);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_auth).toUtf8());
    request.setRawHeader("Accept", "application/json, text/plain, */*");

    QNetworkReply* reply = nullptr;
    if (method == "GET") {
        reply = m_manager->get(request);
    }
    if (method == "POST") {
        reply = m_manager->post(request, body.toUtf8());
    }
    if (method == "DELETE") {
        reply = m_manager->deleteResource(request);
    }

    if (reply) {
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }
    onFinished(reply);
}

bool Request::auth(const QString& url, const QString& user, const QString& password)
{
    QNetworkRequest request(url+"/api/v1/oauth2/token");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36");

    QNetworkReply* reply = m_manager->post(request, QString("{\"username\": \"%1\", \"password\": \"%2\", \"grant_type\": \"password\"}").arg(user).arg(password).toUtf8());
    if (reply) {
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (reply->error() == QNetworkReply::NoError) {
        QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie>>(reply->header(QNetworkRequest::SetCookieHeader));
        if(cookies.count() != 0){
            //you must tell which cookie goes with which url
            m_manager->cookieJar()->setCookiesFromUrl(cookies, reply->url());
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_auth = doc["access_token"].toString();
        return true;
    } else {
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qWarning() << status_code.toString() << ": " << reply->readAll();
    }
    return false;
}

bool Request::check(const QString& url)
{
    if (m_auth.isEmpty()) {
        return false;
    }

    QNetworkRequest request(url+"/api/v1/admin/myuser");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_auth).toUtf8());
    QNetworkReply* reply = m_manager->get(request);
    if (reply) {
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (reply->error() != QNetworkReply::NoError) {
        return false;
    }

    return true;
}

void Request::onFinished(QNetworkReply* reply)
{
    auto readOp = [](QNetworkAccessManager::Operation op) {
        switch(op) {
            case QNetworkAccessManager::HeadOperation: return "HEAD";
            case QNetworkAccessManager::GetOperation: return "GET";
            case QNetworkAccessManager::PutOperation: return "PUT";
            case QNetworkAccessManager::PostOperation: return "POST";
            case QNetworkAccessManager::DeleteOperation: return "DELETE";
            case QNetworkAccessManager::CustomOperation: return "CUSTOM";
            case QNetworkAccessManager::UnknownOperation: return "UNKNOWN";
        }
    };

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (reply->error() == QNetworkReply::NoError) {
        emit success(reply->url().toString(), readOp(reply->operation()), status_code.toString(), reply->readAll());
    } else {
        emit failure(reply->url().toString(), readOp(reply->operation()), status_code.toString(), reply->readAll(), reply->errorString());
    }
}

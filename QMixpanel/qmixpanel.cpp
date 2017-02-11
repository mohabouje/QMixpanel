#include "qmixpanel.h"

#include "qmixpanelevent.h"
#include "qmixpanelprofile.h"
#include "util.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define API_ENTRY_ENGINE "http://api.mixpanel.com/engine/?data="
#define API_ENTRY_TRACK "http://api.mixpanel.com/track/?data="

QMixpanel *QMixpanel::_instance = Q_NULLPTR;
QMixpanel *QMixpanel::instance(QObject *object) {
    if (!_instance) {
        _instance = new QMixpanel(object);
        Q_ASSERT(_instance);
    }
    return _instance;
}

QMixpanel::QMixpanel(QObject* object) : QObject(object) {

}

void QMixpanel::setDistinctId(const QString &distinctId) {
    _distinctId = distinctId;
}

void QMixpanel::setToken(const QString &token) {
    _token = token;
}

void QMixpanel::insertProfile(const QMixpanelProfile &profile, bool instantSync) {
    if (instantSync) {
        postProfileHelper(profile);
    } else {
        _profileSet.insert(&profile);
    }
}

void QMixpanel::insertEvent(const QMixpanelEvent &event, bool instantSync) {
    if (instantSync) {
        postEventHelper(event);
    } else {
        _eventSet.insert(&event);
    }
}



void QMixpanel::flushProfiles() {
    _isSyncingProfiles = true;
    const ProfilesContainer tmp = _profileSet;
    const QJsonArray array = JsonHelper::ObjectSetToJsonArray<QMixpanelProfile>(tmp);
    QNetworkReply* reply = networkReplyHelper(API_ENTRY_ENGINE, JsonHelper::JsonArrayToJsonObject(array));
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _profileSet = _profileSet.subtract(tmp);
        }
        _isSyncingProfiles = false;
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, reply](const QNetworkReply::NetworkError& error){
        Q_UNUSED(error);
        _isSyncingProfiles = false;
        reply->deleteLater();
    });
}


void QMixpanel::flushEvents() {
    _isSyncingEvents = true;
    const EventsContainer tmp = _eventSet;
    const QJsonArray array = JsonHelper::ObjectSetToJsonArray<QMixpanelEvent>(tmp);
    QNetworkReply* reply = networkReplyHelper(API_ENTRY_TRACK, JsonHelper::JsonArrayToJsonObject(array));
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _eventSet = _eventSet.subtract(tmp);
        }
        _isSyncingEvents = false;
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, reply](QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        _isSyncingEvents = false;
        reply->deleteLater();
    });
}

void QMixpanel::postProfileHelper(const QMixpanelProfile& profile) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(&profile);
    QNetworkReply* reply = networkReplyHelper(API_ENTRY_ENGINE, obj);

    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [reply](const QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        reply->deleteLater();
    });
}

void QMixpanel::postEventHelper(const QMixpanelEvent& event) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(&event);
    QNetworkReply* reply = networkReplyHelper(API_ENTRY_TRACK, obj);

    connect(reply, &QNetworkReply::finished, [reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [reply](QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        reply->deleteLater();
    });
}


QNetworkReply* QMixpanel::networkReplyHelper(const QString &API, const QJsonObject &obj) {
    const QJsonDocument doc = QJsonDocument(obj);
    const QByteArray base64 = doc.toJson().toBase64();
    const QNetworkRequest request(API + base64);
    QNetworkAccessManager* netAccessManager = new QNetworkAccessManager(this);
    return netAccessManager->get(request);
}





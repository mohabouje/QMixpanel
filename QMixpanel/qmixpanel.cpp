#include "qmixpanel.h"

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

QString QMixpanel::distinctId() const
{
    return _distinctId;
}

void QMixpanel::setDistinctId(const QString &distinctId)
{
    _distinctId = distinctId;
}

QString QMixpanel::token() const
{
    return _token;
}

void QMixpanel::setToken(const QString &token)
{
    _token = token;
}

void QMixpanel::insertProfile(const QMixpanelProfile &profile, bool instantSync) {
    if (instantSync) {

    } else if (!_profileList.contains(profile)) {
        _profileList.append(profile);
    }
}

void QMixpanel::insertEvent(const QMixpanelEvent &event, bool instantSync) {
    if (instantSync) {

    } else if (!_profileList.contains(profile)) {
        _eventList.append(profile);
    }
}



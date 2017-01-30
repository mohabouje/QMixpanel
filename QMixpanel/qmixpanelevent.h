#ifndef QMIXPANELEVENT_H
#define QMIXPANELEVENT_H

#include "qmixpanel_global.h"

#include <QObject>
#include <QVariantMap>

class QMIXPANELSHARED_EXPORT QMixpanelEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString $event READ event WRITE setEvent)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)
    Q_PROPERTY(QVariantMap $properties READ properties WRITE setProperties)
public:
    explicit QMixpanelEvent(QObject *parent = 0) : QObject(parent) {}

    qlonglong time() const { return _time; }
    void setTime(const qlonglong &time) { _time = time; }

    QString event() const { return _event; }
    void setEvent(const QString &event) { _event = event; }

    QVariantMap properties() const {  return _properties; }
    void setProperties(const QVariantMap &properties) { _properties = properties; }
private:
    qlonglong       _time;
    QString         _event;
    QVariantMap     _properties;
};

#endif // QMIXPANELEVENT_H

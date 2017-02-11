#ifndef QMIXPANELPROFILE_H
#define QMIXPANELPROFILE_H

#include "qmixpanelevent.h"

#include <QObject>
#include <QVariantMap>
class QMixpanelProfile : public QMixpanelEvent {
    Q_OBJECT
    Q_PROPERTY(QVariantMap $set READ properties WRITE setProperties)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)
    Q_PROPERTY(QString $distinct_id READ distinctId WRITE setDistinctId)
    Q_PROPERTY(QString $token READ token WRITE setToken)
public:
    explicit QMixpanelProfile(QObject *parent = 0) : QMixpanelEvent(parent) { }

    inline QString name() const { return _properties.value("$name").toString(); }
    inline QString firstName() const { return _properties.value("$first_name").toString(); }
    inline QString lastName() const { return _properties.value("$last_name").toString(); }
    inline QString email() const { return _properties.value("$email").toString(); }
    inline QString phone() const { return _properties.value("$phone").toString(); }

    void setName(const QString& name) { _properties.insert("$name", name); }
    void setFirstName(const QString& firstName) { _properties.insert("$first_name", firstName); }
    void setLastName(const QString& lastName) { _properties.insert("$last_name", lastName); }
    void setEmail(const QString& email) { _properties.insert("$email", email); }
    void setPhone(const QString& phone) { _properties.insert("$phone", phone); }
};

#endif // QMIXPANELPROFILE_H

#ifndef QMIXPANELPROFILE_H
#define QMIXPANELPROFILE_H

#include "qmixpanel_global.h"

#include <QObject>

class QMIXPANELSHARED_EXPORT QMixpanelProfile : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString $name READ name WRITE setName)
    Q_PROPERTY(QString $first_name READ firstName WRITE setFirstName)
    Q_PROPERTY(QString $last_name READ lastName WRITE setLastName)
    Q_PROPERTY(QString $email READ email WRITE setEmail)
    Q_PROPERTY(QString $phone READ phone WRITE setPhone)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)

public:
    explicit QMixpanelProfile(QObject *parent = 0) : QObject(parent) { }

    QString email() const { return _email; }
    void setEmail(const QString &email) { _email = email; }

    QString firstName() const { return _firstName; }
    void setFirstName(const QString &firstName) { _firstName = firstName; }

    QString lastName() const { return _lastName; }
    void setLastName(const QString &lastName) { _lastName = lastName; }

    QString name() const { return _name; }
    void setName(const QString &name) { _name = name; }

    QString phone() const { return _phone; }
    void setPhone(const QString &phone) { _phone = phone; }

    qlonglong time() const { return _time; }
    void setTime(const qlonglong &time) { _time = time; }

private:
    QString     _email;
    QString     _firstName;
    QString     _lastName;
    QString     _name;
    QString     _phone;
    qlonglong   _time;

};

#endif // QMIXPANELPROFILE_H

#ifndef QMIXPANELPROFILE_H
#define QMIXPANELPROFILE_H

#include <QObject>
class QMixpanelProfile : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString $name READ name WRITE setName)
    Q_PROPERTY(QString $first_name READ firstName WRITE setFirstName)
    Q_PROPERTY(QString $last_name READ lastName WRITE setLastName)
    Q_PROPERTY(QString $email READ email WRITE setEmail)
    Q_PROPERTY(QString $phone READ phone WRITE setPhone)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)
public:
    explicit QMixpanelProfile(QObject *parent = 0) : QObject(parent) { }

    inline QString email() const { return _email; }
    inline QString firstName() const { return _firstName; }
    inline QString lastName() const { return _lastName; }
    inline QString name() const { return _name; }
    inline QString phone() const { return _phone; }
    inline qlonglong time() const { return _time; }
    inline bool operator==(const QMixpanelProfile& other) const {
        return other.time() == _time
                && other.email() == _email
                && other.firstName() == _firstName
                && other.lastName() == _lastName
                && other.name() == _name
                && other.phone() == _phone;
    }

    inline bool operator<(const QMixpanelProfile& other) const {
        return _time < other.time();
    }

    void setEmail(const QString &email) { _email = email; }
    void setFirstName(const QString &firstName) { _firstName = firstName; }
    void setLastName(const QString &lastName) { _lastName = lastName; }
    void setName(const QString &name) { _name = name; }
    void setPhone(const QString &phone) { _phone = phone; }
    void setTime(const qlonglong &time) { _time = time; }
private:
    QString     _email;
    QString     _firstName;
    QString     _lastName;
    QString     _name;
    QString     _phone;
    qlonglong   _time;

};

inline uint qHash(const QMixpanelProfile& other) {
    return other.time();
}

#endif // QMIXPANELPROFILE_H

#ifndef QMIXPANELPROFILE_H
#define QMIXPANELPROFILE_H

#include <QObject>
#include <QVariantMap>
class QMixpanelProfile : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap $set READ properties WRITE setProperties)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)
    Q_PROPERTY(QString $distinct_id READ distinctId WRITE setDistinctId)
    Q_PROPERTY(QString $token READ token WRITE setToken)
public:
    explicit QMixpanelProfile(QObject *parent = 0) : QObject(parent) { }

    inline qlonglong time() const { return _time;}
    inline QVariantMap properties() const { return _properties; }
    inline QString distinctId() const { return _distinctId; }
    inline QString token() const { return _token; }
    inline bool operator==(const QMixpanelProfile& other) const {
        return other.time() == _time
                && other.properties() == _properties
                && other.distinctId() == _distinctId
                && other.token() == _token;
    }

    inline bool operator<(const QMixpanelProfile& other) const {
        return _time < other.time();
    }

    void setProperties(const QVariantMap &extraData) { _properties = extraData; }
    void setDistinctId(const QString &distinctId) { _distinctId = distinctId; }
    void setToken(const QString &token) { _token = token; }
    void setTime(const qlonglong &time) { _time = time; }
private:
    qlonglong       _time;
    QString         _token;
    QString         _distinctId;
    QVariantMap     _properties;

};

inline uint qHash(const QMixpanelProfile& other) {
    return other.time();
}

#endif // QMIXPANELPROFILE_H

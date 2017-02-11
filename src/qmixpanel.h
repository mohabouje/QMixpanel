#ifndef QMIXPANEL_H
#define QMIXPANEL_H

#include <QNetworkReply>
#include <QObject>
#include <QSet>
#include <QTimer>

class QMixpanelProfile;
class QMixpanelEvent;
class QMixpanel : public QObject {
    Q_OBJECT
public:
    static QMixpanel* instance(QObject* object = Q_NULLPTR);
    static const char *EngineApi;
    static const char *TrackApi;

    inline bool isSyncingEvents() const { return _isSyncingEvents; }
    inline bool isSyncingProfiles() const { return _isSyncingProfiles; }
    inline QTimer *timer() const { return _timer; }

public slots:
    bool insertProfile(const QMixpanelProfile& profile, bool instantSync = false);
    bool insertEvent(const QMixpanelEvent& event, bool instantSync = false);

private slots:
    void flushEvents();
    void flushProfiles();

private:
    using ProfilesContainer = QSet<const QMixpanelProfile*>;
    using EventsContainer = QSet<const QMixpanelEvent*>;

    explicit QMixpanel(QObject* object = Q_NULLPTR);
    static QMixpanel*   _instance;
    static const int MaxEventCount = 50;

    bool                    _isSyncingEvents;
    bool                    _isSyncingProfiles;
    ProfilesContainer       _profileSet;
    EventsContainer         _eventSet;
    QTimer*                 _timer;

    bool postEventHelper(const QMixpanelEvent &event);
    bool postProfileHelper(const QMixpanelProfile &event);
    QNetworkReply *networkReplyHelper(const QString& API, const QJsonObject& data);
};


class QMixpanelEvent : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString event READ event WRITE setEvent)
    Q_PROPERTY(QString distinct_id READ distinctId WRITE setDistinctId)
    Q_PROPERTY(QString token READ token WRITE setToken)
    Q_PROPERTY(qlonglong time READ time WRITE setTime)
    Q_PROPERTY(QVariantMap properties READ properties WRITE setProperties)
public:
    explicit QMixpanelEvent(QObject *parent = 0) : QObject(parent) {}
    inline qlonglong time() const { return _time; }
    inline QVariantMap properties() const {  return _properties; }
    inline QString event() const { return _event; }
    inline QString token() const { return _token; }
    inline QString distinctId() const { return _distinctId; }
    inline bool operator==(const QMixpanelEvent& other) const {
        return other.time() == _time
                && other.distinctId() == _distinctId
                && other.token() == _token;
    }
    inline bool operator<(const QMixpanelEvent& other) const {
        return _time < other.time();
    }

    void setTime(const qlonglong &time) { _time = time; }
    void setEvent(const QString &event) { _event = event; }
    void setToken(const QString &token) { _token = token; }
    void setDistinctId(const QString &distinctId) { _distinctId = distinctId; }
    void setProperties(const QVariantMap &properties) {
        for (QVariantMap::const_iterator iter = properties.begin(); iter != properties.end(); ++iter) {
            _properties.insert(iter.key(), iter.value());
        }
    }
protected:
    qlonglong       _time;
    QString         _event;
    QString         _token;
    QString         _distinctId;
    QVariantMap     _properties;
};

inline uint qHash(const QMixpanelEvent& other) {
    return other.time();
}


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
    inline QDateTime creationDate() const { return QDateTime::fromString(_properties.value("$created").toString(), Qt::ISODate); }

    void setName(const QString& name) { _properties.insert("$name", name); }
    void setFirstName(const QString& firstName) { _properties.insert("$first_name", firstName); }
    void setLastName(const QString& lastName) { _properties.insert("$last_name", lastName); }
    void setEmail(const QString& email) { _properties.insert("$email", email); }
    void setPhone(const QString& phone) { _properties.insert("$phone", phone); }
    void setCreationDate(const QDateTime& dateTime) { _properties.insert("$created", dateTime.toString(Qt::ISODate)); }
};
#endif // QMIXPANEL_H

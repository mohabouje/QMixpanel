#ifndef QMIXPANEL_H
#define QMIXPANEL_H

#include <QNetworkReply>
#include <QObject>
#include <QSet>

class QMixpanelProfile;
class QMixpanelEvent;
class QMixpanel : public QObject {
    Q_OBJECT
public:
    static QMixpanel* instance(QObject* object = Q_NULLPTR);

    inline QString token() const { return _token; }
    inline QString distinctId() const { return _distinctId;}
    inline bool isSyncingEvents() const { return _isSyncingEvents; }
    inline bool isSyncingProfiles() const { return _isSyncingProfiles; }

    void insertProfile(const QMixpanelProfile& profile, bool instantSync = false);
    void insertEvent(const QMixpanelEvent& event, bool instantSync = false);
    void setToken(const QString &token);
    void setDistinctId(const QString &distinctId);

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
    QString                 _token;
    QString                 _distinctId;
    ProfilesContainer       _profileSet;
    EventsContainer         _eventSet;



    void postEventHelper(const QMixpanelEvent &event);
    void postProfileHelper(const QMixpanelProfile &event);
    QNetworkReply *networkReplyHelper(const QString& API, const QJsonObject& data);
};





#endif // QMIXPANEL_H

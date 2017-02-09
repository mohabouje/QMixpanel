#ifndef QMIXPANEL_H
#define QMIXPANEL_H

#include "qmixpanel_global.h"
#include "qmixpanelevent.h"
#include "qmixpanelprofile.h"

#include <QObject>





class QMIXPANELSHARED_EXPORT QMixpanel : public QObject {
    Q_OBJECT
public:
    static QMixpanel* instance(QObject* object = Q_NULLPTR);

    void insertProfile(const QMixpanelProfile& profile, bool instantSync = false);
    void insertEvent(const QMixpanelEvent& event, bool instantSync = false);

    QString token() const;
    void setToken(const QString &token);

    QString distinctId() const;
    void setDistinctId(const QString &distinctId);

private slots:
    void flushEvents();
    void flushProfiles();
private:
    typedef QSet<QMixpanelProfile> ProfileSet;
    typedef QSet<QMixpanelEvent>   EventSet;

    explicit QMixpanel(QObject* object = Q_NULLPTR);
    static QMixpanel*   _instance;
    static const int MaxEventCount = 50;
    bool            _isSyncingEvents;
    bool            _isSyncingProfiles;
    QString         _token;
    QString         _distinctId;
    ProfileSet      _profileSet;
    EventSet        _eventSet;



    void postEventHelper(const QMixpanelEvent &event);
    void postProfileHelper(const QMixpanelProfile &event);
    void networkReplyHelper(const QString& API, const QJsonObject& data);
};





#endif // QMIXPANEL_H

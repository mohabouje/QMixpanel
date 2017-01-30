#ifndef QMIXPANEL_H
#define QMIXPANEL_H

#include "qmixpanel_global.h"
#include "qmixpanelevent.h"
#include "qmixpanelprofile.h"

#include <QObject>

typedef QList<QMixpanelProfile> ProfileList;
typedef QList<QMixpanelEvent>   EventList;

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

private:
    explicit QMixpanel(QObject* object = Q_NULLPTR);
    static QMixpanel*   _instance;

    QString         _token;
    QString         _distinctId;
    ProfileList     _profileList;
    EventList       _eventList;
};





#endif // QMIXPANEL_H

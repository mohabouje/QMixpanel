#ifndef QMIXPANELEVENT_H
#define QMIXPANELEVENT_H
#include <QObject>
#include <QVariantMap>

class QMixpanelEvent : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString $event READ event WRITE setEvent)
    Q_PROPERTY(qlonglong $time READ time WRITE setTime)
    Q_PROPERTY(QVariantMap $properties READ properties WRITE setProperties)
public:
    explicit QMixpanelEvent(QObject *parent = 0) : QObject(parent) {}
    inline qlonglong time() const { return _time; }
    inline QString event() const { return _event; }
    inline QVariantMap properties() const {  return _properties; }
    inline bool operator==(const QMixpanelEvent& other) const {
        return other.time() == _time
                && other.event() == _event
                && other.properties() == _properties;
    }
    inline bool operator<(const QMixpanelEvent& other) const {
        return _time < other.time();
    }

    void setTime(const qlonglong &time) { _time = time; }
    void setEvent(const QString &event) { _event = event; }
    void setProperties(const QVariantMap &properties) { _properties = properties; }
private:
    qlonglong       _time;
    QString         _event;
    QVariantMap     _properties;
};

inline uint qHash(const QMixpanelEvent& other) {
    return other.time();
}

#endif // QMIXPANELEVENT_H

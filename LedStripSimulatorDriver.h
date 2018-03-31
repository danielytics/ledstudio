#ifndef LEDSTRIPSIMULATORSCHEDULER_H
#define LEDSTRIPSIMULATORSCHEDULER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>

#include "LedStripSimulator.h"
#include "Effect.h"

#include <chrono>

class LedStripSimulatorDriver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool started READ started WRITE setStarted NOTIFY startedChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(LedStripSimulator* ledStrip READ ledStrip WRITE setLedStrip)
    Q_PROPERTY(QList<QString> effects READ effects NOTIFY effectsChanged)
    Q_PROPERTY(QList<QString> easingFunctions READ easingFunctions NOTIFY easingFunctionsChanged)
    Q_PROPERTY(QList<QString> blendModes READ blendModes NOTIFY blendModesChanged)
public:
    explicit LedStripSimulatorDriver(QObject *parent = nullptr);
    ~LedStripSimulatorDriver();

    Q_INVOKABLE void init();
    Q_INVOKABLE void seek(float time);
    Q_INVOKABLE QMap<QString, QVariant> parametersForEffect(const QString& effectName);
    Q_INVOKABLE void loadFromJson(const QString& jsonString);
    Q_INVOKABLE QString readFromFile (bool homeDir, const QString& fileName);
    Q_INVOKABLE void writeToFile (bool homeDir, const QString& fileName, const QString content);

    bool started();
    void setStarted(bool started);
    bool paused();
    void setPaused(bool paused);
    QList<QString> effects();
    QList<QString> easingFunctions();
    QList<QString> blendModes();

    LedStripSimulator* ledStrip();
    void setLedStrip(LedStripSimulator* ledStrip);

    static void registerTypes();

signals:
    void startedChanged(bool started);
    void pausedChanged(bool paused);
    void currentTimeChanged(float currentTime);
    void effectsChanged(const QList<QString>& effects);
    void easingFunctionsChanged(const QList<QString>& easingFunctions);
    void blendModesChanged(const QList<QString>& blendModes);

public slots:
protected:
    void timerEvent(QTimerEvent* event);

private:
    bool hasStarted;
    bool isPaused;

    LedStripSimulator* simulator;
    class Compositor* compositor;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point pauseStartTime;
    std::map<std::string, EffectID> effectsMap;
    static QList<QString> blendModesList;
    static QList<QString> easingFunctionsList;

    void populateEasingFunctionsMap();
    void populateEasingFunctionsList();
};

#endif // LEDSTRIPSIMULATORSCHEDULER_H

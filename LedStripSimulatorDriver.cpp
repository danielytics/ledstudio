#include "LedStripSimulatorDriver.h"
#include <QQmlApplicationEngine>
#include <QMap>
#include <QDir>

#include "Compositor.h"
#include "SparkleEffectRenderer.h"
#include "FadeEffectRenderer.h"
#include "LightupEffectRenderer.h"
#include "SwipeEffectRenderer.h"

QList<QString> LedStripSimulatorDriver::easingFunctionsList;
QList<QString> LedStripSimulatorDriver::blendModesList;

// Ordered list of easing functions
#define TOSTR(x) #x
#define DECLARE_EASING(name, function) easingFunctionsList.append(TOSTR(name));
#include "easing.h"
void LedStripSimulatorDriver::populateEasingFunctionsList() {
    easingFunctionsList.clear();
    GENERATE_EASING_DECLARATIONS
    emit easingFunctionsChanged(easingFunctionsList);
}
#undef DECLARE_EASING
#undef GENERATE_EASING_DECLARATIONS
#undef TOSTR

LedStripSimulatorDriver::LedStripSimulatorDriver(QObject *parent)
    : QObject(parent)
    , hasStarted(false)
    , isPaused(false)
    , simulator(nullptr)
    , compositor(nullptr)
{

}

LedStripSimulatorDriver::~LedStripSimulatorDriver()
{
    delete compositor;
}


void LedStripSimulatorDriver::init()
{
    compositor = new Compositor(simulator);

    effectsMap.clear();
    effectsMap["sparkle"] = compositor->registerEffect(new SparkleEffectRenderer);
    effectsMap["fade"] = compositor->registerEffect(new FadeEffectRenderer);
    effectsMap["lightup"] = compositor->registerEffect(new LightupEffectRenderer);
    effectsMap["swipe"] = compositor->registerEffect(new SwipeEffectRenderer);
    emit effectsChanged(effects());

    blendModesList.clear();
    blendModesList.append("Normal");
    blendModesList.append("Screen");
    blendModesList.append("Addition");
    blendModesList.append("Subtract");
    blendModesList.append("Multiply");
    blendModesList.append("Divide");
    blendModesList.append("Average");
    blendModesList.append("Difference");
    blendModesList.append("Hue");
    blendModesList.append("Saturation");
    blendModesList.append("Lightness");
    blendModesList.append("White");
    blendModesList.append("Color");
    blendModesList.append("Dodge");
    blendModesList.append("Burn");
    emit blendModesChanged(blendModesList);

    populateEasingFunctionsList();

    startTimer(1, Qt::PreciseTimer);
}

bool LedStripSimulatorDriver::started()
{
    return hasStarted;
}

void LedStripSimulatorDriver::setStarted(bool started)
{
    if (started != hasStarted) {
        hasStarted = started;
        if (started) {
            compositor->startCompositing();
            startTime = std::chrono::high_resolution_clock::now();
        }
        emit startedChanged(started);
    }
}

bool LedStripSimulatorDriver::paused()
{
    return isPaused;
}

void LedStripSimulatorDriver::setPaused(bool paused)
{
    if (paused != isPaused) {
        isPaused = paused;
        if (paused) {
            pauseStartTime = std::chrono::high_resolution_clock::now();
        } else {
            auto unpauseTime = std::chrono::high_resolution_clock::now();
            auto pauseDuration = unpauseTime - pauseStartTime;
            // Advance the start time forward to pretend the pause never happened
            startTime = startTime + pauseDuration;
        }
        emit pausedChanged(paused);
    }
}

LedStripSimulator* LedStripSimulatorDriver::ledStrip()
{
    return simulator;
}

void LedStripSimulatorDriver::setLedStrip(LedStripSimulator* ledStrip)
{
    simulator = ledStrip;
}

QList<QString> LedStripSimulatorDriver::effects()
{
    QList<QString> effects;
    for (auto kv : effectsMap) {
        effects.append(QString::fromStdString(kv.first));
    }
    return effects;
}

QList<QString> LedStripSimulatorDriver::easingFunctions()
{
    return easingFunctionsList;
}

QList<QString> LedStripSimulatorDriver::blendModes()
{
    return blendModesList;
}

void LedStripSimulatorDriver::seek(float time)
{
    startTime = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(int(time * 1000));
    if (!hasStarted) {
        hasStarted = true;
        isPaused = true;
        emit pausedChanged(true);
    }
    if (isPaused) {
        pauseStartTime = std::chrono::high_resolution_clock::now();
    }
    emit currentTimeChanged(time);
}

QString LedStripSimulatorDriver::readFromFile(bool homeDir, const QString& fileName)
{
    QDir dir;
    if (homeDir) {
        dir = QDir(QDir::homePath());
    }
    QFile file(dir.absoluteFilePath(fileName));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        // error
        return "";
    }
    QTextStream in(&file);
    return in.readAll();
}

void LedStripSimulatorDriver::writeToFile (bool homeDir, const QString& fileName, const QString content)
{
    QDir dir;
    if (homeDir) {
        dir = QDir(QDir::homePath());
    }
    QFile file(dir.absoluteFilePath(fileName));
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
         QTextStream stream(&file);
         stream << content;
         file.close();
    }
}

QMap<QString, QVariant> LedStripSimulatorDriver::parametersForEffect(const QString& effectName)
{
    auto id = effectsMap.find(effectName.toStdString());
    if (id == effectsMap.end()) {
        return {};
    }
    EffectRenderer* effect = compositor->getEffect(id->second);
    Params params = effect->parameters();
    QMap<QString,QVariant> result;
    for (auto kv : params) {
        result.insert(QString::fromStdString(kv.first), QVariant(kv.second));
    }
    return result;
}

void LedStripSimulatorDriver::loadFromJson(const QString& jsonString)
{
    compositor->load(effectsMap, jsonString.toStdString());
}

void LedStripSimulatorDriver::timerEvent(QTimerEvent*)
{
    if (hasStarted && !isPaused) {
        auto currentTicks = std::chrono::high_resolution_clock::now();
        float seconds = std::chrono::duration<float>(currentTicks - startTime).count();
        if (seconds > 60.0f * 4.0f) {
            setStarted(false);
            return;
        }
        emit currentTimeChanged(seconds);
        compositor->update(seconds);
    }
}

void LedStripSimulatorDriver::registerTypes()
{
    qmlRegisterType<LedStripSimulatorDriver>("leds", 1, 0, "LedStripDriver");
}

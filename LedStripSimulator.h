#ifndef LEDSTRIPSIMULATOR_H
#define LEDSTRIPSIMULATOR_H

#include <QQuickPaintedItem>
#include "LedStrip.h"


class LedStripSimulator : public QQuickPaintedItem, public LedStrip
{
    Q_OBJECT
public:
    explicit LedStripSimulator(QQuickItem* parent = nullptr);
    ~LedStripSimulator();

    void paint(QPainter *painter);

    void init();
    void term();

    void reset();
    void refresh(const std::vector<NativeColor>& pixels);

    static void registerTypes();

signals:

public slots:

private:
    std::vector<QColor> pixelBuffer;
};

#endif // LEDSTRIPSIMULATOR_H

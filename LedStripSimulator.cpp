#include "LedStripSimulator.h"
#include <QPainter>
#include <QQmlApplicationEngine>
#include <algorithm>

LedStrip::~LedStrip() {}

LedStripSimulator::LedStripSimulator(QQuickItem* parent) : QQuickPaintedItem(parent)
{

}

LedStripSimulator::~LedStripSimulator()
{

}

void LedStripSimulator::init()
{
    // Hook up application here
}

void LedStripSimulator::term()
{
    // No-op in simulator
}

void LedStripSimulator::reset()
{
    std::fill_n(pixelBuffer.begin(), NUM_PIXELS, QColor(0, 0, 0, 0));
}

void LedStripSimulator::refresh(const std::array<NativeColor, NUM_PIXELS>&& pixels)
{
    std::transform(pixels.begin(), pixels.end(), pixelBuffer.begin(),
                       [](NativeColor c) -> QColor {
        // Alpha used as white, converted to white outline in paint(...)
        return QColor(NATIVE_GET_RED(c), NATIVE_GET_GREEN(c), NATIVE_GET_BLUE(c), NATIVE_GET_WHITE(c));
    });
    update();
}

void LedStripSimulator::paint(QPainter* painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QSizeF itemSize = size();
    int pixelSize = std::min(int(itemSize.width() / 19.0), int(itemSize.height() / 15.0));
    QSize size = {pixelSize, pixelSize};

    const std::vector<QPoint> positions {
        {6, 0}, {7, 0}, {8, 0}, // 0
        {8, 1}, {7, 1}, {6, 1}, // 3
        {6, 2}, {7, 2}, {8, 2}, // 6
        {8, 3}, {7, 3}, {6, 3}, // 9
        {6, 4}, {7, 4}, {8, 4}, // 12
        {8, 5}, {7, 5}, {6, 5}, // 15
        {6, 6}, {7, 6}, {8, 6}, // 18
        {8, 7}, {7, 7}, {6, 7}, // 21
        {6, 8}, {7, 8}, {8, 8}, // 24
        {8, 9}, {7, 9}, {6, 9}, // 27
        {6, 10}, {7, 10}, {7, 11}, // 30
        {6, 12}, {6, 11}, {5, 13}, // 33
        {5, 12}, {5, 11}, {4, 11}, // 36
        {4, 12}, {4, 13}, {3, 13}, // 39
        {3, 12}, {2, 12}, {2, 11}, // 42
        {3, 11}, {3, 10}, {2, 10}, // 45
        {1, 10}, {1, 9}, {2, 9}, // 48
        {3, 9}, // 51
        {18, 4}, {17, 4}, {16, 4}, //52
        {16, 3}, {17, 3}, {18, 3}, // 55
        {17, 2}, {16, 2}, {15, 3}, // 58
        {16, 1}, {16, 0}, {15, 0}, // 61
        {15, 1}, {15, 2}, {14, 2}, // 64
        {14, 1}, {14, 0}, {13, 1}, // 67
        {13, 2}, // 70
        {13, 3}, {12, 3}, {11, 3}, // 71
        {10, 4}, {11, 4}, {12, 4},
        {12, 5}, {11, 5}, {10, 5},
        {10, 6}, {11, 6}, {12, 6},
        {12, 7}, {11, 7}, {10, 7},
        {10, 8}, {11, 8}, {12, 8},
        {12, 9}, {11, 9}, {10, 9},
        {10, 10}, {11, 10}, {12, 10},
        {13, 11}, {12, 11}, {11, 11},
        {12, 12}, {13, 12}, {14, 11},
        {14, 12}, {14, 13}, {15, 13},
        {15, 12}, {15, 11}, {16, 12},
        {16, 13}, {17, 12}, {17, 11},
        {16, 11}, {16, 10}, {17, 10},
        {18, 10}, {18, 9}, {17, 9},
        {16, 9}, {14, 8}, {14, 7},
        {14, 6}, {15, 6}, {15, 7},
        {15, 8}, {16, 8}, {17, 7},
        {16, 6}, {17, 6}, {16, 7},
        {17, 8}, {18, 8}, {18, 7},
        {18, 6}
    };

    painter->setFont(QFont("Arial", 10));
    std::size_t index = 0;
    for (auto color : pixelBuffer) {
        QPen pen(QColor(color.alpha(), color.alpha(), color.alpha()));
        pen.setWidth(2);
        pen.setStyle(Qt::PenStyle::SolidLine);
        painter->setPen(pen);
        painter->setBrush(QColor(color.red(), color.green(), color.blue()));
        QPoint pos = positions.at(index);
        painter->drawRect(QRect({pos.x() * pixelSize, pos.y() * pixelSize}, size));
        painter->setPen(QColor(255 - color.red(), 255 - color.green(), 255 - color.blue()));
        painter->drawText(QPoint(2 + (pos.x() * pixelSize), (pixelSize * 0.7) + (pos.y() * pixelSize)), QString("%1").arg(index));
        ++index;
    }

}

void LedStripSimulator::registerTypes()
{
    qmlRegisterType<LedStripSimulator>("leds", 1, 0, "LedStrip");
}

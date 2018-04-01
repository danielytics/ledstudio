#include "SwipeEffectRenderer.h"
#include <set>
#include <algorithm>


std::vector<LedsTable> SwipeEffectRenderer::pixelMapColumns {
    {49, 48, 43},
    {50, 47, 44, 41},
    {51, 46, 45, 42},
    {38, 39, 40},
    {37, 36, 35},
    {0, 5, 6, 11, 12, 17, 18, 23, 24, 29, 30, 34},
    {1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 33},
    {2, 3, 8, 9, 14, 15, 20, 21, 26, 27},
    {74, 79, 80, 85, 86, 91, 92},
    {73, 97},
    {69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 98},
    {70, 71, 76, 77, 82, 83, 88, 89, 94, 95, 99},
    {68, 67, 66, 100, 101, 102},
    {63, 64, 65, 119, 118, 117},
    {61, 60, 120, 121, 122, 105, 104, 103},
    {62, 59, 55, 54, 125, 128, 123, 116, 111, 110, 106},
    {58, 56, 53, 126, 124, 128, 115, 112, 109, 107},
    {57, 52, 131, 130, 129, 114, 113, 108}
};
std::vector<LedsTable> SwipeEffectRenderer::pixelMapRows {
    {0, 1, 2, 68, 63},
    {5, 4, 3, 73, 70, 67, 64, 61, 58},
    {6, 7, 8, 72, 71, 66, 65, 60, 59},
    {11, 10, 9, 74, 75, 76, 55, 56, 57},
    {12, 13, 14, 79, 78, 77, 54, 53, 52},
    {17, 16, 15, 80, 81, 82, 119, 120, 125, 126, 131},
    {18, 19, 20, 85, 84, 83, 118, 121, 127, 124, 130},
    {23, 22, 21, 86, 87, 88, 117, 122, 123, 128, 129},
    {49, 50, 51, 24, 25, 26, 91, 90, 89, 116, 115, 114},
    {48, 47, 46, 29, 28, 27, 92, 93, 94, 111, 112},
    {44, 45, 38, 37, 30, 31, 95, 96, 100, 105, 110, 109, 113},
    {43, 42, 39, 36, 34, 32, 97, 99, 101, 104, 106},
    {41, 40, 35, 33, 98, 102, 103, 107}
};

SwipeEffectRenderer::SwipeEffectRenderer()
{

}

Params SwipeEffectRenderer::parameters()
{
    return {
      {"x", 0},
      {"y", 0},
//      {"trail", 1}
    };
}

EffectHandle SwipeEffectRenderer::instance(const Params& params)
{
    EffectHandle handle = nextFreeHandle++;
    float moveX = get(params, "x");
    float moveY = get(params, "y");
//    int trail = int(get(params, "trail") + F_LIMIT);
    int trail = 1; // Not yet supported
    state.emplace(std::make_pair(handle, SwipeState{moveX, moveY, trail, 0}));
    return handle;
}

void SwipeEffectRenderer::dispose(EffectHandle handle)
{
    state.erase(handle);
}

void SwipeEffectRenderer::render(EffectHandle handle, float percentage, float, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine)
{
    const std::size_t unused = 99999;
    std::size_t currentIndex;
    SwipeState& s = state[handle];

    // Determine which row and column based on percentage
    std::size_t currentColumn = unused;
    currentIndex = std::size_t(percentage * float(pixelMapColumns.size()));
    if (s.moveX > 0.0f) {
        currentColumn = currentIndex;
    } else if (s.moveX < 0.0f) {
        currentColumn = pixelMapColumns.size() - (currentIndex + 1);
    }
    std::size_t currentRow = unused;
    currentIndex = std::size_t(percentage * float(pixelMapRows.size()));
    if (s.moveY > 0.0f) {
        currentRow = currentIndex;
    } else if (s.moveY < 0.0f) {
        currentRow = pixelMapRows.size() - (currentIndex + 1);
    }

    // Determine which leds based on currentColumn and currentRow
    std::set<Pixel> currentPixels;
    if (currentColumn < pixelMapColumns.size()) {
        LedsTable& columns = pixelMapColumns[currentColumn];
        std::copy(columns.begin(), columns.end(), std::inserter(currentPixels, currentPixels.begin()));
    }
    if (currentRow < pixelMapRows.size()) {
        LedsTable& rows = pixelMapRows[currentRow];
        std::copy(rows.begin(), rows.end(), std::inserter(currentPixels, currentPixels.begin()));
    }

    // Only use the leds that are also in the pixels table
    LedsTable activePixels;
    std::set_intersection(currentPixels.begin(), currentPixels.end(),
                          pixels.begin(), pixels.end(),
                          std::back_inserter(activePixels));

    // Draw, fading between colors
    const Color& current = colorTable[s.current];
    if (colorTable.size() == 1) {
        engine->plot(activePixels, Color().lerp(current, percentage));
    } else if (s.current + 1 < colorTable.size()) {
        const Color& next = colorTable[s.current + 1];
        engine->plot(activePixels, current.lerp(next, percentage));
    }
    if (percentage >= 0.95f) {
        s.current = (s.current + 1) % colorTable.size();
    }
}

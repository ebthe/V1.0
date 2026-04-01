// Plugins/Charts/UltraCanvasJitterPlotElement.cpp
// Jitter Plot Element implementation with random horizontal jitter
// Version: 1.0.2
// Last Modified: 2026-03-31
// Author: UltraCanvas Framework

#include "Plugins/Charts/UltraCanvasJitterPlotElement.h"
#include <cmath>
#include <algorithm>

namespace UltraCanvas {

    void UltraCanvasJitterPlotElement::GenerateJitterCache() {
        if (!dataSource || dataSource->GetPointCount() == 0) {
            cachedJitterValues.clear();
            return;
        }

        cachedJitterValues.resize(dataSource->GetPointCount());
        
        float jitterOffset = jitterAmount * cachedPlotArea.width * 0.5f;
        std::uniform_real_distribution<> jitterDist(-std::abs(jitterOffset), std::abs(jitterOffset));
        
        for (size_t i = 0; i < cachedJitterValues.size(); ++i) {
            cachedJitterValues[i] = static_cast<float>(jitterDist(rng));
        }
        
        jitterCacheValid = true;
    }

    float UltraCanvasJitterPlotElement::GetJitterX(size_t index, float screenX) {
        if (!enableJitter || !jitterCacheValid || index >= cachedJitterValues.size()) {
            return screenX;
        }
        return screenX + cachedJitterValues[index];
    }

    void UltraCanvasJitterPlotElement::RenderChart(IRenderContext* ctx) {
        if (!ctx || !dataSource || dataSource->GetPointCount() == 0) return;

        if (!jitterCacheValid) {
            GenerateJitterCache();
        }

        ctx->SetFillPaint(pointColor);
        ctx->SetStrokePaint(pointColor);
        ctx->SetStrokeWidth(1.5f);

        for (size_t i = 0; i < dataSource->GetPointCount(); ++i) {
            auto point = dataSource->GetPoint(i);

            Point2Df screenPos = GetDataPointScreenPosition(i, point);
            screenPos.x = GetJitterX(i, screenPos.x);

            if (screenPos.x < cachedPlotArea.x + pointSize || 
                screenPos.x > cachedPlotArea.x + cachedPlotArea.width - pointSize) {
                continue;
            }

            if (screenPos.y < cachedPlotArea.y + pointSize || 
                screenPos.y > cachedPlotArea.y + cachedPlotArea.height - pointSize) {
                continue;
            }

            switch (pointShape) {
                case PointShape::Circle:
                    ctx->FillCircle(screenPos.x, screenPos.y, pointSize);
                    break;

                case PointShape::Square: {
                    float halfSize = pointSize;
                    ctx->FillRectangle(screenPos.x - halfSize, screenPos.y - halfSize,
                                       halfSize * 2, halfSize * 2);
                    break;
                }

                case PointShape::Triangle: {
                    std::vector<Point2Df> triangle = {
                            Point2Df(screenPos.x, screenPos.y - pointSize),
                            Point2Df(screenPos.x - pointSize, screenPos.y + pointSize),
                            Point2Df(screenPos.x + pointSize, screenPos.y + pointSize)
                    };
                    ctx->FillLinePath(triangle);
                    break;
                }

                case PointShape::Diamond: {
                    std::vector<Point2Df> diamond = {
                            Point2Df(screenPos.x, screenPos.y - pointSize),
                            Point2Df(screenPos.x + pointSize, screenPos.y),
                            Point2Df(screenPos.x, screenPos.y + pointSize),
                            Point2Df(screenPos.x - pointSize, screenPos.y)
                    };
                    ctx->FillLinePath(diamond);
                    break;
                }
            }
        }
    }

    bool UltraCanvasJitterPlotElement::HandleChartMouseMove(const Point2Di& mousePos) {
        if (!dataSource || !enableTooltips) return false;

        if (!jitterCacheValid) {
            GenerateJitterCache();
        }

        float minDistance = pointSize + 5.0f;
        size_t nearestIndex = SIZE_MAX;

        for (size_t i = 0; i < dataSource->GetPointCount(); ++i) {
            auto point = dataSource->GetPoint(i);
            Point2Df screenPos = GetDataPointScreenPosition(i, point);
            screenPos.x = GetJitterX(i, screenPos.x);

            if (screenPos.x < cachedPlotArea.x + pointSize || 
                screenPos.x > cachedPlotArea.x + cachedPlotArea.width - pointSize) {
                continue;
            }

            if (screenPos.y < cachedPlotArea.y + pointSize || 
                screenPos.y > cachedPlotArea.y + cachedPlotArea.height - pointSize) {
                continue;
            }

            float dx = mousePos.x - screenPos.x;
            float dy = mousePos.y - screenPos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        if (nearestIndex != SIZE_MAX) {
            auto point = dataSource->GetPoint(nearestIndex);
            ShowChartPointTooltip(mousePos, point, nearestIndex);
            return true;
        } else if (isTooltipActive) {
            HideTooltip();
        }

        return false;
    }

} // namespace UltraCanvas

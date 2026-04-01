// include/Plugins/Charts/UltraCanvasJitterPlotElement.h
// Jitter Plot Element - Scatter plot with random horizontal jitter to show overlapping points
// Version: 1.0.1
// Last Modified: 2026-03-31
// Author: UltraCanvas Framework
#pragma once

#include "UltraCanvasChartElementBase.h"
#include <cmath>
#include <random>
#include <vector>

namespace UltraCanvas {

// =============================================================================
// JITTER PLOT ELEMENT
// =============================================================================

    class UltraCanvasJitterPlotElement : public UltraCanvasChartElementBase {
    private:
        Color pointColor = Color(255, 99, 132, 255);
        float pointSize = 8.0f;
        float jitterAmount = 0.2f;  // 20% of available width
        bool enableJitter = true;
        std::mt19937 rng;
        std::vector<float> cachedJitterValues;
        bool jitterCacheValid = false;

    public:
        enum class PointShape {
            Circle, Square, Triangle, Diamond
        } pointShape = PointShape::Circle;

        UltraCanvasJitterPlotElement(const std::string &id, long uid, int x, int y, int width, int height)
                : UltraCanvasChartElementBase(id, uid, x, y, width, height) {
            enableZoom = true;
            enablePan = true;
            enableSelection = true;
            std::random_device rd;
            rng = std::mt19937(rd());
        }

        void SetDataSource(std::shared_ptr<IChartDataSource> source) {
            UltraCanvasChartElementBase::SetDataSource(source);
            jitterCacheValid = false;
            cachedJitterValues.clear();
        }

        void SetPointColor(const Color &color) {
            pointColor = color;
            RequestRedraw();
        }

        void SetPointSize(float size) {
            pointSize = size;
            RequestRedraw();
        }

        void SetPointShape(PointShape shape) {
            pointShape = shape;
            RequestRedraw();
        }

        void SetJitterAmount(float amount) {
            jitterAmount = std::clamp(amount, 0.0f, 1.0f);
            jitterCacheValid = false;
            RequestRedraw();
        }

        void SetEnableJitter(bool enabled) {
            enableJitter = enabled;
            RequestRedraw();
        }

        void RenderChart(IRenderContext *ctx) override;

        bool HandleChartMouseMove(const Point2Di &mousePos) override;

    private:
        float GetJitterX(size_t index, float screenX);
        void GenerateJitterCache();
    };

// Factory
    inline std::shared_ptr<UltraCanvasJitterPlotElement> CreateJitterPlotElement(
            const std::string &id, long uid, int x, int y, int width, int height) {
        return std::make_shared<UltraCanvasJitterPlotElement>(id, uid, x, y, width, height);
    }

} // namespace UltraCanvas

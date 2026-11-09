//SFML 3 1/27/2026

#pragma once
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>
#include <cmath>
#include <cstdint>
#include <array>


void drawLine(
    sf::RenderWindow& window,
    sf::Vector2f a,
    sf::Vector2f b,
    float thickness = 1.0f,
    sf::Color color = sf::Color::White
) 
{
    sf::Vector2f direction = b - a;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);
    line.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
    line.setPosition(a);

    float angle = std::atan2(direction.y, direction.x);
    line.setRotation(sf::radians(angle));

    window.draw(line);
}

float distanceToLineSegment(sf::Vector2f point, sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f ab = b - a;
    sf::Vector2f ap = point - a;
    
    float abLengthSq = ab.x * ab.x + ab.y * ab.y;
    
    // Handle degenerate case where a == b
    if (abLengthSq == 0.f) {
        sf::Vector2f diff = point - a;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    }
    
    // Project point onto the line, clamped to [0, 1] to stay on the segment
    float t = std::max(0.f, std::min(1.f, (ap.x * ab.x + ap.y * ab.y) / abLengthSq));
    
    // Find the closest point on the segment
    sf::Vector2f closest = a + t * ab;
    
    // Return distance from point to closest point
    sf::Vector2f diff = point - closest;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v)
{
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}


float magnitude(const sf::Vector2f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}


sf::Vector2f perpendicular(sf::Vector2f a) {
    return sf::Vector2f(a.y, -a.x);
}


bool lineLineIntersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d) {
    auto crossProduct = [](sf::Vector2f v1, sf::Vector2f v2) {
        return v1.x * v2.y - v1.y * v2.x;
    };

    sf::Vector2f r = b - a;
    sf::Vector2f s = d - c;
    float rxs = crossProduct(r, s);
    sf::Vector2f cma = c - a;
    float t = crossProduct(cma, s) / rxs;
    float u = crossProduct(cma, r) / rxs;

    // If rxs is 0, lines are parallel. 
    // t and u must be between 0 and 1 for the segments to intersect.
    return (rxs != 0 && t >= 0 && t <= 1 && u >= 0 && u <= 1);
}


bool lineRectIntersect(sf::Vector2f p1, sf::Vector2f p2, sf::FloatRect rect) {
    if (rect.contains(p1) || rect.contains(p2)) return true;

    
    sf::Vector2f topLeft(rect.position.x, rect.position.y);
    sf::Vector2f topRight(rect.position.x + rect.size.x, rect.position.y);
    sf::Vector2f bottomLeft(rect.position.x, rect.position.y + rect.size.y);
    sf::Vector2f bottomRight(rect.position.x + rect.size.x, rect.position.y + rect.size.y);

    if (lineLineIntersect(p1, p2, topLeft, topRight)) return true;    // Top
    if (lineLineIntersect(p1, p2, topRight, bottomRight)) return true; // Right
    if (lineLineIntersect(p1, p2, bottomRight, bottomLeft)) return true; // Bottom
    if (lineLineIntersect(p1, p2, bottomLeft, topLeft)) return true;    // Left

    return false;
}


std::random_device rd;
std::mt19937 gen(rd());
static sf::Vector2f rand_2d_pos(sf::RenderWindow& window, float padding = 0) {
    unsigned int width = window.getSize().x;
    unsigned int height = window.getSize().y;

    std::uniform_real_distribution<float> x_dist(width*padding, width*(1-padding));
    std::uniform_real_distribution<float> y_dist(height*padding, height*(1-padding));

    return sf::Vector2f(x_dist(gen), y_dist(gen));
}


sf::FloatRect getSelectionRect(sf::Vector2f a, sf::Vector2f b) {
    return {{std::min(a.x, b.x), std::min(a.y, b.y)},
    {std::abs(a.x - b.x), std::abs(a.y - b.y)}};
};





struct MonitorInfo {
    sf::Vector2i position;
    sf::VideoMode mode;
};

std::vector<MonitorInfo> getMonitorInfos() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMon, HDC, LPRECT, LPARAM data) -> BOOL {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        GetMonitorInfo(hMon, &info);
        
        // Calculate exact width and height of this specific monitor
        int width = info.rcMonitor.right - info.rcMonitor.left;
        int height = info.rcMonitor.bottom - info.rcMonitor.top;
        
        auto* vec = reinterpret_cast<std::vector<MonitorInfo>*>(data);
        vec->push_back({
            sf::Vector2i(info.rcMonitor.left, info.rcMonitor.top),
            sf::VideoMode({(unsigned)width, (unsigned)height})
        });

        return TRUE;
    }, reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

sf::RenderWindow createFullScreenWindow(std::string title) {
    
    auto monitors = getMonitorInfos();

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), title, sf::State::Fullscreen);
    if (monitors.size() > 1) {
        MonitorInfo secondMonitor = monitors[1];
        window.close();
        window = sf::RenderWindow(secondMonitor.mode, title, sf::Style::None);

        window.setPosition(secondMonitor.position);
     
        
    }
    
    // Fallback to primary monitor
    return window;
}


std::array<uint8_t, 3> hsv_to_rgb(double h, double s, double v) {
    h = h - std::floor(h);          // wrap hue into [0, 1)
    double h6 = h * 6.0;
    int    i  = static_cast<int>(h6);   // sector 0..5
    double f  = h6 - i;                 // position within sector

    double p = v * (1.0 - s);
    double q = v * (1.0 - s * f);
    double t = v * (1.0 - s * (1.0 - f));

    double r, g, b;
    switch (i % 6) {
        case 0:  r = v; g = t; b = p; break;
        case 1:  r = q; g = v; b = p; break;
        case 2:  r = p; g = v; b = t; break;
        case 3:  r = p; g = q; b = v; break;
        case 4:  r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }

    auto to8 = [](double x) { return static_cast<uint8_t>(std::lround(x * 255.0)); };
    return { to8(r), to8(g), to8(b) };
}


std::array<uint8_t, 3> color(uint64_t n) {
    constexpr double PHI = 0.6180339887498949;
    double h = std::fmod(static_cast<double>(n) * PHI, 1.0);
    return hsv_to_rgb(h, 0.65, 0.95);
}

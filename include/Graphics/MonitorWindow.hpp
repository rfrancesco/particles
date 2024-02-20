#ifndef GRAPHICS_MONITORWINDOW_HPP
#define GRAPHICS_MONITORWINDOW_HPP

#include <SDL2/SDL.h>
#include <vector>

constexpr int defaultCapacity{1000};

class PlotStream
{
    std::vector<double> data;
    int maxCapacity;
    int currentIndex{-1};

public:
    PlotStream() : maxCapacity{defaultCapacity} {};
    explicit PlotStream(int capacity);
    PlotStream &operator<<(double dataPoint);
    double mean() const;
    double min() const;
    double max() const;
    int size() const;
    inline auto gdata() const { return data.data(); };
    inline double operator[](std::vector<double>::size_type i) const { return data[i]; };
    inline void clear() { data.clear(); };
    inline auto begin() { return data.begin(); };
    inline auto end() { return data.end(); };
    inline double last() const
    {
        return data[currentIndex];
    }
    inline double *plast()
    {
        return data.data() + currentIndex;
    }
};

enum class MonitorWindowStatus
{
    Thermalization,
    Plot
};

class MonitorWindow
{
public:
    MonitorWindow();
    ~MonitorWindow();

    MonitorWindowStatus status{MonitorWindowStatus::Thermalization};

    PlotStream pressure;
    PlotStream area;
    PlotStream PA;

    void renderImGuiWindow_thermalization(unsigned int count, unsigned int tot);
    void renderImGuiWindow_plots();

    // private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif

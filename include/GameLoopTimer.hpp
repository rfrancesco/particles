#ifndef FRAMETIMER_HPP
#define FRAMETIMER_HPP

/* GameLoopTimer class
 * Times frames for the rendering and physics engine
 *
 * This should be better thought of, does not satisfy SRP
 * Handles both framerate, physics, profiling...
 * This should probably be split into a general-purpose timer
 * and something else */

class GameLoopTimer
{
public:
    GameLoopTimer() : t_start(0), t_end(0), t_physics_last(0), t_physics_now(0){};
    void start();
    void end();
    double elapsed_time();
    void capFPS(Uint64 max_fps);
    void logFPS() const;
    double get_physics_dt();

private:
    Uint64 t_start, t_end;
    Uint64 t_physics_last, t_physics_now;
    double current_fps;
};

#endif
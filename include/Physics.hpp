#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "GameObject.hpp"
#include <vector>

/* PhysicsEngine class
 *
 *  */

struct World
{
    int width;
    int height;
    int area;
};

class PhysicsEngine
{
public:
    void computeForces(std::vector<GameObject> &objects);
    void evolveObjects(std::vector<GameObject> &objects, double dt);
    void resolveWallCollisions(std::vector<GameObject> &objects);
    void resolveTwoBodyCollisions_spatialHashing(std::vector<GameObject> &objects);
    void resolveTwoBodyCollisions_naive(std::vector<GameObject> &objects);
    void setWorldBox(int width, int height);
    double getPressure() const;
    double getArea() const;
    double getVanDerWaalsArea(const std::vector<GameObject> &objects) const;
    double getTotalKineticEnergy(const std::vector<GameObject> &objects) const;

private:
    void evolve_object(GameObject &object, double dt);
    World world;
    double impulseOnBox;
    double pressure;
};

#endif
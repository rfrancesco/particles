#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "GameObject.hpp"
#include <vector>

/* PhysicsEngine class
 * 
 *  */

class PhysicsEngine
{
public:
    void computeForces(std::vector<GameObject> &objects);
    void evolveObjects(std::vector<GameObject>& objects, double dt, int width, int height);
    void resolveWallCollisions(std::vector<GameObject>& objects, int width, int height);
    void resolveTwoBodyCollisions(std::vector<GameObject>& objects);


private:
    void evolve_object(GameObject &object, double dt);
};

#endif
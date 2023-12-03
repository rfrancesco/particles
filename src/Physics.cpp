#include "Physics.hpp"
#include <iostream>

const double ax_g = 0;
const double ay_g = 500;

const double slowdown = 1.0;

const double wall_collision_energy_loss_n = 0.99;
const double wall_collision_energy_loss_p = 0.99999;

void PhysicsEngine::computeForces(std::vector<GameObject>& objects)
{
    for (int i=0; i<objects.size(); ++i)
    {
        objects[i].force.x=ax_g;
        objects[i].force.y=ay_g;
    }
}

void PhysicsEngine::evolve_object(GameObject& obj, double dt)
{
    if (obj.locked)
    {
        return;
    }

    /* Euler's algorithm, semi-implicit (symplectic) variant */
    obj.velocity.x+=obj.force.x*dt;
    obj.velocity.y+=obj.force.y*dt;
    obj.pos.x+=obj.velocity.x*dt;
    obj.pos.y+=obj.velocity.y*dt;
    // std::cout << obj.velocity.x <<  " " << obj.velocity.y << " " << obj.kineticEnergy() << std::endl;
}

void PhysicsEngine::evolveObjects(std::vector<GameObject>& objects, double dt, int width, int height)
{
    const int n_order=3;
    dt *= slowdown;
    const double eps = dt/n_order;
    for (int i=0; i<n_order; i++)
    {
        for (auto& obj : objects)
            evolve_object(obj,eps);
        resolveWallCollisions(objects, width, height);
        resolveTwoBodyCollisions(objects);
    }
}

void PhysicsEngine::resolveWallCollisions(std::vector<GameObject>& objects, int width, int height)
{
        for (auto& object : objects)
    {

        if ((object.pos.x-object.r) <= 0)
        {
            object.pos.x = object.r;
            object.velocity.x=-wall_collision_energy_loss_n*object.velocity.x;
            object.velocity.y=wall_collision_energy_loss_p*object.velocity.y;
        }
        if ((object.pos.y-object.r) <= 0)
        {
            object.pos.y = object.r;
            object.velocity.x=wall_collision_energy_loss_p*object.velocity.x;
            object.velocity.y=-wall_collision_energy_loss_n*object.velocity.y;
        }
        if ((object.pos.x + object.r) >= width)
        {
            object.pos.x = width - object.r;
            object.velocity.x = -wall_collision_energy_loss_n*object.velocity.x;
            object.velocity.y=wall_collision_energy_loss_p*object.velocity.y;
        }
        if ((object.pos.y + object.r) >= height)
        {
            object.pos.y = height - object.r;
            object.velocity.x=wall_collision_energy_loss_p*object.velocity.x;
            object.velocity.y = -wall_collision_energy_loss_n*object.velocity.y;
        }
    }
}
void PhysicsEngine::resolveTwoBodyCollisions(std::vector<GameObject>& objects)
{

    for (int i=0; i<objects.size(); i++)
    {
        for (int j=i+1; j<objects.size(); j++)
        {
            double dx = objects[i].pos.x - objects[j].pos.x;
            double dy = objects[i].pos.y - objects[j].pos.y;
            Vector2 dp = {dx, dy};

            double min_distance = objects[i].r + objects[j].r;
            double real_distance = dp.norm();

            double overlap_x = dp.x/real_distance;
            double overlap_y = dp.y/real_distance;

            if (real_distance < min_distance)
            {
                Vector2 final_velocity_i, final_velocity_j;
                final_velocity_i = objects[j].velocity;
                final_velocity_j = objects[i].velocity;
                objects[i].velocity = final_velocity_i;
                objects[j].velocity = final_velocity_j;

                objects[i].pos.x+=overlap_x/2;
                objects[j].pos.x-=overlap_x/2;
                objects[i].pos.y+=overlap_y/2;
                objects[j].pos.y-=overlap_y/2;
            }  
        }
    }
}
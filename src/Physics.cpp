#include "Physics.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#define M_PI 3.14159265358979323846

const double ax_g = 0;
const double ay_g = 500;

const double slowdown = 1.0;

const double wall_collision_energy_loss_n = 1.0; // 0.99;
const double wall_collision_energy_loss_p = 1.0; // 0.99999;

void PhysicsEngine::setWorldBox(int width, int height)
{
    world.width = width;
    world.height = height;
    world.area = width * height;
}

double PhysicsEngine::getPressure() const
{
    return pressure;
}

double PhysicsEngine::getArea() const
{
    return world.area;
}

double PhysicsEngine::getVanDerWaalsArea(const std::vector<GameObject> &objects) const
{
    /* Double check that this makes sense...
       At low densities the factor is irrelevant anyway, but at large densities? */
    double a = world.area;
    for (const auto &o : objects)
        a -= 2 * M_PI * o.r * o.r;
    return a;
}

double PhysicsEngine::getTotalKineticEnergy(const std::vector<GameObject> &objects) const
{
    double K = 0.0;
    for (const auto &o : objects)
        K += 0.5 * o.velocity.normSquared();
    return K;
}

void PhysicsEngine::computeForces(std::vector<GameObject> &objects)
{
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i].force.x = ax_g;
        objects[i].force.y = ay_g;
    }
}

void PhysicsEngine::evolve_object(GameObject &obj, double dt)
{
    if (obj.locked)
    {
        return;
    }

    /* Euler's algorithm, semi-implicit (symplectic) variant */
    obj.velocity.x += obj.force.x * dt;
    obj.velocity.y += obj.force.y * dt;
    obj.pos.x += obj.velocity.x * dt;
    obj.pos.y += obj.velocity.y * dt;
    // std::cout << obj.velocity.x <<  " " << obj.velocity.y << " " << obj.kineticEnergy() << std::endl;
}

void PhysicsEngine::evolveObjects(std::vector<GameObject> &objects, double dt)
{
    const int n_order = 3;
    dt *= slowdown;
    const double eps = dt / n_order;

    impulseOnBox = 0.0;
    for (int i = 0; i < n_order; i++)
    {
        for (auto &obj : objects)
            evolve_object(obj, eps);
        resolveWallCollisions(objects);
        resolveTwoBodyCollisions_naive(objects);
    }
    pressure = 2 * (impulseOnBox / dt) / (world.width + world.height);
}

void PhysicsEngine::resolveWallCollisions(std::vector<GameObject> &objects)
{
    for (auto &object : objects)
    {

        /* TODO: replace fabs with appropriate signs */
        if ((object.pos.x - object.r) <= 0)
        {
            object.pos.x = object.r;
            impulseOnBox += (1.0 + wall_collision_energy_loss_n) * fabs(object.velocity.x);
            object.velocity.x = -wall_collision_energy_loss_n * object.velocity.x;
            object.velocity.y = wall_collision_energy_loss_p * object.velocity.y;
        }
        if ((object.pos.y - object.r) <= 0)
        {
            object.pos.y = object.r;
            impulseOnBox += (1.0 + wall_collision_energy_loss_n) * fabs(object.velocity.y);
            object.velocity.x = wall_collision_energy_loss_p * object.velocity.x;
            object.velocity.y = -wall_collision_energy_loss_n * object.velocity.y;
        }
        if ((object.pos.x + object.r) >= world.width)
        {
            object.pos.x = world.width - object.r;
            impulseOnBox += (1.0 + wall_collision_energy_loss_n) * fabs(object.velocity.y);
            object.velocity.x = -wall_collision_energy_loss_n * object.velocity.x;
            object.velocity.y = wall_collision_energy_loss_p * object.velocity.y;
        }
        if ((object.pos.y + object.r) >= world.height)
        {
            object.pos.y = world.height - object.r;
            impulseOnBox += (1.0 + wall_collision_energy_loss_n) * fabs(object.velocity.x);
            object.velocity.x = wall_collision_energy_loss_p * object.velocity.x;
            object.velocity.y = -wall_collision_energy_loss_n * object.velocity.y;
        }
    }
}

/* Cleanup static variables and utility functions!!! */
static int safemod(int x, int m)
{
    while (x < 0)
        x += m;
    while (x >= m)
        x -= m;
    return x;
}

const int n_cells_x = 10;
const int n_cells_y = 10;
static int size = 0;
static int hash(int x, int y, int width, int height)
{
    int cellsize_x = width / n_cells_x;
    int cellsize_y = height / n_cells_y;
    return n_cells_y * (x / cellsize_x) + (y / cellsize_y);
}

bool cmp(const GameObject &a, const GameObject &b)
{
    return (a.r > b.r);
}

/* TODO: Stop passing width and height around!
         Set up a proper PhysicsEngine::World
         with height, width and its own Particle registry */
void PhysicsEngine::resolveTwoBodyCollisions_spatialHashing(std::vector<GameObject> &objects)
{
    /* Prepare hashgrid */
    std::map<int, std::vector<GameObject *>> grid; /* Maybe multimap makes more sense */
    for (int i = 0; i < objects.size(); i++)
    {
        Vector2 &position = objects[i].pos;
        grid[hash(position.x, position.y, world.width, world.height)].push_back(&objects[i]);
    }

    if (size == 0)
        size = (*std::max_element(objects.begin(), objects.end(), cmp)).r;

    int look_ahead_x = std::max(size / (world.width / n_cells_x), 1);
    int look_ahead_y = std::max(size / (world.height / n_cells_y), 1);

    for (int r = 0; r < n_cells_x; r++)
        for (int c = 0; c < n_cells_y; c++)
        {
            std::vector<GameObject *> check;
            for (int dr = -look_ahead_x; dr <= look_ahead_x; dr++)
                for (int dc = -look_ahead_y; dc <= look_ahead_y; dc++)
                {
                    if (((r + dr < 0) || (r + dr >= n_cells_x)) || ((c + dc < 0) || (c + dc >= n_cells_y)))
                        continue;
                    std::vector<GameObject *> &cell = grid[n_cells_y * safemod(r + dr, n_cells_x) + safemod(c + dc, n_cells_y)];
                    check.insert(check.end(), cell.begin(), cell.end());
                }

            for (int i = 0; i < check.size(); i++)
            {
                for (int j = i + 1; j < check.size(); j++)
                {
                    double dx = check[i]->pos.x - check[j]->pos.x;
                    double dy = check[i]->pos.y - check[j]->pos.y;
                    Vector2 dp = {dx, dy};

                    double min_distance = check[i]->r + check[j]->r;
                    double real_distance = dp.norm();

                    /* Division by zero makes particles disappear into NaN */
                    if (real_distance == 0)
                        real_distance = 1;

                    double overlap_x = dp.x / real_distance;
                    double overlap_y = dp.y / real_distance;

                    if (real_distance < min_distance)
                    {
                        Vector2 final_velocity_i, final_velocity_j;
                        final_velocity_i = check[j]->velocity;
                        final_velocity_j = check[i]->velocity;
                        check[i]->velocity = final_velocity_i;
                        check[j]->velocity = final_velocity_j;

                        check[i]->pos.x += overlap_x / 2;
                        check[j]->pos.x -= overlap_x / 2;
                        check[i]->pos.y += overlap_y / 2;
                        check[j]->pos.y -= overlap_y / 2;
                    }
                }
            }
        }
}

void PhysicsEngine::resolveTwoBodyCollisions_naive(std::vector<GameObject> &objects)
{
    /* Naive O(n^2) implementation */

    for (int i = 0; i < objects.size(); i++)
    {
        for (int j = i + 1; j < objects.size(); j++)
        {
            double dx = objects[i].pos.x - objects[j].pos.x;
            double dy = objects[i].pos.y - objects[j].pos.y;
            Vector2 dp = {dx, dy};

            double min_distance = objects[i].r + objects[j].r;
            double real_distance = dp.norm();

            /* Division by zero makes particles disappear into NaN */
            if (real_distance == 0)
                real_distance = 0.1;

            double overlap_x = dp.x / real_distance;
            double overlap_y = dp.y / real_distance;

            if (real_distance < min_distance)
            {
                Vector2 final_velocity_i, final_velocity_j;
                final_velocity_i = objects[j].velocity;
                final_velocity_j = objects[i].velocity;
                objects[i].velocity = final_velocity_i;
                objects[j].velocity = final_velocity_j;

                objects[i].pos.x += overlap_x / 2;
                objects[j].pos.x -= overlap_x / 2;
                objects[i].pos.y += overlap_y / 2;
                objects[j].pos.y -= overlap_y / 2;
            }
        }
    }
}

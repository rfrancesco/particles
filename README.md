# particles
A simple 2D point particle simulator, written for fun in C++ / SDL.

All particles live inside the window borders, colliding both with the walls and with each other. The window can be resized!

## Build
```
mkdir build
cd build
cmake ..
make

./Particles
```

If you see an error message when running `./Particles` under Wayland, try
```
SDL_VIDEODRIVER=x11 ./Particles
```
to force SDL to use XWayland.

## Todo
This is just a very simple first step towards a classical mechanics engine. Some possible things that should be done:

### Project design
- [ ] Decoupling the Physics engine from the Viewer, so that they are fully independent
- [ ] Abstracting away all SDL calls into both Graphics and Input (no checking Scancodes in main!)
### Physics
- [ ] Faster collisions using Quadtrees
- [ ] Adding masses and generic forces (e.g. gravity and springs). For now, the only force is a global uniform gravity field.
- [ ] Parallelizing some parts with OpenMP

### Maybe for a different project
- [ ] Rigid bodies: this requires a more sophisticated collision engine. 

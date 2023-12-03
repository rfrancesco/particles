# particles
A simple 2D point particle simulator, written for fun in C++ / SDL.

All particles live inside the window borders, colliding both with the walls and with each other. The window can be resized!

## Todo
This is just a very simple first step towards a classical mechanics engine. Some possible things that should be done:

### Project design
- [ ] Decoupling the Physics engine from the Viewer, so that they are fully independent
- [ ] Abstracting away all SDL calls into both Graphics and Input (no checking Scancodes in main!)
### Physics
- [ ] Faster collisions using Quadtrees
- [ ] Adding masses and generic forces (e.g. gravity and springs). For now, the only force is a global uniform gravity field.

### Maybe for a different project
- [ ] Rigid bodies: this requires a more sophisticated collision engine. 

#ifndef SDLMANAGER_HPP
#define SDLMANAGER_HPP

/* Note: this is not thread-safe */
class SDLManager
{
protected:
    SDLManager();
    ~SDLManager();
    static SDLManager* singleton;

public:
    SDLManager(SDLManager& other) = delete;
    static void initSDLIfNotYetInitialized();
    static SDLManager* getInstance();
    void operator=(const SDLManager&) = delete;

};

#endif
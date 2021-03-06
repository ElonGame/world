//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <mutex>
#include <memory>
#include <list>

#include <world/core.h>
#include <world/flat.h>

#include "MainView.h"

class Application {
public:
    Application();

    void run(int argc, char **argv);
    void requestStop();

    void setUserPosition(world::vec3d pos);
    world::vec3d getUserPosition() const;

    void refill(std::unique_ptr<world::Collector> &&toRefill);
    std::unique_ptr<world::Collector> popFull();

private:
    std::atomic_bool _running;
    mutable std::mutex _paramLock;

    std::unique_ptr<world::FlatWorld> _world;
    std::unique_ptr<world::FirstPersonView> _resModel;

    std::list<std::unique_ptr<world::Collector>> _emptyCollectors;
    std::list<std::unique_ptr<world::Collector>> _fullCollectors;

    world::vec3d _newUpdatePos;
    world::vec3d _lastUpdatePos;

    std::unique_ptr<MainView> _mainView;

    bool _dbgOn = true;

    void loadWorld(int argc, char **argv);
};


#endif // WORLD_APPLICATION_H

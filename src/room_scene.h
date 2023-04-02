#ifndef ROOM_SCENE_H
#define ROOM_SCENE_H

#include "cudastuff.h"
#include "raytracer_basics.h"

namespace raytracer_cu {

  class RoomScene : public Scene {
  public:
    CUDA_HOSTDEV RoomScene(){};
    CUDA_HOSTDEV void buildScene();
  };

}


#endif

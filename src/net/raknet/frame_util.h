/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifndef FRAME_UTIL_H

#define FRAME_UTIL_H

#include "../../net/raknet/packets.h"

unsigned int get_frame_size(frame_t frame);
unsigned int get_frame_set_size(frame_set_t frame_set);

#endif
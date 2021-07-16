/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include "../../net/raknet/packets.h"
#include "../../net/raknet/reliability_util.h"

unsigned int get_frame_size(frame_t frame)
{
    unsigned int size = 3;
    if (is_reliable(frame.reliability))
    {
        size += 3;
    }
    if (is_sequenced(frame.reliability))
    {
        size += 3;
    }
    if (is_ordered(frame.reliability))
    {
        size += 4;
    }
    if (frame.is_fragmented == 1)
    {
        size += 10;
    }
    size += frame.body_length;
    return size;
}

unsigned int get_frame_set_size(frame_set_t frame_set)
{
    unsigned int size = 4;
    int i;
    for (i = 0; i < frame_set.frame_count; ++i)
    {
        size += get_frame_size(frame_set.frames[i]);
    }
    return size;
}
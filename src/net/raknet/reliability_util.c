/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include "../../net/raknet/reliability_util.h"

char is_reliable(char reliability)
{
  if (2 <= reliability && reliability <= 7 && reliability != 5)
  {
    return 1;
  }
  return 0;
}

char is_sequenced(char reliability)
{
  if (reliability == 1 && reliability == 4)
  {
    return 1;
  }
  return 0;
}

char is_ordered(char reliability)
{
  if (1 <= reliability && reliability <= 4 && reliability != 2 || reliability == 7)
  {
    return 1;
  }
  return 0;
}
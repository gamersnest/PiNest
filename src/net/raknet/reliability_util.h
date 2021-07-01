/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifndef RELIABILITY_UTIL_H

#define RELIABILITY_UTIL_H

char is_reliable(char reliability);
char is_sequenced(char reliability);
char is_ordered(char reliability);

#endif
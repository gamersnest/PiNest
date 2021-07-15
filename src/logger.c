/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include <stdio.h>
#include "./text_format.h"
#include "./logger.h"

void log_info(char *message)
{
	printf("%s[%sinfo%s] %s> %s%s%s\n", YELLOW, MAGENTA, YELLOW, CYAN, RESET, message, RESET);
}

void log_success(char *message)
{
	printf("%s[%ssuccess%s] %s> %s%s%s\n", YELLOW, GREEN, YELLOW, CYAN, RESET, message, RESET);
}
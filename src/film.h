#ifndef JARTSY_FILM_H
#define JARTSY_FILM_H

#include "jartsyla.h"

class Film {
public:
    Point2i resolution;     // x*y e.g. 1024x768
    Float diagonal;         // In meters in world space
};


#endif
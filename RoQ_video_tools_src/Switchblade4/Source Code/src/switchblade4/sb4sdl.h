#ifndef __SB4SDL_H__
#define __SB4SDL_H__

#include "SDL/SDL.h"
#include "../tdp/tdp_vector.hpp"

using namespace TDP;

SDL_Surface *SurfaceFromImage(const class SB4Image *, terVector<unsigned char> &scratch);
int SB4FlushSDLEvents();

#endif

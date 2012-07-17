#ifndef _SDL_BILINEAR_H_
#define _SDL_BILINEAR_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PI 3.14159265f
#define ABS(X) ((X < 0)? -X : X)

typedef struct tRGBA {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} RGBA;

SDL_Surface *SDL_Rotate(SDL_Surface *Surface, double angle);
SDL_Surface *SDL_RotateSmooth(SDL_Surface *Surface, double angle);
SDL_Surface *SDL_Resize(SDL_Surface *Surface, int newW, int newH);
SDL_Surface *SDL_ResizeSmooth(SDL_Surface *Surface, int newW, int newH);

#ifdef __cplusplus
}
#endif

#endif

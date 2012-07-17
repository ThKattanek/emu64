#include "SDL_Bilinear.h"

SDL_Surface *SDL_Rotate(SDL_Surface *Surface, double angle) {
    double dcos = cos(angle);
    double dsin = sin(angle);

    int iWidth  = (int) ceil(ABS(Surface->h * dsin) + ABS(Surface->w * dcos));
    int iHeight = (int) ceil(ABS(Surface->h * dcos) + ABS(Surface->w * dsin));

    int centre_x = Surface->w << 15;
    int centre_y = Surface->h << 15;
    int dx = iWidth  >> 1;
    int dy = iHeight >> 1;

    int rotx, roty, newx, newy, x, y;
    int ncos  = (int) (dcos * 65536.0f);
    int nsin  = (int) (dsin * 65536.0f);
    int srotx = (int) (-dx * ncos + dy * nsin) + centre_x;
    int sroty = (int) (-dx * nsin - dy * ncos) + centre_y;

    Uint8 *sp, *dp, bpp;
    Uint16 destinationGap;
    SDL_Surface *destination;

    if((bpp = Surface->format->BytesPerPixel) == 3) {
        return NULL;
    }

    if((destination = SDL_CreateRGBSurface(Surface->flags, iWidth, iHeight, 
                                           Surface->format->BitsPerPixel,
                                           Surface->format->Rmask,
                                           Surface->format->Gmask,
                                           Surface->format->Bmask,
                                           Surface->format->Amask)) == NULL) {
        return NULL;
    }

    if(SDL_MUSTLOCK(Surface)) {
        if(SDL_LockSurface(Surface) < 0) {
            return NULL;
        }
    }
    if(SDL_MUSTLOCK(destination)) {
        if(SDL_LockSurface(destination) < 0) {
            return NULL;
        }
    }
    
    dp = (Uint8 *) destination->pixels;
    destinationGap = destination->pitch - destination->w * bpp;

    for(y = 0; y < iHeight; y++) {
        rotx = srotx = srotx - nsin;
        roty = sroty = sroty + ncos;
        for(x = 0; x < iWidth; x++) {
            newx = rotx >> 16;
            newy = roty >> 16;
            if(newx >= 0 && newx < Surface->w && newy >= 0 && newy < Surface->h) {
                sp = (Uint8 *) Surface->pixels 
                    + (((int) newy) * Surface->pitch)
                    + (((int) newx) * bpp);

                switch(bpp) {
                    case 1:
                        *dp = *sp;
                        break;
                    case 2:
                        *(Uint16 *) dp = *(Uint16 *) sp;
                        break;
                    case 4:
                        *(Uint32 *) dp = *(Uint32 *) sp;
                        break;
                    default:
                        break;
                }
            }            
            rotx += ncos;
            roty += nsin;
            dp += bpp;
        }
        dp += destinationGap;
    }

    if(SDL_MUSTLOCK(Surface)) {
        SDL_UnlockSurface(Surface);
    }
    if(SDL_MUSTLOCK(destination)) {
        SDL_UnlockSurface(destination);
    }

    return destination;
}

SDL_Surface *SDL_RotateSmooth(SDL_Surface *Surface, double angle) {
    double dcos = cos(angle);
    double dsin = sin(angle);

    int iWidth  = (int) ceil(ABS(Surface->h * dsin) + ABS(Surface->w * dcos));
    int iHeight = (int) ceil(ABS(Surface->h * dcos) + ABS(Surface->w * dsin));

    int centre_x = Surface->w << 15;
    int centre_y = Surface->h << 15;
    int dx = iWidth  >> 1;
    int dy = iHeight >> 1;

    int rotx, roty, newx, newy, x, y;
    int ncos  = (int) (dcos * 65536.0f);
    int nsin  = (int) (dsin * 65536.0f);
    int srotx = (int) (-dx * ncos + dy * nsin) + centre_x;
    int sroty = (int) (-dx * nsin - dy * ncos) + centre_y;

    RGBA *dp, *sp;
    RGBA *cO, *cH, *cV, *cD;
    Uint8 bpp;
    Uint16 destinationGap;
    Uint32 m1, m2;
    Uint32 wx, rwx, wy, rwy;
    SDL_Surface *destination;

    if((bpp = Surface->format->BytesPerPixel) == 3) {
        return NULL;
    }
    if(bpp != 4) {
        return SDL_Rotate(Surface, angle);
    }

    if((destination = SDL_CreateRGBSurface(Surface->flags, iWidth, iHeight, 
                                           Surface->format->BitsPerPixel,
                                           Surface->format->Rmask,
                                           Surface->format->Gmask,
                                           Surface->format->Bmask,
                                           Surface->format->Amask)) == NULL) {
        return NULL;
    }

    if(SDL_MUSTLOCK(Surface)) {
        if(SDL_LockSurface(Surface) < 0) {
            return NULL;
        }
    }
    if(SDL_MUSTLOCK(destination)) {
        if(SDL_LockSurface(destination) < 0) {
            return NULL;
        }
    }
    
    dp = (RGBA *) destination->pixels;
    destinationGap = destination->pitch - destination->w * bpp;

    for(y = 0; y < iHeight; y++) {
        rotx = srotx = srotx - nsin;
        roty = sroty = sroty + ncos;
        for(x = 0; x < iWidth; x++) {
            newx = rotx >> 16;
            newy = roty >> 16;
            if(newx >= 0 && newx < Surface->w && newy >= 0 && newy < Surface->h) {
                sp = (RGBA *) ((Uint8 *) Surface->pixels 
                    + (((int) newy) * Surface->pitch) 
                    + (((int) newx) * bpp));

                cO = sp;
                cV = (newy < Surface->h - 1)? (RGBA *) ((Uint8 *) sp + Surface->pitch) : sp;
                cH = (newx < Surface->w - 1)? sp + 1 : sp;
                cD = (newx < Surface->w - 1)? cV + 1 : cV;

                wx  = rotx % 65536;
                rwx = 65536 - wx;
                wy  = roty % 65536;
                rwy = 65536 - wy;

                m1 = ((cO->r * rwx + cH->r * wx) >> 16);
                m2 = ((cV->r * rwx + cD->r * wx) >> 16);
                dp->r = (m1 * rwy + m2 * wy) >> 16;

                m1 = ((cO->g * rwx + cH->g * wx) >> 16);
                m2 = ((cV->g * rwx + cD->g * wx) >> 16);
                dp->g = (m1 * rwy + m2 * wy) >> 16;

                m1 = ((cO->b * rwx + cH->b * wx) >> 16);
                m2 = ((cV->b * rwx + cD->b * wx) >> 16);
                dp->b = (m1 * rwy + m2 * wy) >> 16;

                m1 = ((cO->a * rwx + cH->a * wx) >> 16);
                m2 = ((cV->a * rwx + cD->a * wx) >> 16);
                dp->a = (m1 * rwy + m2 * wy) >> 16;
            }            
            rotx += ncos;
            roty += nsin;
            dp++;
        }
        dp = (RGBA *) ((Uint8 *) dp + destinationGap);
    }

    if(SDL_MUSTLOCK(Surface)) {
        SDL_UnlockSurface(Surface);
    }
    if(SDL_MUSTLOCK(destination)) {
        SDL_UnlockSurface(destination);
    }

    return destination;
}

SDL_Surface *SDL_Resize(SDL_Surface *Surface, int newW, int newH) {
    int x_increment, y_increment;
    int x, y, accum_col, accum_row;
    Uint32 *col_increment, *row_increment;
    Uint8 bpp = Surface->format->BytesPerPixel;
    Uint8 *sourcePtr, *destinationPtr, *currentSourcePtr;
    Uint16 destinationGap;
    SDL_Surface *destination;

    if(newW <= 0) newW = 1;
    if(newH <= 0) newH = 1;

    if(Surface->format->BytesPerPixel == 3) {
        return NULL;
    }

    x_increment   = (Surface->w << 16) / newW;
    y_increment   = (Surface->h << 16) / newH;
    col_increment = (Uint32 *) malloc(sizeof(*col_increment) * (newW + 1));
    row_increment = (Uint32 *) malloc(sizeof(*row_increment) * (newH + 1));

    if((destination = SDL_CreateRGBSurface(Surface->flags, newW, newH, 
                                           Surface->format->BitsPerPixel,
                                           Surface->format->Rmask,
                                           Surface->format->Gmask,
                                           Surface->format->Bmask,
                                           Surface->format->Amask)) == NULL) {
        return NULL;
    }

    for(x = 0, accum_col = 0; x <= newW; x++) {
        col_increment[x] = (accum_col >> 16) * bpp;
        accum_col &= 0xFFFF;
        accum_col += x_increment;
    }
    for(y = 0, accum_row = 0; y <= newH; y++) {
        row_increment[y] = (accum_row >> 16) * Surface->pitch;
        accum_row &= 0xFFFF;
        accum_row += y_increment;
    }

    if(SDL_MUSTLOCK(Surface)) {
        if(SDL_LockSurface(Surface) < 0) {
            return NULL;
        }
    }
    if(SDL_MUSTLOCK(destination)) {
        if(SDL_LockSurface(destination) < 0) {
            return NULL;
        }
    }

    sourcePtr        = (Uint8 *) Surface->pixels;
    destinationPtr   = (Uint8 *) destination->pixels;
    currentSourcePtr = sourcePtr;
    destinationGap   = destination->pitch - (destination->w * 4);

    for(y = 0; y < newH; y++) {
        for(x = 0; x < newW; x++) {
            switch(bpp) {
                case 1:
                    *destinationPtr = *sourcePtr;
                    break;
                case 2:
                    *(Uint16 *) destinationPtr = *(Uint16 *) sourcePtr;
                    break;
                case 4:
                    *(Uint32 *) destinationPtr = *(Uint32 *) sourcePtr;
                    break;
                default:
                    break;
            }
            destinationPtr += bpp;
            sourcePtr += col_increment[x + 1];
        }
        currentSourcePtr = currentSourcePtr + row_increment[y + 1];
        sourcePtr = currentSourcePtr;
        destinationPtr += destinationGap;
    }

    if(SDL_MUSTLOCK(Surface)) {
        SDL_UnlockSurface(Surface);
    }
    if(SDL_MUSTLOCK(destination)) {
        SDL_UnlockSurface(destination);
    }
    
    free(col_increment);
    free(row_increment);

    return destination;
}

SDL_Surface *SDL_ResizeSmooth(SDL_Surface *Surface, int newW, int newH) {
    Uint32 mean1, mean2;
    Uint16 destinationGap;
    SDL_Surface *destination;
    int x_increment, y_increment;
    int x, y, accum_col, accum_row;
    Uint32 wx, wy, rwx, rwy;
    Uint32 *col_increment, *row_increment;
    RGBA *sourcePtr, *destinationPtr;
    RGBA *cO, *cH, *cV, *cD;
    
    if(Surface->format->BytesPerPixel == 3) {
        return NULL;
    }
    if(Surface->format->BytesPerPixel != 4) {
        return SDL_Resize(Surface, newW, newH);
    }

    if(newW <= 0) newW = 1;
    if(newH <= 0) newH = 1;

    x_increment = (((Surface->w - 1) << 16) / newW);
    y_increment = (((Surface->h - 1) << 16) / newH);

    col_increment = (Uint32 *) malloc(sizeof(*col_increment) * (newW + 1));
    row_increment = (Uint32 *) malloc(sizeof(*row_increment) * (newH + 1));

    for(x = 0, accum_col = 0; x <= newW; x++) {
        col_increment[x] = accum_col;
        accum_col &= 0xFFFF;
        accum_col += x_increment;
    }
    for(y = 0, accum_row = 0; y <= newH; y++) {
        row_increment[y] = accum_row;
        accum_row &= 0xFFFF;
        accum_row += y_increment;
    }

    if((destination = SDL_CreateRGBSurface(Surface->flags, newW, newH, 
                                           Surface->format->BitsPerPixel,
                                           Surface->format->Rmask,
                                           Surface->format->Gmask,
                                           Surface->format->Bmask,
                                           Surface->format->Amask)) == NULL) {
        return NULL;
    }

    if(SDL_MUSTLOCK(Surface)) {
        if(SDL_LockSurface(Surface) < 0) {
            return NULL;
        }
    }
    if(SDL_MUSTLOCK(destination)) {
        if(SDL_LockSurface(destination) < 0) {
            return NULL;
        }
    }

    sourcePtr = (RGBA *) Surface->pixels;
    destinationPtr = (RGBA *) destination->pixels;
    destinationGap = destination->pitch - (destination->w * 4);

    for(y = 0; y < newH; y++) {
        cO = cH = sourcePtr;
        cH++;
        cV = cD = (RGBA *) ((Uint8 *) sourcePtr + Surface->pitch);
        cD++;
        wy = row_increment[y] % 65536;
        rwy = 65536 - wy;
        for(x = 0; x < newW; x++) {
            wx = col_increment[x] % 65536;
            rwx = 65536 - wx;

            mean1 = ((cO->r * rwx + cH->r * wx) >> 16);
            mean2 = ((cV->r * rwx + cD->r * wx) >> 16);
            destinationPtr->r = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->g * rwx + cH->g * wx) >> 16);
            mean2 = ((cV->g * rwx + cD->g * wx) >> 16);
            destinationPtr->g = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->b * rwx + cH->b * wx) >> 16);
            mean2 = ((cV->b * rwx + cD->b * wx) >> 16);
            destinationPtr->b = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->a * rwx + cH->a * wx) >> 16);
            mean2 = ((cV->a * rwx + cD->a * wx) >> 16);
            destinationPtr->a = (mean1 * rwy + mean2 * wy) >> 16;

            destinationPtr++;

            wx = col_increment[x + 1] >> 16;
            cO += wx;
            cH += wx;
            cV += wx;
            cD += wx;
        }
        sourcePtr = (RGBA *) ((Uint8 *) sourcePtr + (row_increment[y + 1] >> 16) * Surface->pitch);
        destinationPtr = (RGBA *) ((Uint8 *) destinationPtr + destinationGap);
    }

    if(SDL_MUSTLOCK(Surface)) {
        SDL_UnlockSurface(Surface);
    }
    if(SDL_MUSTLOCK(destination)) {
        SDL_UnlockSurface(destination);
    }

    free(col_increment);
    free(row_increment);

    return destination;
}

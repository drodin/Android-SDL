/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* Being a android driver, there's no event stream. We just define stubs for
   most of the API. */

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_androidvideo.h"
#include "SDL_androidevents_c.h"

static SDLKey keymap[92] = {
SDLK_UNKNOWN/*null*/, SDLK_F1, SDLK_F2, SDLK_HOME, SDLK_ESCAPE, SDLK_UNKNOWN/*MOUSE MIDDLE*/, SDLK_UNKNOWN/*MOUSE RIGHT*/,
SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
SDLK_KP_MULTIPLY, SDLK_KP_DIVIDE, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
SDLK_PAUSE, SDLK_PAGEUP, SDLK_PAGEDOWN, SDLK_END, SDLK_PRINT, SDLK_DELETE,
SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l, SDLK_m,
SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z,
SDLK_COMMA, SDLK_PERIOD, SDLK_LALT, SDLK_RALT, SDLK_LSHIFT, SDLK_RSHIFT,
SDLK_TAB, SDLK_SPACE, SDLK_CAPSLOCK, SDLK_F3, SDLK_F4, SDLK_RETURN, SDLK_BACKSPACE, SDLK_BACKQUOTE,
SDLK_MINUS, SDLK_EQUALS, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET, SDLK_BACKSLASH, SDLK_SEMICOLON,
SDLK_QUOTE, SDLK_SLASH, SDLK_INSERT, SDLK_NUMLOCK, SDLK_KP_MINUS, SDLK_F12, SDLK_PLUS, SDLK_UNKNOWN/*null*/,
SDLK_KP_PLUS, SDLK_COMPOSE/*SPECIAL*/, SDLK_F5, SDLK_F6, SDLK_F7,
SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11
};

void ANDROID_InitOSKeymap(_THIS) {
}

SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym) {
    if ( scancode >= SDL_arraysize(keymap) )
        scancode = 0;
    keysym->scancode = scancode;
    keysym->sym = keymap[scancode];
    keysym->mod = KMOD_NONE;

    /* If UNICODE is on, get the UNICODE value for the key */
    keysym->unicode = 0;
    if ( SDL_TranslateUNICODE ) {
        /* Populate the unicode field with the ASCII value */
        keysym->unicode = keymap[scancode];
    }

    return(keysym);
}

void ANDROID_PumpEvents(_THIS) {
    int keyState, touchState;
    int keyAction, keyCode, touchAction, mouseButton, X, Y;
    SDL_keysym keysym;

    keyState = (*jvmEnv)->CallStaticIntMethod(jvmEnv, nativeClass, getKey);

    if (keyState != -1) {
        keyAction = keyState/1000;
        keyCode = keyState%1000;
        //LOGI("keyEvent: %i, %i", keyAction, keyCode);

        SDL_PrivateKeyboard( (keyAction) ? SDL_RELEASED : SDL_PRESSED, TranslateKey (keyCode, &keysym));
    }

    touchState = (*jvmEnv)->CallStaticIntMethod(jvmEnv, nativeClass, getTouch);

    if (touchState != -1) {
        touchAction = touchState/1000000;
        X = (touchState%1000000)/1000;
        Y = touchState%1000;
        //LOGI("touchEvent: %i, %i, %i", touchAction, X, Y);

        if (touchAction>=6) {
            mouseButton = SDL_BUTTON_RIGHT;
            touchAction -=6;
        } else if (touchAction>=3) {
            mouseButton = SDL_BUTTON_MIDDLE;
            touchAction -=3;
        } else {
            mouseButton = 1;
        }

	    if( touchAction == MOUSE_DOWN ) {
            SDL_PrivateMouseMotion(SDL_RELEASED, 0, X, Y);
            SDL_PrivateMouseButton(SDL_PRESSED, mouseButton, X, Y);
            SDL_PrivateMouseMotion(SDL_PRESSED, 0, X, Y);
        }
        else if ( touchAction == MOUSE_UP ) {
		    SDL_PrivateMouseMotion(SDL_PRESSED, 0, X, Y);
		    SDL_PrivateMouseButton(SDL_RELEASED, mouseButton, X, Y);
            SDL_PrivateMouseMotion(SDL_RELEASED, 0, X, Y);
	    }
	    if( touchAction == MOUSE_MOVE )
		    SDL_PrivateMouseMotion(SDL_PRESSED, 0, X, Y);
    }

}


/* end of SDL_androidevents.c ... */


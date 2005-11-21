/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* System dependent library loading routines                           */

#if !SDL_INTERNAL_BUILDING_LOADSO
#error Do not compile directly...compile src/SDL_loadso.c instead!
#endif

#if !defined(macintosh)
#error Compiling for the wrong platform?
#endif

#include <stdio.h>
#include <string.h>
#define OLDP2C 1
#include <Strings.h>
#include <CodeFragments.h>
#include <Errors.h>

#include "SDL_types.h"
#include "SDL_error.h"
#include "SDL_loadso.h"

void *SDL_LoadObject(const char *sofile)
{
	void *handle = NULL;
	const char *loaderror = NULL;
	CFragConnectionID library_id;
	Ptr mainAddr;
	Str255 errName;
	OSErr error;
	char psofile[512];

	strncpy(psofile, sofile, SDL_TABLESIZE(psofile));
	psofile[SDL_TABLESIZE(psofile)-1] = '\0';
	error = GetSharedLibrary(C2PStr(psofile), kCompiledCFragArch,
			kLoadCFrag, &library_id, &mainAddr, errName);
	switch (error) {
		case noErr:
			loaderror = NULL;
			break;
		case cfragNoLibraryErr:
			loaderror = "Library not found";
			break;
		case cfragUnresolvedErr:
			loaderror = "Unabled to resolve symbols";
			break;
		case cfragNoPrivateMemErr:
		case cfragNoClientMemErr:
			loaderror = "Out of memory";
			break;
		default:
			loaderror = "Unknown Code Fragment Manager error";
			break;
	}
	if ( loaderror == NULL ) {
		handle = (void *)(library_id);
	} else {
		SDL_SetError("Failed loading %s: %s", sofile, loaderror);
	}
	return(handle);
}

void *SDL_LoadFunction(void *handle, const char *name)
{
	void *symbol = NULL;
	const char *loaderror = NULL;
	CFragSymbolClass class;
	CFragConnectionID library_id = (CFragConnectionID)handle;
	char pname[512];

	strncpy(pname, name, SDL_TABLESIZE(pname));
	pname[SDL_TABLESIZE(pname)-1] = '\0';
	if ( FindSymbol(library_id, C2PStr(pname),
	                (char **)&symbol, &class) != noErr ) {
		loaderror = "Symbol not found";
	}

	if ( symbol == NULL ) {
		SDL_SetError("Failed loading %s: %s", name, loaderror);
	}
	return(symbol);
}

void SDL_UnloadObject(void *handle)
{
	CFragConnectionID library_id;
	if ( handle != NULL ) {
		library_id = (CFragConnectionID)handle;
		CloseConnection(&library_id);
	}
}

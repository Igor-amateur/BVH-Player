// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here

#include<string>
#include<Mmsystem.h>
#include<Commdlg.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include"glext.h"

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "Winmm.lib")

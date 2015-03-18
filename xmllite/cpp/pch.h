//
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <shcore.h>
#include <wrl.h>
#include <xmllite.h>

#define ChkHr(stmt)	do { hr = stmt; if (FAILED(hr)) return hr; } while(0)

#include "App.xaml.h"

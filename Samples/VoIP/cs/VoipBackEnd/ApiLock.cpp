/* 
    Copyright (c) 2012 Microsoft Corporation.  All rights reserved.
    Use of this sample source code is subject to the terms of the Microsoft license 
    agreement under which you licensed this sample source code and is provided AS-IS.
    If you did not accept the terms of the license agreement, you are not authorized 
    to use this sample source code.  For the terms of the license, please see the 
    license agreement between you and Microsoft.
  
*/
#include "pch.h"
#include "ApiLock.h"

namespace VoipBackEnd
{
    // A mutex used to protect objects accessible from the API surface exposed by this DLL
    std::recursive_mutex g_apiLock;
}


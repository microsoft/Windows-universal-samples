//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#pragma once

// Level6:
// This class defines the sixth and final level of the game.  It derives from the
// fifth level.  In this level the targets do not disappear when they are hit.
// The target will stay highlighted for two seconds.  As this is the last level
// the only criteria for completion is time expiring.

#include "Level5.h"

ref class Level6: public Level5
{
internal:
    Level6();
    virtual bool Update(
        float time,
        float elapsedTime,
        float timeRemaining,
        std::vector<GameObject^> objects
        ) override;
};

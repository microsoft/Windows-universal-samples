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

// Level4:
// This class defines the fourth level of the game. It derives from the
// third level. The targets must be hit in numeric order.

#include "Level3.h"

class Level4 : public Level3
{
public:
    Level4();
    virtual void Initialize(std::vector<std::shared_ptr<GameObject>> const& objects) override;

    virtual bool Update(
        float time,
        float elapsedTime,
        float timeRemaining,
        std::vector<std::shared_ptr<GameObject>> const& objects
        ) override;

    virtual void SaveState(PersistentState& state) override;
    virtual void LoadState(PersistentState& state) override;

private:
    int m_nextId;
};

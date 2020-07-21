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

// Level2:
// This class defines the second level of the game. It derives from the
// first level. In this level the targets must be hit in numeric order.

#include "Level1.h"

class Level2 : public Level1
{
public:
    Level2();
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

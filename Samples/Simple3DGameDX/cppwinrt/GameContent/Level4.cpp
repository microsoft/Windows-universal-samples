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

#include "pch.h"
#include "Level4.h"
#include "Face.h"

Level4::Level4()
{
    m_timeLimit = 30.0f;
    m_objective = L"Hit each of the moving targets in ORDER before time runs out.";
}

void Level4::Initialize(std::vector<std::shared_ptr<GameObject>> const& objects)
{
    Level3::Initialize(objects);

    int targetCount = 0;
    for (auto&& object : objects)
    {
        if (auto target = dynamic_cast<Face*>(object.get()))
        {
            if (targetCount < 9)
            {
                target->Target(targetCount == 0 ? true : false);
                targetCount++;
            }
        }
    }
    m_nextId = 1;
}

bool Level4::Update(
    float /* time */,
    float /* elapsedTime */,
    float /* timeRemaining */,
    std::vector<std::shared_ptr<GameObject>> const& objects
    )
{
    int left = 0;
    for (auto&& object : objects)
    {
        if (object->Active() && (object->TargetId() > 0))
        {
            if (object->Hit() && (object->TargetId() == m_nextId))
            {
                object->Active(false);
                m_nextId++;
            }
            else
            {
                left++;
            }
        }
        if (object->Active() && (object->TargetId() == m_nextId))
        {
            object->Target(true);
        }
    }
    return (left == 0);
}

void Level4::SaveState(PersistentState& state)
{
    state.SaveInt32(L":NextTarget", m_nextId);
}

void Level4::LoadState(PersistentState& state)
{
    m_nextId = state.LoadInt32(L":NextTarget", 1);
}
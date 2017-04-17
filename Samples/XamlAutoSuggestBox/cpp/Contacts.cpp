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
#include "pch.h"
#include "Contacts.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;

std::vector<Contact^> CreateContactSampleData()
{
    auto contacts = std::vector<Contact^>();
    std::sort(contacts.begin(), contacts.end(), [](Contact^ a, Contact^ b) { return a->FullName < b->FullName; });
    return contacts;
}

std::vector<Contact^> ContactSampleDataSource::contacts;

task<void> ContactSampleDataSource::CreateContactSampleDataAsync()
{
    // Don't need to do this more than once.
    if (contacts.size() > 0)
    {
        return task_from_result();
    }

    return create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Uri("ms-appx:///Assets/Contacts.txt")))
        .then([](StorageFile^ file)
    {
        return create_task(FileIO::ReadLinesAsync(file));
    }).then([](IVector<String^>^ lines)
    {
        // Verify that it hasn't been done by someone else in the meantime.
        if (contacts.size() == 0)
        {
            for (unsigned i = 0; i < lines->Size; i += 3)
            {
                contacts.push_back(ref new Contact(lines->GetAt(i), lines->GetAt(i + 1), lines->GetAt(i + 2)));
            }
        }
    });
}

/// <summary>
/// Determine whether a string is contained in another string,
/// according to the current locale.
/// </summary>
/// <param name="source"></param>
// 
int FindString(Platform::String^ source, Platform::String^ value, DWORD flags)
{
    return FindNLSStringEx(nullptr, flags, source->Data(), source->Length(), value->Data(), value->Length(), nullptr, nullptr, nullptr, 0);
}

/// <summary>
/// Do a fuzzy search on all contacts and order results based on a pre-defined rule set
/// </summary>
/// <param name="query">The part of the name or company to look for</param>
/// <returns>An ordered list of contacts that matches the query</returns>
std::vector<Contact^> ContactSampleDataSource::GetMatchingContacts(Platform::String^ query)
{
    std::vector<Contact^> filteredContacts;
    std::copy_if(contacts.begin(), contacts.end(), std::back_inserter(filteredContacts),
        [query](Contact^ c)
    {
        return FindString(c->FirstName, query, NORM_IGNORECASE | FIND_FROMSTART) >= 0 ||
            FindString(c->LastName, query, NORM_IGNORECASE | FIND_FROMSTART) >= 0 ||
            FindString(c->Company, query, NORM_IGNORECASE | FIND_FROMSTART) >= 0;
    });

    auto create_sort_key = [query](Contact^ c)
    {
        return std::make_tuple(
            FindString(c->FirstName, query, NORM_IGNORECASE | FIND_STARTSWITH),
            FindString(c->LastName, query, NORM_IGNORECASE | FIND_STARTSWITH),
            FindString(c->Company, query, NORM_IGNORECASE | FIND_STARTSWITH));
    };

    std::sort(filteredContacts.begin(), filteredContacts.end(), [create_sort_key](Contact^ a, Contact^ b)
    {
        // Compare in reverse order to get descending sort.
        return create_sort_key(b) < create_sort_key(a);
    });

    return filteredContacts;
}

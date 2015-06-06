//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// CallengeResponseAlgorithm.h
// Declaration of the CallengeResponseAlgorithm class
//

#pragma once
using namespace Windows::Storage::Streams;

namespace Smartcard
{
	ref class ChallengeResponseAlgorithm sealed
	{
	public:
		static IBuffer^ CalculateResponse(IBuffer^ challenge, IBuffer^ adminkey);
	};
}

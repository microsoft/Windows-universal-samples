//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include <windows.h>
#include <ppltasks.h>
#include <concrt.h>
#include <collection.h>
#include <windows.devices.alljoyn.interop.h>
#include <map>

#include <alljoyn_c/busattachment.h>
#include <alljoyn_c/dbusstddefines.h>
#include <alljoyn_c/AboutData.h>
#include <alljoyn_c/AboutObj.h>
#include <alljoyn_c/AboutObjectDescription.h>

#include "AllJoynHelpers.h"
#define PROJECT_NAMESPACE org::alljoyn::Bus::Samples::Secure
#include "SecureInterfaceStructures.h"
#include "TypeConversionHelpers.h"

#include "SecureInterfaceMethodResults.h"
#include "SecureInterfaceEventArgs.h"
#include "ISecureInterfaceService.h"
#include "SecureInterfaceSignals.h"
#include "SecureInterfaceProducer.h"
#include "SecureInterfaceWatcher.h"
#include "SecureInterfaceConsumer.h"


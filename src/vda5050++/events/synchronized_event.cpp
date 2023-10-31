// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains static member functions of the synchronized event family.
//

#include "vda5050++/events/synchronized_event.h"

#include "vda5050++/core/common/exception.h"

vda5050pp::VDA5050PPSynchronizedEventNotAcquired
vda5050pp::events::_SynchronizedEventTokenStatic::notAcquiredException() {
  throw VDA5050PPSynchronizedEventNotAcquired(MK_FN_EX_CONTEXT(""));
}
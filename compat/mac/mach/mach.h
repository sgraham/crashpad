// Copyright 2014 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CRASHPAD_COMPAT_MAC_MACH_MACH_H_
#define CRASHPAD_COMPAT_MAC_MACH_MACH_H_

#include_next <mach/mach.h>

// <mach/exception_types.h>

// 10.8 SDK

#ifndef EXC_RESOURCE
#define EXC_RESOURCE 11
#endif

#ifndef EXC_MASK_RESOURCE
#define EXC_MASK_RESOURCE (1 << EXC_RESOURCE)
#endif

// 10.9 SDK

#ifndef EXC_GUARD
#define EXC_GUARD 12
#endif

#ifndef EXC_MASK_GUARD
#define EXC_MASK_GUARD (1 << EXC_GUARD)
#endif

// Don’t expose EXC_MASK_ALL or EXC_MASK_VALID at all, because their definitions
// vary with SDK, and older kernels will reject values that they don’t
// understand. Instead, use crashpad::ExcMaskAll(), which computes the correct
// value of EXC_MASK_ALL for the running system.
#undef EXC_MASK_ALL
#undef EXC_MASK_VALID

#if defined(__i386__) || defined(__x86_64__)

// <mach/i386/exception.h>

// 10.9 SDK

#if EXC_TYPES_COUNT > 13  // Definition varies with SDK
#error Update this file for new exception types
#elif EXC_TYPES_COUNT != 13
#undef EXC_TYPES_COUNT
#define EXC_TYPES_COUNT 13
#endif

// <mach/i386/thread_status.h>

// 10.8 SDK

#ifndef x86_AVX_STATE
#define x86_AVX_STATE 18
#endif

#endif  // defined(__i386__) || defined(__x86_64__)

// <mach/thread_status.h>

// 10.8 SDK

#ifndef THREAD_STATE_FLAVOR_LIST_10_9
#define THREAD_STATE_FLAVOR_LIST_10_9 129
#endif

#endif  // CRASHPAD_COMPAT_MAC_MACH_MACH_H_

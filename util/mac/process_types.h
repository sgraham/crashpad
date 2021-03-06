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

#ifndef CRASHPAD_UTIL_MAC_PROCESS_TYPES_H_
#define CRASHPAD_UTIL_MAC_PROCESS_TYPES_H_

#include <mach/mach.h>
#include <mach-o/dyld_images.h>
#include <mach-o/loader.h>
#include <stdint.h>
#include <sys/types.h>

#include "util/mac/process_reader.h"

namespace crashpad {
namespace process_types {
namespace internal {

// Some structure definitions differ in 32-bit and 64-bit environments by having
// additional “reserved” padding fields present only in the 64-bit environment.
// These Reserved64Only* types allow the process_types system to replicate these
// structures more precisely.
typedef char Reserved64Only32[0];
typedef uint32_t Reserved64Only64;

}  // namespace internal
}  // namespace process_types
}  // namespace crashpad

#include "util/mac/process_types/traits.h"

// Creates the traits type crashpad::process_types::internal::TraitsGeneric.
DECLARE_PROCESS_TYPE_TRAITS_CLASS(Generic, 64)

#undef DECLARE_PROCESS_TYPE_TRAITS_CLASS

// Declare the crashpad::process_types::struct_name structs. These are the
// user-visible generic structs that callers will interact with. They read data
// from 32-bit or 64-bit processes by using the specific internal templatized
// structs below.
#define PROCESS_TYPE_STRUCT_DECLARE 1

#define PROCESS_TYPE_STRUCT_BEGIN(struct_name)                                 \
  namespace crashpad {                                                         \
  namespace process_types {                                                    \
  struct struct_name {                                                         \
   public:                                                                     \
    typedef internal::TraitsGeneric::Long Long;                                \
    typedef internal::TraitsGeneric::ULong ULong;                              \
    typedef internal::TraitsGeneric::Pointer Pointer;                          \
    typedef internal::TraitsGeneric::IntPtr IntPtr;                            \
    typedef internal::TraitsGeneric::UIntPtr UIntPtr;                          \
    typedef internal::TraitsGeneric::Reserved64Only Reserved64Only;            \
                                                                               \
    /* Initializes an object with data read from |process_reader| at           \
     * |address|, properly genericized. */                                     \
    bool Read(ProcessReader* process_reader, mach_vm_address_t address) {      \
      return ReadInto(process_reader, address, this);                          \
    }                                                                          \
                                                                               \
    /* Reads |count| objects from |process_reader| beginning at |address|, and \
     * genericizes the objects. The caller must provide storage for |count|    \
     * objects in |generic|. */                                                \
    static bool ReadArrayInto(ProcessReader* process_reader,                   \
                              mach_vm_address_t address,                       \
                              size_t count,                                    \
                              struct_name* generic);                           \
                                                                               \
    /* Returns the size of the object that was read. This is the size of the   \
     * storage in the process that the data is read from, and is not the same  \
     * as the size of the generic struct. */                                   \
    size_t Size() const { return size_; }                                      \
                                                                               \
    /* Similar to Size(), but computes the expected size of a structure based  \
     * on the process’ bitness. This can be used prior to reading any data     \
     * from a process. */                                                      \
    static size_t ExpectedSize(ProcessReader* process_reader);

#define PROCESS_TYPE_STRUCT_MEMBER(member_type, member_name, ...)              \
    member_type member_name __VA_ARGS__;

#define PROCESS_TYPE_STRUCT_END(struct_name)                                   \
   private:                                                                    \
    /* The static form of Read(). Populates the struct at |generic|. */        \
    static bool ReadInto(ProcessReader* process_reader,                        \
                         mach_vm_address_t address,                            \
                         struct_name* generic);                                \
                                                                               \
    template <typename T>                                                      \
    static bool ReadIntoInternal(ProcessReader* process_reader,                \
                                 mach_vm_address_t address,                    \
                                 struct_name* generic);                        \
    template <typename T>                                                      \
    static bool ReadArrayIntoInternal(ProcessReader* process_reader,           \
                                      mach_vm_address_t address,               \
                                      size_t count,                            \
                                      struct_name* generic);                   \
    size_t size_;                                                              \
  };                                                                           \
  }  /* namespace process_types */                                             \
  }  /* namespace crashpad */

#include "util/mac/process_types/all.proctype"

#undef PROCESS_TYPE_STRUCT_BEGIN
#undef PROCESS_TYPE_STRUCT_MEMBER
#undef PROCESS_TYPE_STRUCT_END
#undef PROCESS_TYPE_STRUCT_DECLARE

// Declare the templatized crashpad::process_types::internal::struct_name<>
// structs. These are the 32-bit and 64-bit specific structs that describe the
// layout of objects in another process. This is repeated instead of being
// shared with the generic declaration above because both the generic and
// templatized specific structs need all of the struct members declared.
//
//
// GenericizeInto() translates a struct from the representation used in the
// remote process into the generic form.
#define PROCESS_TYPE_STRUCT_DECLARE_INTERNAL 1

#define PROCESS_TYPE_STRUCT_BEGIN(struct_name)                                 \
  namespace crashpad {                                                         \
  namespace process_types {                                                    \
  namespace internal {                                                         \
  template <typename Traits>                                                   \
  struct struct_name {                                                         \
   public:                                                                     \
    typedef typename Traits::Long Long;                                        \
    typedef typename Traits::ULong ULong;                                      \
    typedef typename Traits::Pointer Pointer;                                  \
    typedef typename Traits::IntPtr IntPtr;                                    \
    typedef typename Traits::UIntPtr UIntPtr;                                  \
    typedef typename Traits::Reserved64Only Reserved64Only;                    \
                                                                               \
    /* Read(), ReadArrayInto(), and Size() are as in the generic user-visible  \
     * struct above. */                                                        \
    bool Read(ProcessReader* process_reader, mach_vm_address_t address) {      \
      return ReadInto(process_reader, address, this);                          \
    }                                                                          \
    static bool ReadArrayInto(ProcessReader* process_reader,                   \
                              mach_vm_address_t address,                       \
                              size_t count,                                    \
                              struct_name<Traits>* specific);                  \
    static size_t Size() { return sizeof(struct_name<Traits>); }               \
                                                                               \
    /* Translates a struct from the representation used in the remote process  \
     * into the generic form. */                                               \
    void GenericizeInto(process_types::struct_name* generic,                   \
                        size_t* specific_size);

#define PROCESS_TYPE_STRUCT_MEMBER(member_type, member_name, ...)              \
    member_type member_name __VA_ARGS__;

#define PROCESS_TYPE_STRUCT_END(struct_name)                                   \
   private:                                                                    \
    /* ReadInto() is as in the generic user-visible struct above. */           \
    static bool ReadInto(ProcessReader* process_reader,                        \
                         mach_vm_address_t address,                            \
                         struct_name<Traits>* specific);                       \
  };                                                                           \
  }  /* namespace internal */                                                  \
  }  /* namespace process_types */                                             \
  }  /* namespace crashpad */

#include "util/mac/process_types/all.proctype"

#undef PROCESS_TYPE_STRUCT_BEGIN
#undef PROCESS_TYPE_STRUCT_MEMBER
#undef PROCESS_TYPE_STRUCT_END
#undef PROCESS_TYPE_STRUCT_DECLARE_INTERNAL

#endif  // CRASHPAD_UTIL_MAC_PROCESS_TYPES_H_

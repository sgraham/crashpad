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

#ifndef CRASHPAD_SNAPSHOT_MODULE_SNAPSHOT_H_
#define CRASHPAD_SNAPSHOT_MODULE_SNAPSHOT_H_

#include <stdint.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <vector>

#include "util/misc/uuid.h"

namespace crashpad {

//! \brief An abstract interface to a snapshot representing a code module
//!     (binary image) loaded into a snapshot process.
class ModuleSnapshot {
 public:
  //! \brief A module’s type.
  enum ModuleType {
    //! \brief The module’s type is unknown.
    kModuleTypeUnknown = 0,

    //! \brief The module is a main executable.
    kModuleTypeExecutable,

    //! \brief The module is a shared library.
    //!
    //! \sa kModuleTypeLoadableModule
    kModuleTypeSharedLibrary,

    //! \brief The module is a loadable module.
    //!
    //! On some platforms, loadable modules are distinguished from shared
    //! libraries. On these platforms, a shared library is a module that another
    //! module links against directly, and a loadable module is not. Loadable
    //! modules tend to be binary plug-ins.
    kModuleTypeLoadableModule,

    //! \brief The module is a dynamic loader.
    //!
    //! This is the module responsible for loading other modules. This is
    //! normally `dyld` for Mac OS X and `ld.so` for Linux and other systems
    //! using ELF.
    kModuleTypeDynamicLoader,
  };

  //! \brief Returns the module’s pathname.
  virtual std::string Name() const = 0;

  //! \brief Returns the base address that the module is loaded at in the
  //!     snapshot process.
  virtual uint64_t Address() const = 0;

  //! \brief Returns the size that the module occupies in the snapshot process’
  //!     address space, starting at its base address.
  //!
  //! For Mac OS X snapshots, this method only reports the size of the `__TEXT`
  //! segment, because segments may not be loaded contiguously.
  virtual uint64_t Size() const = 0;

  //! \brief Returns the module’s timestamp, if known.
  //!
  //! The timestamp is typically the modification time of the file that provided
  //! the module in `time_t` format, seconds since the POSIX epoch. If the
  //! module’s timestamp is unknown, this method returns `0`.
  virtual time_t Timestamp() const = 0;

  //! \brief Returns the module’s file version in the \a version_* parameters.
  //!
  //! If no file version can be determined, the \a version_* parameters are set
  //! to `0`.
  //!
  //! For Mac OS X snapshots, this is taken from the module’s `LC_ID_DYLIB` load
  //! command for shared libraries, and is `0` for other module types.
  virtual void FileVersion(uint16_t* version_0,
                           uint16_t* version_1,
                           uint16_t* version_2,
                           uint16_t* version_3) const = 0;

  //! \brief Returns the module’s source version in the \a version_* parameters.
  //!
  //! If no source version can be determined, the \a version_* parameters are
  //! set to `0`.
  //!
  //! For Mac OS X snapshots, this is taken from the module’s
  //! `LC_SOURCE_VERSION` load command.
  virtual void SourceVersion(uint16_t* version_0,
                             uint16_t* version_1,
                             uint16_t* version_2,
                             uint16_t* version_3) const = 0;

  //! \brief Returns the module’s type.
  virtual ModuleType GetModuleType() const = 0;

  //! \brief Returns the module’s UUID in the \a uuid parameter.
  //!
  //! A snapshot module’s UUID is taken directly from the module itself. If the
  //! module does not have a UUID, the \a uuid parameter will be zeroed out.
  virtual void UUID(crashpad::UUID* uuid) const = 0;

  //! \brief Returns diagnostic messages recorded in the module.
  //!
  //! This method retrieves diagnostic messages recorded in a module. These
  //! messages are intended for diagnostic use, including crash analysis. A
  //! module may contain multiple diagnostic messages.
  //!
  //! For Mac OS X snapshots, the diagnostic messages are found by interpreting
  //! the module’s `__DATA, __crash_info` section as
  //! `crashreporter_annotations_t`. System libraries using the crash reporter
  //! client interface may reference diagnostic messages in this structure.
  //! Additional diagnostic messages may be found in other locations, which may
  //! be module-specific. The dynamic linker (`dyld`) can provide a diagnostic
  //! message at its `_error_string` symbol.
  virtual std::vector<std::string> DiagnosticMessages() const = 0;

  //! \brief Returns simple annotations recorded in the module.
  //!
  //! This method retrieves simple annotations recorded in a module. These
  //! annotations are intended for diagnostic use, including crash analysis.
  //! Simple annotations are structured as a sequence of key-value pairs. These
  //! are referred to in Chrome as “crash keys.”
  //!
  //! For Mac OS X snapshots, simple annotations are found by interpreting
  //! the `__DATA, __crashpad_info` section as `CrashpadInfo`. Clients can use
  //! the Crashpad client interface to store annotations in this structure.
  virtual std::map<std::string, std::string> SimpleAnnotations() const = 0;

 protected:
  ~ModuleSnapshot() {}
};

}  // namespace crashpad

#endif  // CRASHPAD_SNAPSHOT_MODULE_SNAPSHOT_H_

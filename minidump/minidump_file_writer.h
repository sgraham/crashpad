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

#ifndef CRASHPAD_MINIDUMP_MINIDUMP_FILE_WRITER_H_
#define CRASHPAD_MINIDUMP_MINIDUMP_FILE_WRITER_H_

#include <dbghelp.h>
#include <sys/types.h>

#include <set>
#include <vector>

#include "base/basictypes.h"
#include "minidump/minidump_stream_writer.h"
#include "minidump/minidump_writable.h"
#include "util/file/file_writer.h"

namespace crashpad {

//! \brief The root-level object in a minidump file.
//!
//! This object writes a MINIDUMP_HEADER and list of MINIDUMP_DIRECTORY entries
//! to a minidump file.
class MinidumpFileWriter final : public internal::MinidumpWritable {
 public:
  MinidumpFileWriter();
  ~MinidumpFileWriter();

  //! \brief Sets MINIDUMP_HEADER::Timestamp.
  //!
  //! \note Valid in #kStateMutable.
  void SetTimestamp(time_t timestamp);

  //! \brief Adds a stream to the minidump file as a child of the object, and
  //!     arranges for a MINIDUMP_DIRECTORY entry to point to it.
  //!
  //! At most one object of each stream type (as obtained from
  //! internal::MinidumpStreamWriter::StreamType()) may be added to a
  //! MinidumpFileWriter object. It is an error to attempt to add multiple
  //! streams with the same stream type.
  //!
  //! \note Valid in #kStateMutable.
  void AddStream(internal::MinidumpStreamWriter* stream);

  // MinidumpWritable:

  //! \copydoc internal::MinidumpWritable::WriteEverything()
  //!
  //! This method does not initially write the final value for
  //! MINIDUMP_HEADER::Signature. After all child objects have been written, it
  //! rewinds to the beginning of the file and writes the correct value for this
  //! field. This prevents incompletely-written minidump files from being
  //! mistaken for valid ones.
  virtual bool WriteEverything(FileWriterInterface* file_writer) override;

 protected:
  // MinidumpWritable:
  virtual bool Freeze() override;
  virtual size_t SizeOfObject() override;
  virtual std::vector<MinidumpWritable*> Children() override;
  virtual bool WillWriteAtOffsetImpl(off_t offset) override;
  virtual bool WriteObject(FileWriterInterface* file_writer) override;

 private:
  MINIDUMP_HEADER header_;
  std::vector<internal::MinidumpStreamWriter*> streams_;  // weak

  // Protects against multiple streams with the same ID being added.
  std::set<MinidumpStreamType> stream_types_;

  DISALLOW_COPY_AND_ASSIGN(MinidumpFileWriter);
};

}  // namespace crashpad

#endif  // CRASHPAD_MINIDUMP_MINIDUMP_WRITER_H_

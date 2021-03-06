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

#include "util/file/fd_io.h"

#include <unistd.h>

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/posix/eintr_wrapper.h"

namespace {

struct ReadTraits {
  typedef void* VoidBufferType;
  typedef char* CharBufferType;
  static ssize_t Operate(int fd, CharBufferType buffer, size_t size) {
    return read(fd, buffer, size);
  }
};

struct WriteTraits {
  typedef const void* VoidBufferType;
  typedef const char* CharBufferType;
  static ssize_t Operate(int fd, CharBufferType buffer, size_t size) {
    return write(fd, buffer, size);
  }
};

template <typename Traits>
ssize_t ReadOrWrite(int fd,
                    typename Traits::VoidBufferType buffer,
                    size_t size) {
  typename Traits::CharBufferType buffer_c =
      reinterpret_cast<typename Traits::CharBufferType>(buffer);

  ssize_t total_bytes = 0;
  while (size > 0) {
    ssize_t bytes = HANDLE_EINTR(Traits::Operate(fd, buffer_c, size));
    if (bytes < 0) {
      return bytes;
    } else if (bytes == 0) {
      break;
    }

    buffer_c += bytes;
    size -= bytes;
    total_bytes += bytes;
  }

  return total_bytes;
}

}  // namespace

namespace crashpad {

ssize_t ReadFD(int fd, void* buffer, size_t size) {
  return ReadOrWrite<ReadTraits>(fd, buffer, size);
}

ssize_t WriteFD(int fd, const void* buffer, size_t size) {
  return ReadOrWrite<WriteTraits>(fd, buffer, size);
}

void CheckedReadFD(int fd, void* buffer, size_t size) {
  ssize_t expect = base::checked_cast<ssize_t>(size);
  ssize_t rv = ReadFD(fd, buffer, size);
  if (rv < 0) {
    PCHECK(rv == expect) << "read";
  } else {
    CHECK_EQ(rv, expect) << "read";
  }
}

void CheckedWriteFD(int fd, const void* buffer, size_t size) {
  ssize_t expect = base::checked_cast<ssize_t>(size);
  ssize_t rv = WriteFD(fd, buffer, size);
  if (rv < 0) {
    PCHECK(rv == expect) << "write";
  } else {
    CHECK_EQ(rv, expect) << "write";
  }
}

void CheckedReadFDAtEOF(int fd) {
  char c;
  ssize_t rv = ReadFD(fd, &c, 1);
  if (rv < 0) {
    PCHECK(rv == 0) << "read";
  } else {
    CHECK_EQ(rv, 0) << "read";
  }
}

}  // namespace crashpad

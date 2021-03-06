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

#ifndef CRASHPAD_UTIL_FILE_FD_IO_H_
#define CRASHPAD_UTIL_FILE_FD_IO_H_

#include <sys/types.h>

namespace crashpad {

//! \brief Wraps `read()`, retrying when interrupted or following a short read.
//!
//! This function reads into \a buffer, stopping only when \a size bytes have
//! been read or when `read()` returns 0, indicating that end-of-file has been
//! reached.
//!
//! \return The number of bytes read and placed into \a buffer, or `-1` on
//!     error, with `errno` set appropriately. On error, a portion of \a fd may
//!     have been read into \a buffer.
//!
//! \sa WriteFD
//! \sa CheckedReadFD
//! \sa CheckedReadFDAtEOF
ssize_t ReadFD(int fd, void* buffer, size_t size);

//! \brief Wraps `write()`, retrying when interrupted or following a short
//!     write.
//!
//! This function writes to \a fd, stopping only when \a size bytes have been
//! written.
//!
//! \return The number of bytes written from \a buffer, or `-1` on error, with
//!     `errno` set appropriately. On error, a portion of \a buffer may have
//!     been written to \a fd.
//!
//! \sa ReadFD
//! \sa CheckedWriteFD
ssize_t WriteFD(int fd, const void* buffer, size_t size);

//! \brief Wraps ReadFD(), ensuring that exactly \a size bytes are read.
//!
//! If \a size is out of the range of possible `read()` return values, if the
//! underlying ReadFD() fails, or if other than \a size bytes were read, this
//! function causes execution to terminate without returning.
//!
//! \sa CheckedWriteFD
//! \sa ReadFD
//! \sa CheckedReadFDAtEOF
void CheckedReadFD(int fd, void* buffer, size_t size);

//! \brief Wraps WriteFD(), ensuring that exactly \a size bytes are written.
//!
//! If \a size is out of the range of possible `write()` return values, if the
//! underlying WriteFD() fails, or if other than \a size bytes were written,
//! this function causes execution to terminate without returning.
//!
//! \sa CheckedReadFD
//! \sa WriteFD
void CheckedWriteFD(int fd, const void* buffer, size_t size);

//! \brief Wraps ReadFD(), ensuring that it indicates end-of-file.
//!
//! Attempts to read a single byte from \a fd, expecting no data to be read. If
//! the underlying ReadFD() fails, or if a byte actually is read, this function
//! causes execution to terminate without returning.
//!
//! \sa CheckedReadFD
//! \sa ReadFD
void CheckedReadFDAtEOF(int fd);

}  // namespace crashpad

#endif  // CRASHPAD_UTIL_FILE_FD_IO_H_

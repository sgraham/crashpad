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

#ifndef CRASHPAD_UTIL_MACH_MACH_MESSAGE_SERVER_H_
#define CRASHPAD_UTIL_MACH_MACH_MESSAGE_SERVER_H_

#include <mach/mach.h>

#include "base/basictypes.h"

namespace crashpad {

//! \brief Runs a Mach message server to handle a Mach RPC request for MIG
//!     servers.
//!
//! The principal entry point to this interface is the static Run() method.
class MachMessageServer {
 public:
  //! \brief A Mach RPC callback interface, called by Run().
  class Interface {
   public:
    //! \brief Handles a Mach RPC request.
    //!
    //! This method is a stand-in for a MIG-generated Mach RPC server “demux”
    //! function such as `exc_server()` and `mach_exc_server()`. Implementations
    //! may call such a function directly. This method is expected to behave
    //! exactly as these functions behave.
    //!
    //! \param[in] in The request message, received as a Mach message. Note that
    //!     this interface uses a `const` parameter for this purpose, whereas
    //!     MIG-generated “demux” functions do not.
    //! \param[out] out The reply message. The caller allocates storage, and the
    //!     callee is expected to populate the reply message appropriately.
    //!     After returning, the caller will send this reply as a Mach message
    //!     via the message’s reply port.
    //! \param[out] destroy_complex_request `true` if a complex request message
    //!     is to be destroyed even when handled successfully, `false`
    //!     otherwise. The traditional behavior is `false`. In this case, the
    //!     caller only destroys the request message in \a in when the reply
    //!     message in \a out is not complex and when it indicates a return code
    //!     other than `KERN_SUCCESS` or `MIG_NO_REPLY`. The assumption is that
    //!     the rights or out-of-line data carried in a complex message may be
    //!     retained by the server in this situation, and that it is the
    //!     responsibility of the server to release these resources as needed.
    //!     However, in many cases, these resources are not needed beyond the
    //!     duration of a request-reply transaction, and in such cases, it is
    //!     less error-prone to always have the caller,
    //!     MachMessageServer::Run(), destroy complex request messages. To
    //!     choose this behavior, this parameter should be set to `true`.
    //!
    //! \return `true` on success and `false` on failure, although the caller
    //!     ignores the return value. However, the return code to be included in
    //!     the reply message should be set as `mig_reply_error_t::RetCode`. The
    //!     non-`void` return value is used for increased compatibility with
    //!     MIG-generated functions.
    virtual bool MachMessageServerFunction(const mach_msg_header_t* in,
                                           mach_msg_header_t* out,
                                           bool* destroy_complex_request) = 0;

    //! \return The expected or maximum size, in bytes, of a request message to
    //!     be received as the \a in parameter of MachMessageServerFunction().
    virtual mach_msg_size_t MachMessageServerRequestSize() = 0;

    //! \return The maximum size, in bytes, of a reply message to be sent via
    //!     the \a out parameter of MachMessageServerFunction(). This value does
    //!     not need to include the size of any trailer to be sent with the
    //!     message.
    virtual mach_msg_size_t MachMessageServerReplySize() = 0;

   protected:
    ~Interface() {}
  };

  //! \brief Informs Run() whether to handle a single request-reply transaction
  //!     or to run in a loop.
  enum Persistent {
    //! \brief Handle a single request-reply transaction and then return.
    kOneShot = false,

    //! \brief Run in a loop, potentially handling multiple request-reply
    //!     transactions.
    kPersistent,
  };

  //! \brief Informs Run() whether or not to block while waiting for requests.
  enum Nonblocking {
    //! \brief Wait for a request message if none is queued.
    kBlocking = false,

    //! \brief Return as soon as there no request messages queued. This may
    //!     result in an immediate return without handling any requests.
    kNonblocking,
  };

  //! \brief Runs a Mach message server to handle a Mach RPC request for MIG
  //!     servers.
  //!
  //! This function listens for a request message and passes it to a callback
  //! interface. A reponse is collected from that interface, and is sent back as
  //! a reply.
  //!
  //! This function is similar to `mach_msg_server()` and
  //! `mach_msg_server_once()`.
  //!
  //! \param[in] interface The MachMessageServerInterface that is responsible
  //!     for handling the message. Interface::MachMessageServerRequestSize() is
  //!     used as the receive size for the request message, and
  //!     Interface::MachMessageServerReplySize() is used as the
  //!     maximum size of the reply message. If \a options contains
  //!     `MACH_RCV_LARGE`, this function will retry a receive operation that
  //!     returns `MACH_RCV_TOO_LARGE` with an appropriately-sized buffer.
  //!     MachMessageServerInterface::MachMessageServerFunction() is called to
  //!     handle the request and populate the reply.
  //! \param[in] receive_port The port on which to receive the request message.
  //! \param[in] options Options suitable for mach_msg. For the defaults, use
  //!     `MACH_MSG_OPTION_NONE`.
  //! \param[in] persistent Chooses between one-shot and persistent operation.
  //! \param[in] nonblocking Chooses between blocking and nonblocking operation.
  //! \param[in] timeout_ms When \a nonblocking is `false`, the the maximum
  //!     duration that this entire function will run, in milliseconds, or
  //!     `MACH_MSG_TIMEOUT_NONE` to specify no timeout (infinite waiting). When
  //!     \a nonblocking is `true`, this parameter has no effect. When \a
  //!     persistent is `true`, the timeout applies to the overall duration of
  //!     this function, not to any individual `mach_msg()` call.
  //!
  //! \return On success, `KERN_SUCCESS` (when \a persistent is `false`) or
  //!     `MACH_RCV_TIMED_OUT` (when \a persistent and \a nonblocking are both
  //!     `true`, or when \a persistent is `true`, \a nonblocking is `false`,
  //!     and \a timeout is not `MACH_MSG_TIMEOUT_NONE`. This function has no
  //!     successful return value when \a persistent is `true`, \a nonblocking
  //!     is `false`, and \a timeout is `MACH_MSG_TIMEOUT_NONE`. On failure,
  //!     returns a value identifying the nature of the error.
  static mach_msg_return_t Run(Interface* interface,
                               mach_port_t receive_port,
                               mach_msg_options_t options,
                               Persistent persistent,
                               Nonblocking nonblocking,
                               mach_msg_timeout_t timeout_ms);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(MachMessageServer);
};

}  // namespace crashpad

#endif  // CRASHPAD_UTIL_MACH_MACH_MESSAGE_SERVER_H_

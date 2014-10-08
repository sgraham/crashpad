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

#ifndef CRASHPAD_SNAPSHOT_SNAPSHOT_CPU_CONTEXT_H_
#define CRASHPAD_SNAPSHOT_SNAPSHOT_CPU_CONTEXT_H_

#include <stdint.h>

#include "snapshot/cpu_architecture.h"

namespace crashpad {

//! \brief A context structure carrying 32-bit x86 CPU state.
struct CPUContextX86 {
  typedef uint8_t X87Register[10];

  union X87OrMMXRegister {
    struct {
      X87Register st;
      uint8_t st_reserved[6];
    };
    struct {
      uint8_t mm_value[8];
      uint8_t mm_reserved[8];
    };
  };

  typedef uint8_t XMMRegister[16];

  struct Fxsave {
    uint16_t fcw;  // FPU control word
    uint16_t fsw;  // FPU status word
    uint8_t ftw;  // abridged FPU tag word
    uint8_t reserved_1;
    uint16_t fop;  // FPU opcode
    uint32_t fpu_ip;  // FPU instruction pointer offset
    uint16_t fpu_cs;  // FPU instruction pointer segment selector
    uint16_t reserved_2;
    uint32_t fpu_dp;  // FPU data pointer offset
    uint16_t fpu_ds;  // FPU data pointer segment selector
    uint16_t reserved_3;
    uint32_t mxcsr;  // multimedia extensions status and control register
    uint32_t mxcsr_mask;  // valid bits in mxcsr
    X87OrMMXRegister st_mm[8];
    XMMRegister xmm[8];
    uint8_t reserved_4[176];
    uint8_t available[48];
  };

  // Integer registers.
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t edi;  // destination index
  uint32_t esi;  // source index
  uint32_t ebp;  // base pointer
  uint32_t esp;  // stack pointer
  uint32_t eip;  // instruction pointer
  uint32_t eflags;
  uint16_t cs;  // code segment selector
  uint16_t ds;  // data segment selector
  uint16_t es;  // extra segment selector
  uint16_t fs;
  uint16_t gs;
  uint16_t ss;  // stack segment selector

  // Floating-point and vector registers.
  Fxsave fxsave;

  // Debug registers.
  uint32_t dr0;
  uint32_t dr1;
  uint32_t dr2;
  uint32_t dr3;
  uint32_t dr4;  // obsolete, normally an alias for dr6
  uint32_t dr5;  // obsolete, normally an alias for dr7
  uint32_t dr6;
  uint32_t dr7;
};

//! \brief A context structure carrying x86_64 CPU state.
struct CPUContextX86_64 {
  typedef CPUContextX86::X87Register X87Register;
  typedef CPUContextX86::X87OrMMXRegister X87OrMMXRegister;
  typedef CPUContextX86::XMMRegister XMMRegister;

  struct Fxsave {
    uint16_t fcw;  // FPU control word
    uint16_t fsw;  // FPU status word
    uint8_t ftw;  // abridged FPU tag word
    uint8_t reserved_1;
    uint16_t fop;  // FPU opcode
    union {
      // The expression of these union members is determined by the use of
      // fxsave/fxrstor or fxsave64/fxrstor64 (fxsaveq/fxrstorq). Mac OS X and
      // Windows systems use the traditional fxsave/fxrstor structure.
      struct {
        // fxsave/fxrstor
        uint32_t fpu_ip;  // FPU instruction pointer offset
        uint16_t fpu_cs;  // FPU instruction pointer segment selector
        uint16_t reserved_2;
        uint32_t fpu_dp;  // FPU data pointer offset
        uint16_t fpu_ds;  // FPU data pointer segment selector
        uint16_t reserved_3;
      };
      struct {
        // fxsave64/fxrstor64 (fxsaveq/fxrstorq)
        uint64_t fpu_ip_64;  // FPU instruction pointer
        uint64_t fpu_dp_64;  // FPU data pointer
      };
    };
    uint32_t mxcsr;  // multimedia extensions status and control register
    uint32_t mxcsr_mask;  // valid bits in mxcsr
    X87OrMMXRegister st_mm[8];
    XMMRegister xmm[16];
    uint8_t reserved_4[48];
    uint8_t available[48];
  };

  // Integer registers.
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rdi;  // destination index
  uint64_t rsi;  // source index
  uint64_t rbp;  // base pointer
  uint64_t rsp;  // stack pointer
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rip;  // instruction pointer
  uint64_t rflags;
  uint16_t cs;  // code segment selector
  uint16_t fs;
  uint16_t gs;

  // Floating-point and vector registers.
  Fxsave fxsave;

  // Debug registers.
  uint64_t dr0;
  uint64_t dr1;
  uint64_t dr2;
  uint64_t dr3;
  uint64_t dr4;  // obsolete, normally an alias for dr6
  uint64_t dr5;  // obsolete, normally an alias for dr7
  uint64_t dr6;
  uint64_t dr7;
};

//! \brief A context structure capable of carrying the context of any supported
//!     CPU architecture.
struct CPUContext {
  //! \brief Returns the instruction pointer value from the context structure.
  //!
  //! This is a CPU architecture-independent method that is capable of
  //! recovering the instruction pointer from any supported CPU architecture’s
  //! context structure.
  uint64_t InstructionPointer() const;

  //! \brief The CPU architecture of a context structure. This field controls
  //!     the expression of the union.
  CPUArchitecture architecture;
  union {
    CPUContextX86* x86;
    CPUContextX86_64* x86_64;
  };
};

}  // namespace crashpad

#endif  // CRASHPAD_SNAPSHOT_SNAPSHOT_CPU_CONTEXT_H_

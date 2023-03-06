//===-- CTargetMachine.h - TargetMachine for the C backend ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetMachine that is used by the C backend.
//
//===----------------------------------------------------------------------===//

#ifndef CTARGETMACHINE_H
#define CTARGETMACHINE_H

#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class CTargetLowering : public TargetLowering {
public:
  explicit CTargetLowering(const TargetMachine &TM) : TargetLowering(TM) {
    setMaxAtomicSizeInBitsSupported(0);
  }
};

class CTargetSubtargetInfo : public TargetSubtargetInfo {
public:
#if LLVM_VERSION_MAJOR >= 12
  CTargetSubtargetInfo(const TargetMachine &TM, const Triple &TT, StringRef CPU,
                       StringRef TuneCPU,StringRef FS)
#else
  CTargetSubtargetInfo(const TargetMachine &TM, const Triple &TT, StringRef CPU,
                       StringRef FS)
#endif
#if LLVM_VERSION_MAJOR >= 9
#if LLVM_VERSION_MAJOR >= 12
      : TargetSubtargetInfo(TT, CPU,TuneCPU, FS, ArrayRef<SubtargetFeatureKV>(),
                            ArrayRef<SubtargetSubTypeKV>(), nullptr, nullptr,
                            nullptr, nullptr, nullptr, nullptr),
#else
      : TargetSubtargetInfo(TT, CPU, FS, ArrayRef<SubtargetFeatureKV>(),
                            ArrayRef<SubtargetSubTypeKV>(), nullptr, nullptr,
                            nullptr, nullptr, nullptr, nullptr),
#endif
#else
      : TargetSubtargetInfo(TT, CPU, FS, ArrayRef<SubtargetFeatureKV>(),
                            ArrayRef<SubtargetFeatureKV>(), nullptr, nullptr,
                            nullptr, nullptr, nullptr, nullptr, nullptr),
#endif
        Lowering(TM) {
  }
  bool enableAtomicExpand() const override;
  const TargetLowering *getTargetLowering() const override;
  const CTargetLowering Lowering;
};

class CTargetMachine : public LLVMTargetMachine {
public:
  CTargetMachine(const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
                 const TargetOptions &Options, std::optional<Reloc::Model> RM,
                 std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL,
                 bool /*JIT*/)
      : LLVMTargetMachine(T, "", TT, CPU, FS, Options,
                          RM.has_value() ? RM.value() : Reloc::Static,
                          CM.has_value() ? CM.value() : CodeModel::Small, OL),
#if LLVM_VERSION_MAJOR >= 12
        SubtargetInfo(*this, TT, CPU,"", FS) {}
#else
        SubtargetInfo(*this, TT, CPU, FS) {}
#endif

  /// Add passes to the specified pass manager to get the specified file
  /// emitted.  Typically this will involve several steps of code generation.
  bool addPassesToEmitFile(PassManagerBase &PM, raw_pwrite_stream &Out,
#if LLVM_VERSION_MAJOR >= 7
                           raw_pwrite_stream *DwoOut,
#endif
                           CodeGenFileType FileType, bool DisableVerify = true,
#if LLVM_VERSION_MAJOR >= 10
                           MachineModuleInfoWrapperPass *MMI = nullptr
#else
                           MachineModuleInfo *MMI = nullptr
#endif
                           ) override;

  // TargetMachine interface
  const TargetSubtargetInfo *getSubtargetImpl(const Function &) const override;
  const CTargetSubtargetInfo SubtargetInfo;
};

extern Target TheCBackendTarget;

} // namespace llvm

#endif

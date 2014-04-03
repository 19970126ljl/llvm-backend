//===-- LEGTargetMachine.h - Define TargetMachine for LEG ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the LEG specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LEGTARGETMACHINE_H
#define LEGTARGETMACHINE_H

#include "LEGFrameLowering.h"
#include "LEGISelLowering.h"
#include "LEGInstrInfo.h"
#include "LEGSelectionDAGInfo.h"
#include "LEGSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class LEGTargetMachine : public LLVMTargetMachine {
  LEGSubtarget Subtarget;
  const DataLayout DL;
  LEGInstrInfo InstrInfo;
  LEGFrameLowering FrameLowering;
  LEGTargetLowering TLInfo;
  LEGSelectionDAGInfo TSInfo;

public:
  LEGTargetMachine(const Target &T, StringRef TT, StringRef CPU, StringRef FS,
                   const TargetOptions &Options, Reloc::Model RM,
                   CodeModel::Model CM, CodeGenOpt::Level OL);

  virtual const LEGInstrInfo *getInstrInfo() const { return &InstrInfo; }
  virtual const LEGFrameLowering *getFrameLowering() const {
    return &FrameLowering;
  }
  virtual const LEGSubtarget *getSubtargetImpl() const { return &Subtarget; }
  virtual const LEGTargetLowering *getTargetLowering() const { return &TLInfo; }

  virtual const LEGSelectionDAGInfo *getSelectionDAGInfo() const {
    return &TSInfo;
  }

  virtual const TargetRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
  virtual const DataLayout *getDataLayout() const { return &DL; }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);

  virtual void addAnalysisPasses(PassManagerBase &PM);
};

} // end namespace llvm

#endif

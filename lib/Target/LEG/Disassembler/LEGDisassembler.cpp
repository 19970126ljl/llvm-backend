//===-- LEGDisassembler.cpp - Disassembler for LEG --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LEGDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCDisassembler.h"
#include "MCTargetDesc/LEGMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "leg-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class LEGDisassembler : public MCDisassembler {
public:
  LEGDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  ~LEGDisassembler() override {};

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createLEGDisassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new LEGDisassembler(STI, Ctx);
}

extern "C" void LLVMInitializeLEGDisassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(TheLEGTarget,
                                         createLEGDisassembler);
}

static const unsigned GPRDecoderTable[] = {
  LEG::R0, LEG::R1, LEG::R2, LEG::R3, LEG::R4, LEG::R5, LEG::R6, LEG::R7, LEG::R8, LEG::R9, LEG::SP,
};

static DecodeStatus DecodeGRRegsRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const void *Decoder) {
   if (RegNo > sizeof(GPRDecoderTable)) {
     return MCDisassembler::Fail;
   }

   // We must define our own mapping from RegNo to register identifier.
   // Accessing index RegNo in the register class will work in the case that
   // registers were added in ascending order, but not in general.
   unsigned Reg = GPRDecoderTable[RegNo];
   Inst.addOperand(MCOperand::createReg(Reg));
   return MCDisassembler::Success;
}

#include "LEGGenDisassemblerTables.inc"

DecodeStatus LEGDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &OS,
                                               raw_ostream &CS) const {
  // TODO: although assuming 4-byte instructions is sufficient for RV32 and
  // RV64, this will need modification when supporting the compressed
  // instruction set extension (RVC) which uses 16-bit instructions. Other
  // instruction set extensions have the option of defining instructions up to
  // 176 bits wide.
  Size = 4;
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  // Get the four bytes of the instruction.
  uint32_t Inst = support::endian::read32le(Bytes.data());

  return decodeInstruction(DecoderTable32, MI, Inst, Address, this, STI);
}

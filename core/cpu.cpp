#include "cpu.hpp"

#include <iostream>

#include "instructions.hpp"

namespace rivoss {
    namespace cpu {

        void Cpu::cycle() {
            this->registerFile[instr::X0] = 0;
            bool shouldIncrementPc        = true;

            uint32_t currentInstruction = this->memory[this->pc] | (this->memory[this->pc + 1] << 8) |
                                          (this->memory[this->pc + 2] << 16) |
                                          (this->memory[this->pc + 3] << 24);

            instr::DecodedInstruction decodedInstr = instr::decodeInstruction(currentInstruction);

            uint32_t rd        = decodedInstr.rd;
            uint32_t rs1       = decodedInstr.rs1;
            uint32_t rs2       = decodedInstr.rs2;
            uint32_t imm       = decodedInstr.imm;
            int32_t  signedImm = decodedInstr.signedImm;

            switch (decodedInstr.instr) {

            // Basic Arithmetic
            case instr::Instruction::ADD: {
                this->registerFile[rd] = this->registerFile[rs1] + this->registerFile[rs2];
                break;
            }
            case instr::Instruction::SUB: {
                this->registerFile[rd] = this->registerFile[rs1] - this->registerFile[rs2];
                break;
            }
            case instr::Instruction::XOR: {
                this->registerFile[rd] = this->registerFile[rs1] ^ this->registerFile[rs2];
                break;
            }
            case instr::Instruction::OR: {
                this->registerFile[rd] = this->registerFile[rs1] | this->registerFile[rs2];
                break;
            }
            case instr::Instruction::AND: {
                this->registerFile[rd] = this->registerFile[rs1] & this->registerFile[rs2];
                break;
            }
            case instr::Instruction::SLL: {
                uint32_t shiftAmount   = this->registerFile[rs2] & 0b11111u;
                this->registerFile[rd] = this->registerFile[rs1] << shiftAmount;
                break;
            }
            case instr::Instruction::SRL: {
                uint32_t shiftAmount   = this->registerFile[rs2] & 0b11111u;
                this->registerFile[rd] = this->registerFile[rs1] >> shiftAmount;
                break;
            }
            case instr::Instruction::SRA: {
                uint32_t shiftAmount = this->registerFile[rs2] & 0b11111u;
                if (shiftAmount == 0) {
                    this->registerFile[rd] = this->registerFile[rs1];
                } else {
                    this->registerFile[rd] =
                        (~0u << (32 - shiftAmount)) | (this->registerFile[rs1] >> shiftAmount);
                }
                break;
            }
            case instr::Instruction::SLT: {
                int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b              = static_cast<int32_t>(this->registerFile[rs2]);
                this->registerFile[rd] = a < b ? 1u : 0u;
                break;
            }
            case instr::Instruction::SLTU: {
                this->registerFile[rd] = this->registerFile[rs1] < this->registerFile[rs2] ? 1u : 0u;
                break;
            }

            // Immediate Arithmetic
            case instr::Instruction::ADDI: {
                this->registerFile[rd] = this->registerFile[rs1] + imm;
                break;
            }
            case instr::Instruction::XORI: {
                this->registerFile[rd] = this->registerFile[rs1] ^ imm;
                break;
            }
            case instr::Instruction::ORI: {
                this->registerFile[rd] = this->registerFile[rs1] | imm;
                break;
            }
            case instr::Instruction::ANDI: {
                this->registerFile[rd] = this->registerFile[rs1] & imm;
                break;
            }
            case instr::Instruction::SLLI: {
                this->registerFile[rd] = this->registerFile[rs1] << (imm & 0b11111u);
                break;
            }
            case instr::Instruction::SRLI: {
                uint32_t shiftAmount   = imm & 0b11111u;
                this->registerFile[rd] = this->registerFile[rs1] >> shiftAmount;
                break;
            }
            case instr::Instruction::SRAI: {
                uint32_t shiftAmount = imm & 0b11111u;
                if (shiftAmount == 0) {
                    this->registerFile[rd] = this->registerFile[rs1];
                } else {
                    this->registerFile[rd] =
                        (~0u << (32 - shiftAmount)) | (this->registerFile[rs1] >> shiftAmount);
                }
                break;
            }
            case instr::Instruction::SLTI: {
                int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b              = signedImm;
                this->registerFile[rd] = a < b ? 1u : 0u;
                break;
            }
            case instr::Instruction::SLTIU: {
                this->registerFile[rd] = this->registerFile[rs1] < imm ? 1u : 0u;
                break;
            }

            // Loading
            // The nested casts are used here for sign extension
            case instr::Instruction::LB: {
                uint32_t baseAddress   = this->registerFile[rs1] + imm;
                this->registerFile[rd] = static_cast<uint32_t>(
                    static_cast<int32_t>(static_cast<int8_t>(this->memory[baseAddress])));
                break;
            }
            case instr::Instruction::LH: {
                uint32_t baseAddress   = this->registerFile[rs1] + imm;
                uint16_t halfword      = static_cast<uint16_t>(this->memory[baseAddress]) |
                                         (static_cast<uint16_t>(this->memory[baseAddress + 1]) << 8);
                this->registerFile[rd] = static_cast<uint32_t>(static_cast<int16_t>(halfword));
                break;
            }
            case instr::Instruction::LW: {
                uint32_t baseAddress   = this->registerFile[rs1] + imm;
                this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]) |
                                         (static_cast<uint32_t>(this->memory[baseAddress + 1]) << 8) |
                                         (static_cast<uint32_t>(this->memory[baseAddress + 2]) << 16) |
                                         (static_cast<uint32_t>(this->memory[baseAddress + 3]) << 24);
                break;
            }
            case instr::Instruction::LBU: {
                uint32_t baseAddress   = this->registerFile[rs1] + imm;
                this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]);
                break;
            }
            case instr::Instruction::LHU: {
                uint32_t baseAddress   = this->registerFile[rs1] + imm;
                this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]) |
                                         (static_cast<uint32_t>(this->memory[baseAddress + 1] << 8));
                break;
            }

            // storigng
            case instr::Instruction::SB: {
                uint32_t baseAddress      = this->registerFile[rs1] + imm;
                this->memory[baseAddress] = static_cast<uint8_t>(this->registerFile[rs2]);
                break;
            }
            case instr::Instruction::SH: {
                uint32_t baseAddress          = this->registerFile[rs1] + imm;
                this->memory[baseAddress]     = static_cast<uint8_t>(this->registerFile[rs2]);
                this->memory[baseAddress + 1] = static_cast<uint8_t>(this->registerFile[rs2] >> 8);
                break;
            }
            case instr::Instruction::SW: {
                uint32_t baseAddress          = this->registerFile[rs1] + imm;
                this->memory[baseAddress]     = static_cast<uint8_t>(this->registerFile[rs2]);
                this->memory[baseAddress + 1] = static_cast<uint8_t>(this->registerFile[rs2] >> 8);
                this->memory[baseAddress + 2] = static_cast<uint8_t>(this->registerFile[rs2] >> 16);
                this->memory[baseAddress + 3] = static_cast<uint8_t>(this->registerFile[rs2] >> 24);
                break;
            }

            // branching
            case instr::Instruction::BEQ: {
                if (this->registerFile[rs1] == this->registerFile[rs2]) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }
            case instr::Instruction::BNE: {
                if (this->registerFile[rs1] != this->registerFile[rs2]) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }
            case instr::Instruction::BLT: {
                int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b = static_cast<int32_t>(this->registerFile[rs2]);
                if (a < b) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }
            case instr::Instruction::BGE: {
                int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b = static_cast<int32_t>(this->registerFile[rs2]);
                if (a >= b) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }
            case instr::Instruction::BLTU: {
                if (this->registerFile[rs1] < this->registerFile[rs2]) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }
            case instr::Instruction::BGEU: {
                if (this->registerFile[rs1] >= this->registerFile[rs2]) {
                    shouldIncrementPc = false;
                    this->pc += imm;
                }
                break;
            }

            // Jumping
            case instr::Instruction::JAL: {
                shouldIncrementPc      = false;
                this->registerFile[rd] = this->pc + 4;
                this->pc += imm;
                break;
            }
            case instr::Instruction::JALR: {
                shouldIncrementPc      = false;
                this->registerFile[rd] = this->pc + 4;
                this->pc               = this->registerFile[rs1] + imm;
                break;
            }

            // More loading
            case instr::Instruction::LUI: {
                this->registerFile[rd] = imm;
                break;
            }
            case instr::Instruction::AUIPC: {
                this->registerFile[rd] = this->pc + imm;
                break;
            }

            // System
            case instr::Instruction::ECALL: {
                // TODO: ecall
                std::cout << "ECALL" << std::endl;
                break;
            }
            case instr::Instruction::EBREAK: {
                // TODO: ebreak
                std::cout << "EBREAK" << std::endl;
                break;
            }

            // Multiplication extension
            case instr::Instruction::MUL: {
                int64_t a              = static_cast<int64_t>(this->registerFile[rs1]);
                int64_t b              = static_cast<int64_t>(this->registerFile[rs2]);
                this->registerFile[rd] = static_cast<uint32_t>(a * b);
                break;
            }
            case instr::Instruction::MULH: {
                int64_t a              = static_cast<int64_t>(this->registerFile[rs1]);
                int64_t b              = static_cast<int64_t>(this->registerFile[rs2]);
                this->registerFile[rd] = static_cast<uint32_t>((a * b) >> 32);
                break;
            }
            case instr::Instruction::MULSU: {
                int64_t  widenedSignedA = static_cast<int64_t>(static_cast<int32_t>(this->registerFile[rs1]));
                uint64_t widenedUnsignedB = static_cast<uint64_t>(this->registerFile[rs2]);

                int64_t result         = widenedSignedA * static_cast<int64_t>(widenedUnsignedB);
                this->registerFile[rd] = static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32);
                break;
            }
            case instr::Instruction::MULU: {
                uint64_t a             = static_cast<uint64_t>(this->registerFile[rs1]);
                uint64_t b             = static_cast<uint64_t>(this->registerFile[rs2]);
                this->registerFile[rd] = static_cast<uint32_t>((a * b) >> 32);
                break;
            }
            case instr::Instruction::DIV: {
                int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b              = static_cast<int32_t>(this->registerFile[rs2]);
                this->registerFile[rd] = static_cast<uint32_t>(a / b);
                break;
            }
            case instr::Instruction::DIVU: {
                this->registerFile[rd] = this->registerFile[rs1] / this->registerFile[rs2];
                break;
            }
            case instr::Instruction::REM: {
                int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                int32_t b              = static_cast<int32_t>(this->registerFile[rs2]);
                this->registerFile[rd] = static_cast<uint32_t>(a % b);
                break;
            }
            case instr::Instruction::REMU: {
                this->registerFile[rd] = this->registerFile[rs1] % this->registerFile[rs2];
                break;
            }

            default:
                // Handle an exception here
                // See https://docs.riscv.org/reference/isa/v20260120/unpriv/intro.html#trap-defn
                break;
            }

            if (shouldIncrementPc) {
                this->pc += 4;
            }
        }

    } // namespace cpu

} // namespace rivoss

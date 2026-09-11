#include "cpu.hpp"

#include <iostream>

#include "utils.hpp"

namespace rivoss {
    namespace cpu {

        void Cpu::cycle() {
            this->registerFile[x0] = 0;
            bool shouldIncrementPc = true;

            uint32_t currentInstruction = this->memory[this->pc] | (this->memory[this->pc + 1] << 8) |
                                          (this->memory[this->pc + 2] << 16) |
                                          (this->memory[this->pc + 3] << 24);

            // TODO: restructure this to decode using a separate method which returns a struct containing the
            // decoded values. E.g. rs1, rs2, funct3 etc. Purpose being instructions can be easily consumed by
            // both the CPU and the UI.
            // TODO: add errors for invalid decodes

            // Instruction encodings per
            // https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html#1-1-2-base-instruction-formats,
            // Ordered using this table
            // https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf.

            uint32_t opcode = utils::extractBits(currentInstruction, 0, 7);

            switch (opcode) {
            case 0b0110011u: {
                // R Type
                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t funct7 = utils::extractBits(currentInstruction, 25, 7);
                uint32_t rd     = utils::extractBits(currentInstruction, 7, 5);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                uint32_t rs2    = utils::extractBits(currentInstruction, 20, 5);

                if (funct7 == 0x01) {
                    // RV32M (Multiply Extension)

                    switch (funct3) {
                    case 0x0: {
                        // MUL
                        int64_t a = static_cast<int64_t>(this->registerFile[rs1]);
                        int64_t b = static_cast<int64_t>(this->registerFile[rs2]);

                        this->registerFile[rd] = static_cast<uint32_t>(a * b);
                        break;
                    }
                    case 0x1: {
                        // MUL High
                        int64_t a = static_cast<int64_t>(this->registerFile[rs1]);
                        int64_t b = static_cast<int64_t>(this->registerFile[rs2]);

                        this->registerFile[rd] = static_cast<uint32_t>((a * b) >> 32);
                        break;
                    }
                    case 0x2: {
                        // Mul High Signed Unsigned
                        int64_t widenedSignedA =
                            static_cast<int64_t>(static_cast<int32_t>(this->registerFile[rs1]));
                        uint64_t widenedUnsignedB = static_cast<uint64_t>(this->registerFile[rs2]);

                        int64_t result         = widenedSignedA * static_cast<int64_t>(widenedUnsignedB);
                        this->registerFile[rd] = static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32);
                        break;
                    }
                    case 0x3: {
                        uint64_t a = static_cast<uint64_t>(this->registerFile[rs1]);
                        uint64_t b = static_cast<uint64_t>(this->registerFile[rs2]);

                        this->registerFile[rd] = static_cast<uint32_t>((a * b) >> 32);
                        break;
                    }
                    case 0x4: {
                        // DIV
                        int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                        int32_t b = static_cast<int32_t>(this->registerFile[rs2]);

                        this->registerFile[rd] = static_cast<uint32_t>(a / b);
                        break;
                    }
                    case 0x5: {
                        // DIV Unsigned
                        this->registerFile[rd] = this->registerFile[rs1] / this->registerFile[rs2];
                        break;
                    }
                    case 0x6: {
                        // Remainder
                        int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                        int32_t b = static_cast<int32_t>(this->registerFile[rs2]);

                        this->registerFile[rd] = static_cast<uint32_t>(a % b);
                        break;
                    }
                    case 0x7: {
                        // Remainder Unsigned
                        this->registerFile[rd] = this->registerFile[rs1] % this->registerFile[rs2];
                        break;
                    }

                    default:
                        break;
                    }

                } else {
                    // RV32I Instructions
                    switch (funct3) {
                    case 0x0: { // SUB & ADD
                        if (funct7 == 0x20) {
                            this->registerFile[rd] = this->registerFile[rs1] - this->registerFile[rs2];
                        } else {
                            this->registerFile[rd] = this->registerFile[rs1] + this->registerFile[rs2];
                        }
                        break;
                    }
                    case 0x4: { // XOR
                        this->registerFile[rd] = this->registerFile[rs1] ^ this->registerFile[rs2];
                        break;
                    }
                    case 0x6: { // OR
                        this->registerFile[rd] = this->registerFile[rs1] | this->registerFile[rs2];
                        break;
                    }
                    case 0x7: { // AND
                        this->registerFile[rd] = this->registerFile[rs1] & this->registerFile[rs2];
                        break;
                    }
                    case 0x1: { // Shift Left Logical
                        uint32_t shiftAmount   = this->registerFile[rs2] & 0b11111u;
                        this->registerFile[rd] = this->registerFile[rs1] << shiftAmount;
                        break;
                    }
                    case 0x5: {
                        uint32_t shiftAmount = this->registerFile[rs2] & 0b11111u;

                        // Shift Right Logical & Shift Right Arithmetic
                        if (funct7 == 0x00) {
                            this->registerFile[rd] = this->registerFile[rs1] >> shiftAmount;
                        } else {
                            if (shiftAmount == 0) {
                                this->registerFile[rd] = this->registerFile[rs1];
                            } else {
                                this->registerFile[rd] =
                                    (~0u << (32 - shiftAmount)) | (this->registerFile[rs1] >> shiftAmount);
                            }
                        }
                        break;
                    }
                    case 0x2: {
                        // Set Less Than
                        int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                        int32_t b              = static_cast<int32_t>(this->registerFile[rs2]);
                        this->registerFile[rd] = a < b ? 1u : 0u;
                        break;
                    }
                    case 0x3: {
                        // Set Less Than Unsigned
                        this->registerFile[rd] = this->registerFile[rs1] < this->registerFile[rs2] ? 1u : 0u;
                        break;
                    }

                    default:
                        break;
                    }
                }

                break;
            }
            case 0b0010011u: {
                // I Type
                uint32_t rd     = utils::extractBits(currentInstruction, 7, 5);
                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                int32_t  signedImm =
                    static_cast<int32_t>(utils::extractBits(currentInstruction, 20, 12) << 20) >> 20;
                uint32_t imm = static_cast<uint32_t>(signedImm);

                switch (funct3) {
                case 0x0: {
                    // Add Immediate
                    this->registerFile[rd] = this->registerFile[rs1] + imm;
                    break;
                }
                case 0x4: {
                    // XOR immediate
                    this->registerFile[rd] = this->registerFile[rs1] ^ imm;
                    break;
                }
                case 0x6: {
                    // OR Immediate
                    this->registerFile[rd] = this->registerFile[rs1] | imm;
                    break;
                }
                case 0x7: {
                    // AND Immediate
                    this->registerFile[rd] = this->registerFile[rs1] & imm;
                    break;
                }
                case 0x1: {
                    // Shift Left Logical Immediate
                    this->registerFile[rd] = this->registerFile[rs1] << (imm & 0b11111u);
                }
                case 0x5: {
                    // Shift Right Logical & Arithmetic
                    // Same function as funct7
                    uint32_t imm5to11    = utils::extractBits(imm, 5, 6);
                    uint32_t shiftAmount = imm & 0b11111u;

                    if (imm5to11 == 0x00) {
                        this->registerFile[rd] = this->registerFile[rs1] >> shiftAmount;
                    } else {
                        if (shiftAmount == 0) {
                            this->registerFile[rd] = this->registerFile[rs1];
                        } else {
                            this->registerFile[rd] =
                                (~0u << (32 - shiftAmount)) | (this->registerFile[rs1] >> shiftAmount);
                        }
                    }
                    break;
                }
                case 0x2:
                    // Set Less Than
                    int32_t a              = static_cast<int32_t>(this->registerFile[rs1]);
                    int32_t b              = static_cast<int32_t>(imm);
                    this->registerFile[rd] = a < b ? 1u : 0u;
                    break;
                case 0x3: {
                    // Set Less Than Imm Unsigned
                    this->registerFile[rd] = this->registerFile[rs1] < imm ? 1u : 0u;
                    break;
                }

                default:
                    break;
                }

                break;
            }
            case 0b0000011u: {
                // I Type
                uint32_t rd     = utils::extractBits(currentInstruction, 7, 5);
                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                int32_t  imm =
                    static_cast<int32_t>(utils::extractBits(currentInstruction, 20, 12) << 20) >> 20;
                uint32_t baseAddress = this->registerFile[rs1] + static_cast<uint32_t>(imm);

                // TODO: add access checks for these

                // The nested casts are used here for sign extension
                switch (funct3) {
                case 0x0: {
                    // Load Byte
                    this->registerFile[rd] = static_cast<uint32_t>(
                        static_cast<int32_t>(static_cast<int8_t>(this->memory[baseAddress])));
                    break;
                }
                case 0x1: {
                    // Load Half
                    uint16_t halfword      = static_cast<uint16_t>(this->memory[baseAddress]) |
                                             (static_cast<uint16_t>(this->memory[baseAddress + 1]) << 8);
                    this->registerFile[rd] = static_cast<uint32_t>(static_cast<int16_t>(halfword));
                    break;
                }
                case 0x2: {
                    // Load Word
                    this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]) |
                                             (static_cast<uint32_t>(this->memory[baseAddress + 1]) << 8) |
                                             (static_cast<uint32_t>(this->memory[baseAddress + 2]) << 16) |
                                             (static_cast<uint32_t>(this->memory[baseAddress + 3]) << 24);
                    break;
                }
                case 0x4: {
                    // Load Byte Unsigned
                    this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]);
                    break;
                }
                case 0x5: {
                    // Load Word Unsigned
                    this->registerFile[rd] = static_cast<uint32_t>(this->memory[baseAddress]) |
                                             (static_cast<uint32_t>(this->memory[baseAddress + 1] << 8));
                    break;
                }

                default:
                    break;
                }
                break;
            }
            case 0b0100011u: {
                // S Type
                // For more detail about the split immediate, see: https://stackoverflow.com/a/58417989
                int32_t signedImm =
                    static_cast<int32_t>((utils::extractBits(currentInstruction, 25, 7) << 5) |
                                         utils::extractBits(currentInstruction, 7, 5));
                signedImm    = (signedImm << 20) >> 20;
                uint32_t imm = static_cast<uint32_t>(signedImm);

                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                uint32_t rs2    = utils::extractBits(currentInstruction, 20, 5);

                uint32_t baseAddress = rs1 + imm;

                // TODO: also add access checks for these

                switch (funct3) {
                case 0x0: {
                    // Store Byte
                    this->memory[baseAddress] = static_cast<uint8_t>(this->registerFile[rs2]);
                    break;
                }
                case 0x1: {
                    // Store Half
                    this->memory[baseAddress]     = static_cast<uint8_t>(this->registerFile[rs2]);
                    this->memory[baseAddress + 1] = static_cast<uint8_t>(this->registerFile[rs2] >> 8);
                    break;
                }
                case 0x2: {
                    // Store Word
                    this->memory[baseAddress]     = static_cast<uint64_t>(this->registerFile[rs2]);
                    this->memory[baseAddress + 1] = static_cast<uint64_t>(this->registerFile[rs2] >> 8);
                    this->memory[baseAddress + 2] = static_cast<uint64_t>(this->registerFile[rs2] >> 16);
                    this->memory[baseAddress + 3] = static_cast<uint64_t>(this->registerFile[rs2] >> 24);
                    break;
                }
                default:
                    break;
                }

                break;
            }
            case 0b1100011u: {
                // B Type
                // Here the first bit of the immediate value is ignored because instructions are always
                // aligned to an even memory addresss.

                uint32_t immediateBit11     = ((currentInstruction & (1u << 7)) >> 7) << 11;
                uint32_t immediateBits4to1  = utils::extractBits(currentInstruction, 8, 4) << 1;
                uint32_t immediateBits10to5 = utils::extractBits(currentInstruction, 25, 6);
                uint32_t immediateBit12     = ((currentInstruction & (1u << 31)) >> 31) << 12;

                int32_t signedImm = static_cast<int32_t>(immediateBits4to1 | immediateBits10to5 |
                                                         immediateBit11 | immediateBit12);
                signedImm         = (signedImm << 19) >> 19;
                uint32_t imm      = static_cast<uint32_t>(signedImm);

                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                uint32_t rs2    = utils::extractBits(currentInstruction, 20, 5);

                switch (funct3) {
                case 0x0: {
                    // Branch ==
                    if (this->registerFile[rs1] == this->registerFile[rs2]) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }
                case 0x1: {
                    // Branch !=
                    if (this->registerFile[rs1] != this->registerFile[rs2]) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }
                case 0x4: {
                    // Branch < Signed
                    int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                    int32_t b = static_cast<int32_t>(this->registerFile[rs2]);
                    if (a < b) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }
                case 0x5: {
                    // Branch >= Signed
                    int32_t a = static_cast<int32_t>(this->registerFile[rs1]);
                    int32_t b = static_cast<int32_t>(this->registerFile[rs2]);
                    if (a >= b) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }
                case 0x6: {
                    // Branch < Unsigned
                    if (this->registerFile[rs1] < this->registerFile[rs2]) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }
                case 0x7: {
                    // Branch >= Unsigned
                    if (this->registerFile[rs1] >= this->registerFile[rs2]) {
                        shouldIncrementPc = false;
                        this->pc += imm;
                    }
                    break;
                }

                default:
                    break;
                }

                break;
            }
            case 0b1101111u: {
                // Jump and Link
                uint32_t rd = utils::extractBits(currentInstruction, 7, 5);

                uint32_t immediateBits19t012 = utils::extractBits(currentInstruction, 12, 8) << 12;
                uint32_t immediateBit11      = ((currentInstruction & (1u << 20)) >> 20) << 11;
                uint32_t immediateBits10to1  = utils::extractBits(currentInstruction, 21, 10) << 1;
                uint32_t immediateBit20      = ((currentInstruction & (1u << 31)) >> 31) << 20;

                int32_t signedImm = static_cast<int32_t>(immediateBits10to1 | immediateBit11 |
                                                         immediateBits19t012 | immediateBit20);
                signedImm         = (signedImm << 11) >> 11;
                uint32_t imm      = static_cast<uint32_t>(signedImm);

                shouldIncrementPc = false;

                this->registerFile[rd] = this->pc + 4;
                this->pc += imm;

                break;
            }
            case 0b1100111u: {
                // Jump And Link Reg
                uint32_t rd     = utils::extractBits(currentInstruction, 7, 5);
                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                int32_t  signedImm =
                    static_cast<int32_t>(utils::extractBits(currentInstruction, 20, 12) << 20) >> 20;
                uint32_t imm = static_cast<uint32_t>(signedImm);

                shouldIncrementPc = false;

                this->registerFile[rd] = this->pc + 4;
                this->pc               = this->registerFile[rs1] + imm;

                break;
            }
            case 0b0110111u:
            case 0b0010111u: {
                // Load Upper Immediate & Add Upper Immediate to PC
                uint32_t rd = utils::extractBits(currentInstruction, 7, 5);

                int32_t signedImm =
                    static_cast<int32_t>(utils::extractBits(currentInstruction, 12, 20) << 12);
                uint32_t imm = static_cast<uint32_t>(signedImm);

                break;
            }
            case 0b1110011u: {
                // Environment Call & Environment Break
                uint32_t rd     = utils::extractBits(currentInstruction, 7, 5);
                uint32_t funct3 = utils::extractBits(currentInstruction, 12, 3);
                uint32_t rs1    = utils::extractBits(currentInstruction, 15, 5);
                int32_t  signedImm =
                    static_cast<int32_t>(utils::extractBits(currentInstruction, 20, 12) << 20) >> 20;
                uint32_t imm = static_cast<uint32_t>(signedImm);

                if (imm == 0x0) {
                    // TODO: ecall
                    std::cout << "ECALL" << std::endl;
                } else {
                    // TODO: ebreak
                    std::cout << "EBREAK" << std::endl;
                }

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
        };

    } // namespace cpu

} // namespace rivoss

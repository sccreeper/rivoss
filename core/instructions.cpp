#include "instructions.hpp"

#include <cstdint>
#include <format>

#include "utils.hpp"

// TODO: move the binary values in switch statements to named constants

namespace rivoss {
    namespace instr {

        DecodedInstruction decodeInstruction(uint32_t instr) {

            DecodedInstruction result{};

            // Instruction encodings per
            // https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html#1-1-2-base-instruction-formats,
            // Ordered using this table
            // https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf.

            result.opcode = utils::extractBits(instr, 0, 7);

            switch (result.opcode) {
            case 0b0110011u: {
                // R Type
                result.funct3 = utils::extractBits(instr, 12, 3);
                result.funct7 = utils::extractBits(instr, 25, 7);
                result.rd     = utils::extractBits(instr, 7, 5);
                result.rs1    = utils::extractBits(instr, 15, 5);
                result.rs2    = utils::extractBits(instr, 20, 5);

                if (result.funct7 == 0x01) {
                    // RV32M (Multiply Extension)

                    switch (result.funct3) {
                    case 0x0: {
                        // MUL
                        result.instr = Instruction::MUL;
                        break;
                    }
                    case 0x1: {
                        // MUL High
                        result.instr = Instruction::MULH;
                        break;
                    }
                    case 0x2: {
                        // Mul High Signed Unsigned
                        result.instr = Instruction::MULSU;
                        break;
                    }
                    case 0x3: {
                        // MUL High Unsigned
                        result.instr = Instruction::MULU;
                        break;
                    }
                    case 0x4: {
                        // DIV
                        result.instr = Instruction::DIV;
                        break;
                    }
                    case 0x5: {
                        // DIV Unsigned
                        result.instr = Instruction::DIVU;
                        break;
                    }
                    case 0x6: {
                        // Remainder
                        result.instr = Instruction::REM;
                        break;
                    }
                    case 0x7: {
                        // Remainder Unsigned
                        result.instr = Instruction::REMU;
                        break;
                    }

                    default:
                        break;
                    }

                } else {
                    // RV32I Instructions
                    switch (result.funct3) {
                    case 0x0: { // SUB & ADD
                        if (result.funct7 == 0x20) {
                            result.instr = Instruction::SUB;
                        } else {
                            result.instr = Instruction::ADD;
                        }
                        break;
                    }
                    case 0x4: { // XOR
                        result.instr = Instruction::XOR;
                        break;
                    }
                    case 0x6: { // OR
                        result.instr = Instruction::OR;
                        break;
                    }
                    case 0x7: { // AND
                        result.instr = Instruction::AND;
                        break;
                    }
                    case 0x1: { // Shift Left Logical
                        result.instr = Instruction::SLL;
                        break;
                    }
                    case 0x5: {

                        // Shift Right Logical & Shift Right Arithmetic
                        if (result.funct7 == 0x00) {
                            result.instr = Instruction::SRL;
                        } else {
                            result.instr = Instruction::SRA;
                        }
                        break;
                    }
                    case 0x2: {
                        // Set Less Than
                        result.instr = Instruction::SLT;
                        break;
                    }
                    case 0x3: {
                        // Set Less Than Unsigned
                        result.instr = Instruction::SLTU;
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
                result.rd        = utils::extractBits(instr, 7, 5);
                result.funct3    = utils::extractBits(instr, 12, 3);
                result.rs1       = utils::extractBits(instr, 15, 5);
                result.signedImm = static_cast<int32_t>(utils::extractBits(instr, 20, 12) << 20) >> 20;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                switch (result.funct3) {
                case 0x0: {
                    // Add Immediate
                    result.instr = Instruction::ADDI;
                    break;
                }
                case 0x4: {
                    // XOR immediate
                    result.instr = Instruction::XORI;
                    break;
                }
                case 0x6: {
                    // OR Immediate
                    result.instr = Instruction::ORI;
                    break;
                }
                case 0x7: {
                    // AND Immediate
                    result.instr = Instruction::ANDI;
                    break;
                }
                case 0x1: {
                    // Shift Left Logical Immediate
                    result.instr = Instruction::SLLI;
                    break;
                }
                case 0x5: {
                    // Shift Right Logical & Arithmetic
                    // Same function as funct7
                    uint32_t imm5to11 = utils::extractBits(result.imm, 5, 6);

                    if (imm5to11 == 0x00) {
                        result.instr = Instruction::SRLI;
                    } else {
                        result.instr = Instruction::SRAI;
                    }
                    break;
                }
                case 0x2:
                    // Set Less Than
                    result.instr = Instruction::SLTI;
                    break;
                case 0x3: {
                    // Set Less Than Imm Unsigned
                    result.instr = Instruction::SLTIU;
                    break;
                }

                default:
                    break;
                }

                break;
            }
            case 0b0000011u: {
                // I Type
                result.rd     = utils::extractBits(instr, 7, 5);
                result.funct3 = utils::extractBits(instr, 12, 3);
                result.rs1    = utils::extractBits(instr, 15, 5);
                result.imm    = static_cast<int32_t>(utils::extractBits(instr, 20, 12) << 20) >> 20;

                // The nested casts are used here for sign extension
                switch (result.funct3) {
                case 0x0: {
                    // Load Byte
                    result.instr = Instruction::LB;
                    break;
                }
                case 0x1: {
                    // Load Half
                    result.instr = Instruction::LH;
                    break;
                }
                case 0x2: {
                    // Load Word
                    result.instr = Instruction::LW;
                    break;
                }
                case 0x4: {
                    // Load Byte Unsigned
                    result.instr = Instruction::LBU;
                    break;
                }
                case 0x5: {
                    // Load Half Unsigned
                    result.instr = Instruction::LHU;
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
                result.signedImm = static_cast<int32_t>((utils::extractBits(instr, 25, 7) << 5) |
                                                        utils::extractBits(instr, 7, 5));
                result.signedImm = (result.signedImm << 20) >> 20;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                result.funct3 = utils::extractBits(instr, 12, 3);
                result.rs1    = utils::extractBits(instr, 15, 5);
                result.rs2    = utils::extractBits(instr, 20, 5);

                // TODO: also add access checks for these

                switch (result.funct3) {
                case 0x0: {
                    // Store Byte
                    result.instr = Instruction::SB;
                    break;
                }
                case 0x1: {
                    // Store Half
                    result.instr = Instruction::SH;
                    break;
                }
                case 0x2: {
                    // Store Word
                    result.instr = Instruction::SW;
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

                uint32_t immediateBit11     = ((instr & (1u << 7)) >> 7) << 11;
                uint32_t immediateBits4to1  = utils::extractBits(instr, 8, 4) << 1;
                uint32_t immediateBits10to5 = utils::extractBits(instr, 25, 6);
                uint32_t immediateBit12     = ((instr & (1u << 31)) >> 31) << 12;

                result.signedImm = static_cast<int32_t>(immediateBits4to1 | immediateBits10to5 |
                                                        immediateBit11 | immediateBit12);
                result.signedImm = (result.signedImm << 19) >> 19;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                result.funct3 = utils::extractBits(instr, 12, 3);
                result.rs1    = utils::extractBits(instr, 15, 5);
                result.rs2    = utils::extractBits(instr, 20, 5);

                switch (result.funct3) {
                case 0x0: {
                    // Branch ==
                    result.instr = Instruction::BEQ;
                    break;
                }
                case 0x1: {
                    // Branch !=
                    result.instr = Instruction::BNE;
                    break;
                }
                case 0x4: {
                    // Branch < Signed
                    result.instr = Instruction::BLT;
                    break;
                }
                case 0x5: {
                    // Branch >= Signed
                    result.instr = Instruction::BGE;
                    break;
                }
                case 0x6: {
                    // Branch < Unsigned
                    result.instr = Instruction::BLTU;
                    break;
                }
                case 0x7: {
                    // Branch >= Unsigned
                    result.instr = Instruction::BGEU;
                    break;
                }

                default:
                    break;
                }

                break;
            }
            case 0b1101111u: {
                // Jump and Link
                result.rd = utils::extractBits(instr, 7, 5);

                uint32_t immediateBits19t012 = utils::extractBits(instr, 12, 8) << 12;
                uint32_t immediateBit11      = ((instr & (1u << 20)) >> 20) << 11;
                uint32_t immediateBits10to1  = utils::extractBits(instr, 21, 10) << 1;
                uint32_t immediateBit20      = ((instr & (1u << 31)) >> 31) << 20;

                result.signedImm = static_cast<int32_t>(immediateBits10to1 | immediateBit11 |
                                                        immediateBits19t012 | immediateBit20);
                result.signedImm = (result.signedImm << 11) >> 11;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                result.instr = Instruction::JAL;

                break;
            }
            case 0b1100111u: {
                // Jump And Link Reg
                result.rd        = utils::extractBits(instr, 7, 5);
                result.funct3    = utils::extractBits(instr, 12, 3);
                result.rs1       = utils::extractBits(instr, 15, 5);
                result.signedImm = static_cast<int32_t>(utils::extractBits(instr, 20, 12) << 20) >> 20;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                result.instr = Instruction::JALR;

                break;
            }
            case 0b0110111u:
            case 0b0010111u: {
                // Load Upper Immediate & Add Upper Immediate to PC
                result.rd = utils::extractBits(instr, 7, 5);

                result.signedImm = static_cast<int32_t>(utils::extractBits(instr, 12, 20) << 12);
                result.imm       = static_cast<uint32_t>(result.signedImm);

                if (result.opcode == 0b0110111u) {
                    result.instr = Instruction::LUI;
                } else {
                    result.instr = Instruction::AUIPC;
                }

                break;
            }
            case 0b1110011u: {
                // Environment Call & Environment Break
                result.rd        = utils::extractBits(instr, 7, 5);
                result.funct3    = utils::extractBits(instr, 12, 3);
                result.rs1       = utils::extractBits(instr, 15, 5);
                result.signedImm = static_cast<int32_t>(utils::extractBits(instr, 20, 12) << 20) >> 20;
                result.imm       = static_cast<uint32_t>(result.signedImm);

                if (result.imm == 0x0) {
                    // TODO: ecall
                    result.instr = Instruction::ECALL;
                } else {
                    // TODO: ebreak
                    result.instr = Instruction::EBREAK;
                }

                break;
            }

            default:
                // Handle an exception here
                // See https://docs.riscv.org/reference/isa/v20260120/unpriv/intro.html#trap-defn
                break;
            }

            return result;
        }

        constexpr std::string_view getInstructionName(Instruction instr) {
            switch (instr) {
            case ADD:
                return "ADD";
            case SUB:
                return "SUB";
            case XOR:
                return "XOR";
            case OR:
                return "OR";
            case AND:
                return "AND";
            case SLL:
                return "SLL";
            case SRL:
                return "SRL";
            case SRA:
                return "SRA";
            case SLT:
                return "SLT";
            case SLTU:
                return "SLTU";
            case ADDI:
                return "ADDI";
            case XORI:
                return "XORI";
            case ORI:
                return "ORI";
            case ANDI:
                return "ANDI";
            case SLLI:
                return "SLLI";
            case SRLI:
                return "SRLI";
            case SRAI:
                return "SRAI";
            case SLTI:
                return "SLTI";
            case SLTIU:
                return "SLTIU";
            case LB:
                return "LB";
            case LH:
                return "LH";
            case LW:
                return "LW";
            case LBU:
                return "LBU";
            case LHU:
                return "LHU";
            case SB:
                return "SB";
            case SH:
                return "SH";
            case SW:
                return "SW";
            case BEQ:
                return "BEQ";
            case BNE:
                return "BNE";
            case BLT:
                return "BLT";
            case BGE:
                return "BGE";
            case BLTU:
                return "BLTU";
            case BGEU:
                return "BGEU";
            case JAL:
                return "JAL";
            case JALR:
                return "JALR";
            case LUI:
                return "LUI";
            case AUIPC:
                return "AUIPC";
            case ECALL:
                return "ECALL";
            case EBREAK:
                return "EBREAK";
            case MUL:
                return "MUL";
            case MULH:
                return "MULH";
            case MULSU:
                return "MULSU";
            case MULU:
                return "MULU";
            case DIV:
                return "DIV";
            case DIVU:
                return "DIVU";
            case REM:
                return "REM";
            case REMU:
                return "REMU";
            default:
                break;
            }
        }

        constexpr std::string_view getRegisterName(Register reg) {
            switch (reg) {
            case X0:
                return "X0";
            case X1:
                return "X1";
            case X2:
                return "X2";
            case X3:
                return "X3";
            case X4:
                return "X4";
            case X5:
                return "X5";
            case X6:
                return "X6";
            case X7:
                return "X7";
            case X8:
                return "X8";
            case X9:
                return "X9";
            case X10:
                return "X10";
            case X11:
                return "X11";
            case X12:
                return "X12";
            case X13:
                return "X13";
            case X14:
                return "X14";
            case X15:
                return "X15";
            case X16:
                return "X16";
            case X17:
                return "X17";
            case X18:
                return "X18";
            case X19:
                return "X19";
            case X20:
                return "X20";
            case X21:
                return "X21";
            case X22:
                return "X22";
            case X23:
                return "X23";
            case X24:
                return "X24";
            case X25:
                return "X25";
            case X26:
                return "X26";
            case X27:
                return "X27";
            case X28:
                return "X28";
            case X29:
                return "X29";
            case X30:
                return "X30";
            case X31:
                return "X31";
            default:
                return "PC";
            }
        };

        std::string instructionToString(DecodedInstruction instr) {
            // Format differently based on the encoding type
            switch (instr.opcode) {
            case 0b0110011u:
                // R Type
                return std::format("{}, {}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), static_cast<Register>(instr.rs1),
                                   static_cast<Register>(instr.rs1));

            case 0b0010011u:
            case 0b0000011u:
            case 0b1100111u:
            case 0b1110011u:
                // I Type
                return std::format("{}, {}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), static_cast<Register>(instr.rs1),
                                   instr.imm);

            case 0b0100011u:
                // S Type
                return std::format("{}, {}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), static_cast<Register>(instr.rs1),
                                   instr.imm);

            case 0b1100011u:
                // B Type
                return std::format("{}, {}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), static_cast<Register>(instr.rs1),
                                   instr.imm);

            case 0b1101111u:
                // J Type
                return std::format("{}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), instr.imm);
            case 0b0110111u:
            case 0b0010111u:
                // U Type
                return std::format("{}, {}, {}", getInstructionName(instr.instr),
                                   static_cast<Register>(instr.rd), instr.imm);

            default:
                break;
            }
        }

    } // namespace instr

} // namespace rivoss

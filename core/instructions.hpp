#pragma once

#include <cstdint>
#include <string_view>

namespace rivoss {

    // Utilities for decoding and encoding instructions
    // Separated because it is used by both the CPU and OS
    namespace instr {

        enum Instruction {
            // Basic Arithmetic
            ADD,
            SUB,
            XOR,
            OR,
            AND,
            SLL,
            SRL,
            SRA,
            SLT,
            SLTU,

            // Immediate Arithmetic
            ADDI,
            XORI,
            ORI,
            ANDI,
            SLLI,
            SRLI,
            SRAI,
            SLTI,
            SLTIU,

            // Loading
            LB,
            LH,
            LW,
            LBU,
            LHU,

            // Storing
            SB,
            SH,
            SW,

            // Branching
            BEQ,
            BNE,
            BLT,
            BGE,
            BLTU,
            BGEU,

            // Jumping
            JAL,
            JALR,

            LUI,
            AUIPC,

            ECALL,
            EBREAK,

            // Multiplication Extension
            MUL,
            MULH,
            MULSU,
            MULU,
            DIV,
            DIVU,
            REM,
            REMU

        };

        enum Register {
            X0,
            X1,
            X2,
            X3,
            X4,
            X5,
            X6,
            X7,
            X8,
            X9,
            X10,
            X11,
            X12,
            X13,
            X14,
            X15,
            X16,
            X17,
            X18,
            X19,
            X20,
            X21,
            X22,
            X23,
            X24,
            X25,
            X26,
            X27,
            X28,
            X29,
            X30,
            X31
        };

        struct DecodedInstruction {
            instr::Instruction instr;
            uint32_t           opcode;
            uint32_t           rd;
            uint32_t           rs1;
            uint32_t           rs2;

            uint32_t funct3;
            uint32_t funct7;

            int32_t  signedImm;
            uint32_t imm;
        };

        DecodedInstruction decodeInstruction(uint32_t instr);

        constexpr std::string_view getInstructionName(Instruction instr);
        constexpr std::string_view getRegisterName(Register reg);
        std::string                instructionToString(DecodedInstruction instr);

    }; // namespace instr

}; // namespace rivoss

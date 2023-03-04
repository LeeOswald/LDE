/*
References:

* http://wiki.osdev.org/X86-64_Instruction_Encoding
* http://ref.x86asm.net/geek64.html

May contain errors...
*/

#include "../lde.h"
#include "helpers.h"

static const uint32_t TABLE_PREFIX[8] =
{
    /*0123456789ABCDEF0123456789ABCDEF*/
    0b00000000000000000000000000000000,// 0
    0b00000010000000100000001000000010,// 2
    0b11111111111111110000000000000000,// 4
    0b00001111000000000000000000000000,// 6
    0b00000000000000000000000000010000,// 8
    0b00000000000000000000000000000000,// A
    0b00000000000000000000000000000000,// C
    0b00000000000000001011000000000000,// E
};

//---- One-byte opcodes ----
static const uint32_t TABLE_INVALID_A[8] =
{
    0b00000011000000110000001100000011,// 0
    0b00000001000000010000000100000001,// 2
    0b00000000000000000000000000000000,// 4
    0b11100000000000000000000000000000,// 6
    0b00100000000000000000000000100000,// 8
    0b00000000000000000000000000000000,// A
    0b00001100000000000000111000000000,// C
    0b00000000001000000000000000000000,// E
};

static const uint32_t TABLE_MODRM_A[8] =
{
    0b11110000111100001111000011110000,// 0
    0b11110000111100001111000011110000,// 2
    0b00000000000000000000000000000000,// 4
    0b00110000010100000000000000000000,// 6
    0b11111111111111110000000000000000,// 8
    0b00000000000000000000000000000000,// A
    0b11001111000000001111000011111111,// C
    0b00000000000000000000001100000011,// E
};

static const uint32_t TABLE_IMM8_A[8] =
{
    0b00001000000010000000100000001000,// 0
    0b00001000000010000000100000001000,// 2
    0b00000000000000000000000000000000,// 4
    0b00000000001100001111111111111111,// 6
    0b10110000000000000000000000000000,// 8
    0b00000000100000001111111100000000,// A
    0b11000010100001000000110000000000,// C
    0b11111111000100000000000000000000,// E
};

static const uint32_t TABLE_IMM_A[8] =
{
    0b00000100000001000000010000000100,// 0
    0b00000100000001000000010000000100,// 2
    0b00000000000000000000000000000000,// 4
    0b00000000110000000000000000000000,// 6
    0b01000000000000000000000000100000,// 8
    0b00000000010000000000000011111111,// A
    0b00000001000000000000000000000000,// C
    0b00000000111000000000000000000000,// E
};

//---- Two-byte opcodes ----
static const uint32_t TABLE_MODRM_B[8] =
{
    0b11110000000001001111111111111111,// 0
    0b00000000111111110000000000000000,// 2
    0b11111111111111111111111111111111,// 4
    0b11111111111111111000111011111111,// 6
    0b00000000000000001111111111111111,// 8
    0b00011100000111111111111110111111,// A
    0b11111111000000001111111111111111,// C
    0b11111111111111111111111111111111,// E
};

static const uint32_t TABLE_INVALID_B[8] =
{
    0b00001000001010110000000000000000,// 0
    0b00001111000000000000001011111111,// 2
    0b00000000000000000000000000000000,// 4
    0b00000000000000000000000000110000,// 6
    0b00000000000000000000000000000000,// 8
    0b00000011000000000000000000000000,// A
    0b00000000011111110000000000000000,// C
    0b00000000000000000000000000000001,// E
};

//---- Three-byte opcodes ----
static const uint32_t TABLE_INVALID_C[2] =
{
    0b00000000000011110111001011110001,// 0
    0b00000011000011110000001000000000,// 2
};


LdeInstructionLength lde_get_instruction_length_x86_64(const void* opcode, uint32_t length)
{
    bool modrm = false;
    uint8_t op = 0;
    uint32_t ddef = 4;
    uint32_t mdef = 8;
    uint32_t dsize = 0;
    uint32_t msize = 0;
    bool rex_w = false;
    byte_iterator it = byte_iterator_init((const uint8_t*)opcode, length);
    uint32_t prefix_len = 0;

    LdeInstructionLength result = { 0, 0, 0, 0 };

    while (byte_iterator_next(&it, &op))
    {
        if (find_8(TABLE_PREFIX, op))
        {
            prefix_len += 1;

            if (op == 0x66) // Operand-size override prefix
            {
                ddef = 2;
            }
            else if (op == 0x67) // Address-size override prefix
            {
                mdef = 4;
            }
            else if (in_range(0x48, 0x50, op)) // REX prefixes with 0x8 set (W)
            { 
                rex_w = true; 
            }
        }
        else
        {
            break;
        }
    }

    if (!length)
        return result;

    uint32_t op_len = 1;

    if (op == 0x0F)
    {
        if (!byte_iterator_next(&it, &op))
            return result;

        op_len += 1;
        if (op == 0x38) // Three-byte opcodes (C)
        {
            if (!byte_iterator_next(&it, &op))
                return result;

            op_len += 1;
            // Invalid opcodes
            bool valid = false;
            if (op < 0x40)
            {
                valid = !find_2(TABLE_INVALID_C, op);
            }
            else
            {
                valid = in_range(0x40, 0x42, op) || in_range(0x80, 0x82, op) || in_range(0xF0, 0xF2, op);
            }

            if (!valid)
                return result;

            modrm = true;
        }
        else if (op == 0x3A) // Three-byte opcodes (D)
        {
            if (!byte_iterator_next(&it, &op))
                return result;

            op_len += 1;
            // Invalid opcodes
            if (!(in_range(0x08, 0x10, op) || in_range(0x14, 0x18, op) || in_range(0x20, 0x23, op) || in_range(0x40, 0x43, op) || in_range(0x60, 0x64, op)))
                return result;

            modrm = true;
            dsize += 1;
        }
        else // Two-byte opcodes (B)
        {
            // Invalid opcodes
            if (find_8(TABLE_INVALID_B, op))
                return result;

            modrm = find_8(TABLE_MODRM_B, op);
            // Check for imm8
            if (in_range(0x70, 0x74, op) || (op == 0xA4) || (op == 0xAC) || (op == 0xBA) || (op == 0xC2) || in_range(0xC4, 0xC7, op))
            {
                dsize += 1;
            }

            // Check for imm16
            if ((op & 0xF0) == 0x80)
            {
                dsize += ddef;
            }
        }
    }
    else // One-byte opcodes (A)
    {
        // Reject invalid opcodes
        if (find_8(TABLE_INVALID_A, op)) 
        {
            return result;
        }

        modrm = find_8(TABLE_MODRM_A, op);

        // Check `test` opcode with immediate
        if ((op == 0xF6 || op == 0xF7))
        {
            byte_iterator clone = it;
            if (!byte_iterator_next(&clone, &op))
                return result;

            if ((op & 0x38) == 0)
            {
                if (op & 1)
                {
                    dsize += ddef;
                }
                else
                {
                    dsize += 1;
                }
            }
        }

        // Check for imm8
        if (find_8(TABLE_IMM8_A, op))
        {
            dsize += 1;
        }
        
        // Check for imm16: RETN Iw, ENTER eBP Iw Ib, RETF Iw
        if ((op == 0xC2) || (op == 0xC8) || (op == 0xCA))
        {
            dsize += 2;
        }

        // Check for immediate
        if (find_8(TABLE_IMM_A, op)) 
        {
            // `mov reg, imm` uses 64-bit immediate if REX.W is set
            if (in_range(0xb8, 0xc0, op) && rex_w) 
            {
                dsize += 8;
            }
            else 
            {
                dsize += ddef;
            }
        }

        // Special snowflake `movabs`
        if ((op & 0xFC) == 0xA0)
        {
            msize += mdef;
        }
    }

    // Mod R/M
    if (modrm)
    {
        if (!byte_iterator_next(&it, &op))
            return result;

        uint8_t mode = op & 0xC0;
        uint8_t rm = op & 0b111;

        if (mode != 0xC0) 
        {
            if (rm == 0b100) 
            {
                // Scaled Index Byte
                if (!byte_iterator_next(&it, &op))
                    return result;

                if (mode == 0x00) 
                {
                    if ((op & 0b111) == 0b101) 
                    {
                        msize += 4;
                    }
                }
            }

            if (mode == 0x00) 
            {
                if (rm == 0b101) 
                {
                    msize += 4;
                }
            }
            else if (mode == 0x40) 
            {
                msize += 1;
            }
            else if (mode == 0x80)
            {
                msize += 4;
            }
        }
    }

    // Get total length and bounds check
    uint32_t total_len = it.position;
    total_len += (dsize + msize);
    uint32_t arg_len = total_len - prefix_len - op_len;
    if (total_len <= length)
    {
        result.total_len = total_len;
        result.op_len = op_len;
        result.arg_len = arg_len;
        result.prefix_len = prefix_len;
    }

    return result;
}
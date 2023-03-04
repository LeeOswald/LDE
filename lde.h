#ifndef __LIBLDE_LDE_H_INCLUDED__
#define __LIBLDE_LDE_H_INCLUDED__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint32_t total_len;
    uint32_t op_len;
    uint32_t arg_len;
    uint32_t prefix_len;
} LdeInstructionLength;

LdeInstructionLength lde_get_instruction_length_x86(const void* opcode, uint32_t length);
LdeInstructionLength lde_get_instruction_length_x86_64(const void* opcode, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif // __LIBLDE_LDE_H_INCLUDED__


#include <lde.h>

#include "googletest/googletest/include/gtest/gtest.h"


TEST(x86_32, simple)
{
    // add al, *
    EXPECT_EQ(lde_get_instruction_length_x86("\x04*", 2).total_len, 2);
    // mov DWORD PTR [ebp+*]
    EXPECT_EQ(lde_get_instruction_length_x86("\x89\x5D*", 3).total_len, 3);
    // test al, al
    EXPECT_EQ(lde_get_instruction_length_x86("\x84\xC0", 2).total_len, 2);
    // fld QWORD PTR [eax+eax*4+****]
    EXPECT_EQ(lde_get_instruction_length_x86("\xDD\x84\x00****", 7).total_len, 7);
    // mov esi, ****
    EXPECT_EQ(lde_get_instruction_length_x86("\xBE****", 5).total_len, 5);
    // mov eax, fs:****
    EXPECT_EQ(lde_get_instruction_length_x86("\x64\xA1****", 6).total_len, 6);
    // add DWORD PTR ds:****, eax
    EXPECT_EQ(lde_get_instruction_length_x86("\x01\x05****", 6).total_len, 6);
    // addr16 mov eax, dx:**
    EXPECT_EQ(lde_get_instruction_length_x86("\x67\xA1**", 4).total_len, 4);
    // add BYTE PTR [bx+si+**], al
    EXPECT_EQ(lde_get_instruction_length_x86("\x67\x00\x80**", 5).total_len, 5);
    // inc eax
    EXPECT_EQ(lde_get_instruction_length_x86("\x40", 1).total_len, 1);
    // retn
    EXPECT_EQ(lde_get_instruction_length_x86("\xC3", 1).total_len, 1);
    // nop dword ptr [rax+*]
    EXPECT_EQ(lde_get_instruction_length_x86("\x0F\x1F\x40\x00", 4).total_len, 4);
    // nop dword ptr [rax+****]
    EXPECT_EQ(lde_get_instruction_length_x86("\x66\x0F\x0D\x80****", 8).total_len, 8);
    // clflush byte ptr [rax]
    EXPECT_EQ(lde_get_instruction_length_x86("\x0F\xAE\x38", 3).total_len, 3);
}

TEST(x86_64, simple)
{
    // sub rsp, *
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x48\x83\xEC*", 4).total_len, 4);
    // lea rcx, [rip+****]
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x48\x8D\x0D****", 7).total_len, 7);
    // rex push rbp
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x40\x55", 2).total_len, 2);
    // movabs rax, ********
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x48\xA3********", 10).total_len, 10);
    // addr32 mov ds:****, eax
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x67\x48\xA3****", 7).total_len, 7);
    // mov qword ptr [rbp+****], ****
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x48\xC7\x85\xA8\x00\x00\x00\x0C\x00\x00\x00", 11).total_len, 11);
    // mov word ptr [rsp+*], **
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x66\xC7\x44\x24\x34\x63\x74", 7).total_len, 7);
    // nop dword ptr [rax+*]
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x0F\x1F\x40\x00", 4).total_len, 4);
    // nop dword ptr [rax+****]
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x66\x0F\x0D\x80****", 8).total_len, 8);
    // weird nop
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x66\x66\x0f\x1f\x84\x00\x00\x00\x00\x00", 10).total_len, 10);
    // rep movsb
    EXPECT_EQ(lde_get_instruction_length_x86_64("\xF3\xA4", 2).total_len, 2);
    // mov r15, ********
    EXPECT_EQ(lde_get_instruction_length_x86_64("\x49\xBF********", 10).total_len, 10);

    EXPECT_EQ(lde_get_instruction_length_x86_64("\x40\x55\x48\x83\xEC\xFC\x00\x80", 8).total_len, 2);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

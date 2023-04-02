/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <windows.h>
#include <tlhelp32.h>
#include <limits.h>

#include "MinHook.h"
#include "MultiHook.h"

//#include "trampoline.h"

// buffer.c

// Size of each memory block. (= page size of VirtualAlloc)
#define MEMORY_BLOCK_SIZE 0x1000

// Max range for seeking a memory block. (= 1024MB)
#define MAX_MEMORY_RANGE 0x40000000

// Memory protection flags to check the executable address.
#define PAGE_EXECUTE_FLAGS \
    (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

// Memory slot.
typedef struct _MEMORY_SLOT
{
    union
    {
        struct _MEMORY_SLOT *pNext;
        UINT8 buffer[MEMORY_SLOT_SIZE];
    };
} MEMORY_SLOT, *PMEMORY_SLOT;

// Memory block info. Placed at the head of each block.
typedef struct _MEMORY_BLOCK
{
    struct _MEMORY_BLOCK *pNext;
    PMEMORY_SLOT pFree;         // First element of the free slot list.
    UINT usedCount;
} MEMORY_BLOCK, *PMEMORY_BLOCK;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------

// First element of the memory block list.
PMEMORY_BLOCK g_pMemoryBlocks;

//-------------------------------------------------------------------------
VOID InitializeBuffer(VOID)
{
    // Nothing to do for now.
}

//-------------------------------------------------------------------------
VOID UninitializeBuffer(VOID)
{
    PMEMORY_BLOCK pBlock = g_pMemoryBlocks;
    g_pMemoryBlocks = NULL;

    while (pBlock)
    {
        PMEMORY_BLOCK pNext = pBlock->pNext;
        VirtualFree(pBlock, 0, MEM_RELEASE);
        pBlock = pNext;
    }
}

//-------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
static LPVOID FindPrevFreeRegion(LPVOID pAddress, LPVOID pMinAddr, DWORD dwAllocationGranularity)
{
    ULONG_PTR tryAddr = (ULONG_PTR)pAddress;

    // Round down to the allocation granularity.
    tryAddr -= tryAddr % dwAllocationGranularity;

    // Start from the previous allocation granularity multiply.
    tryAddr -= dwAllocationGranularity;

    while (tryAddr >= (ULONG_PTR)pMinAddr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPVOID)tryAddr, &mbi, sizeof(mbi)) == 0)
            break;

        if (mbi.State == MEM_FREE)
            return (LPVOID)tryAddr;

        if ((ULONG_PTR)mbi.AllocationBase < dwAllocationGranularity)
            break;

        tryAddr = (ULONG_PTR)mbi.AllocationBase - dwAllocationGranularity;
    }

    return NULL;
}
#endif

//-------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
static LPVOID FindNextFreeRegion(LPVOID pAddress, LPVOID pMaxAddr, DWORD dwAllocationGranularity)
{
    ULONG_PTR tryAddr = (ULONG_PTR)pAddress;

    // Round down to the allocation granularity.
    tryAddr -= tryAddr % dwAllocationGranularity;

    // Start from the next allocation granularity multiply.
    tryAddr += dwAllocationGranularity;

    while (tryAddr <= (ULONG_PTR)pMaxAddr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPVOID)tryAddr, &mbi, sizeof(mbi)) == 0)
            break;

        if (mbi.State == MEM_FREE)
            return (LPVOID)tryAddr;

        tryAddr = (ULONG_PTR)mbi.BaseAddress + mbi.RegionSize;

        // Round up to the next allocation granularity.
        tryAddr += dwAllocationGranularity - 1;
        tryAddr -= tryAddr % dwAllocationGranularity;
    }

    return NULL;
}
#endif

//-------------------------------------------------------------------------
static PMEMORY_BLOCK GetMemoryBlock(LPVOID pOrigin)
{
    PMEMORY_BLOCK pBlock;
#if defined(_M_X64) || defined(__x86_64__)
    ULONG_PTR minAddr;
    ULONG_PTR maxAddr;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    minAddr = (ULONG_PTR)si.lpMinimumApplicationAddress;
    maxAddr = (ULONG_PTR)si.lpMaximumApplicationAddress;

    // pOrigin ± 512MB
    if ((ULONG_PTR)pOrigin > MAX_MEMORY_RANGE && minAddr < (ULONG_PTR)pOrigin - MAX_MEMORY_RANGE)
        minAddr = (ULONG_PTR)pOrigin - MAX_MEMORY_RANGE;

    if (maxAddr > (ULONG_PTR)pOrigin + MAX_MEMORY_RANGE)
        maxAddr = (ULONG_PTR)pOrigin + MAX_MEMORY_RANGE;

    // Make room for MEMORY_BLOCK_SIZE bytes.
    maxAddr -= MEMORY_BLOCK_SIZE - 1;
#endif

    // Look the registered blocks for a reachable one.
    for (pBlock = g_pMemoryBlocks; pBlock != NULL; pBlock = pBlock->pNext)
    {
#if defined(_M_X64) || defined(__x86_64__)
        // Ignore the blocks too far.
        if ((ULONG_PTR)pBlock < minAddr || (ULONG_PTR)pBlock >= maxAddr)
            continue;
#endif
        // The block has at least one unused slot.
        if (pBlock->pFree != NULL)
            return pBlock;
    }

#if defined(_M_X64) || defined(__x86_64__)
    // Alloc a new block above if not found.
    {
        LPVOID pAlloc = pOrigin;
        while ((ULONG_PTR)pAlloc >= minAddr)
        {
            pAlloc = FindPrevFreeRegion(pAlloc, (LPVOID)minAddr, si.dwAllocationGranularity);
            if (pAlloc == NULL)
                break;

            pBlock = (PMEMORY_BLOCK)VirtualAlloc(
                pAlloc, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (pBlock != NULL)
                break;
        }
    }

    // Alloc a new block below if not found.
    if (pBlock == NULL)
    {
        LPVOID pAlloc = pOrigin;
        while ((ULONG_PTR)pAlloc <= maxAddr)
        {
            pAlloc = FindNextFreeRegion(pAlloc, (LPVOID)maxAddr, si.dwAllocationGranularity);
            if (pAlloc == NULL)
                break;

            pBlock = (PMEMORY_BLOCK)VirtualAlloc(
                pAlloc, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (pBlock != NULL)
                break;
        }
    }
#else
    // In x86 mode, a memory block can be placed anywhere.
    pBlock = (PMEMORY_BLOCK)VirtualAlloc(
        NULL, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif

    if (pBlock != NULL)
    {
        // Build a linked list of all the slots.
        PMEMORY_SLOT pSlot = (PMEMORY_SLOT)pBlock + 1;
        pBlock->pFree = NULL;
        pBlock->usedCount = 0;
        do
        {
            pSlot->pNext = pBlock->pFree;
            pBlock->pFree = pSlot;
            pSlot++;
        } while ((ULONG_PTR)pSlot - (ULONG_PTR)pBlock <= MEMORY_BLOCK_SIZE - MEMORY_SLOT_SIZE);

        pBlock->pNext = g_pMemoryBlocks;
        g_pMemoryBlocks = pBlock;
    }

    return pBlock;
}

//-------------------------------------------------------------------------
LPVOID AllocateBuffer(LPVOID pOrigin)
{
    PMEMORY_SLOT  pSlot;
    PMEMORY_BLOCK pBlock = GetMemoryBlock(pOrigin);
    if (pBlock == NULL)
        return NULL;

    // Remove an unused slot from the list.
    pSlot = pBlock->pFree;
    pBlock->pFree = pSlot->pNext;
    pBlock->usedCount++;
#ifdef _DEBUG
    // Fill the slot with INT3 for debugging.
    memset(pSlot, 0xCC, sizeof(MEMORY_SLOT));
#endif
    return pSlot;
}

//-------------------------------------------------------------------------
VOID FreeBuffer(LPVOID pBuffer)
{
    PMEMORY_BLOCK pBlock = g_pMemoryBlocks;
    PMEMORY_BLOCK pPrev = NULL;
    ULONG_PTR pTargetBlock = ((ULONG_PTR)pBuffer / MEMORY_BLOCK_SIZE) * MEMORY_BLOCK_SIZE;

    while (pBlock != NULL)
    {
        if ((ULONG_PTR)pBlock == pTargetBlock)
        {
            PMEMORY_SLOT pSlot = (PMEMORY_SLOT)pBuffer;
#ifdef _DEBUG
            // Clear the released slot for debugging.
            memset(pSlot, 0x00, sizeof(MEMORY_SLOT));
#endif
            // Restore the released slot to the list.
            pSlot->pNext = pBlock->pFree;
            pBlock->pFree = pSlot;
            pBlock->usedCount--;

            // Free if unused.
            if (pBlock->usedCount == 0)
            {
                if (pPrev)
                    pPrev->pNext = pBlock->pNext;
                else
                    g_pMemoryBlocks = pBlock->pNext;

                VirtualFree(pBlock, 0, MEM_RELEASE);
            }

            break;
        }

        pPrev = pBlock;
        pBlock = pBlock->pNext;
    }
}

//-------------------------------------------------------------------------
BOOL IsExecutableAddress(LPVOID pAddress)
{
    MEMORY_BASIC_INFORMATION mi;
    VirtualQuery(pAddress, &mi, sizeof(mi));

    return (mi.State == MEM_COMMIT && (mi.Protect & PAGE_EXECUTE_FLAGS));
}



// trampoline.c

#ifdef _MSC_VER
    #include <intrin.h>
#endif

#ifndef ARRAYSIZE
    #define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif

#if defined(_M_X64) || defined(__x86_64__)
    // hde64.c
    unsigned int hde64_disasm(const void *code, hde64s *hs)
    {
        uint8_t x, c, *p = (uint8_t *)code, cflags, opcode, pref = 0;
        uint8_t *ht = hde64_table, m_mod, m_reg, m_rm, disp_size = 0;
        uint8_t op64 = 0;

        memset(hs, 0, sizeof(hde64s));

        for (x = 16; x; x--)
            switch (c = *p++) {
                case 0xf3:
                    hs->p_rep = c;
                    pref |= PRE_F3;
                    break;
                case 0xf2:
                    hs->p_rep = c;
                    pref |= PRE_F2;
                    break;
                case 0xf0:
                    hs->p_lock = c;
                    pref |= PRE_LOCK;
                    break;
                case 0x26: case 0x2e: case 0x36:
                case 0x3e: case 0x64: case 0x65:
                    hs->p_seg = c;
                    pref |= PRE_SEG;
                    break;
                case 0x66:
                    hs->p_66 = c;
                    pref |= PRE_66;
                    break;
                case 0x67:
                    hs->p_67 = c;
                    pref |= PRE_67;
                    break;
                default:
                    goto pref_done;
            }
    pref_done:

        hs->flags = (uint32_t)pref << 23;

        if (!pref)
            pref |= PRE_NONE;

        if ((c & 0xf0) == 0x40) {
            hs->flags |= F_PREFIX_REX;
            if ((hs->rex_w = (c & 0xf) >> 3) && (*p & 0xf8) == 0xb8)
                op64++;
            hs->rex_r = (c & 7) >> 2;
            hs->rex_x = (c & 3) >> 1;
            hs->rex_b = c & 1;
            if (((c = *p++) & 0xf0) == 0x40) {
                opcode = c;
                goto error_opcode;
            }
        }

        if ((hs->opcode = c) == 0x0f) {
            hs->opcode2 = c = *p++;
            ht += DELTA_OPCODES;
        } else if (c >= 0xa0 && c <= 0xa3) {
            op64++;
            if (pref & PRE_67)
                pref |= PRE_66;
            else
                pref &= ~PRE_66;
        }

        opcode = c;
        cflags = ht[ht[opcode / 4] + (opcode % 4)];

        if (cflags == C_ERROR) {
        error_opcode:
            hs->flags |= F_ERROR | F_ERROR_OPCODE;
            cflags = 0;
            if ((opcode & -3) == 0x24)
                cflags++;
        }

        x = 0;
        if (cflags & C_GROUP) {
            uint16_t t;
            t = *(uint16_t *)(ht + (cflags & 0x7f));
            cflags = (uint8_t)t;
            x = (uint8_t)(t >> 8);
        }

        if (hs->opcode2) {
            ht = hde64_table + DELTA_PREFIXES;
            if (ht[ht[opcode / 4] + (opcode % 4)] & pref)
                hs->flags |= F_ERROR | F_ERROR_OPCODE;
        }

        if (cflags & C_MODRM) {
            hs->flags |= F_MODRM;
            hs->modrm = c = *p++;
            hs->modrm_mod = m_mod = c >> 6;
            hs->modrm_rm = m_rm = c & 7;
            hs->modrm_reg = m_reg = (c & 0x3f) >> 3;

            if (x && ((x << m_reg) & 0x80))
                hs->flags |= F_ERROR | F_ERROR_OPCODE;

            if (!hs->opcode2 && opcode >= 0xd9 && opcode <= 0xdf) {
                uint8_t t = opcode - 0xd9;
                if (m_mod == 3) {
                    ht = hde64_table + DELTA_FPU_MODRM + t*8;
                    t = ht[m_reg] << m_rm;
                } else {
                    ht = hde64_table + DELTA_FPU_REG;
                    t = ht[t] << m_reg;
                }
                if (t & 0x80)
                    hs->flags |= F_ERROR | F_ERROR_OPCODE;
            }

            if (pref & PRE_LOCK) {
                if (m_mod == 3) {
                    hs->flags |= F_ERROR | F_ERROR_LOCK;
                } else {
                    uint8_t *table_end, op = opcode;
                    if (hs->opcode2) {
                        ht = hde64_table + DELTA_OP2_LOCK_OK;
                        table_end = ht + DELTA_OP_ONLY_MEM - DELTA_OP2_LOCK_OK;
                    } else {
                        ht = hde64_table + DELTA_OP_LOCK_OK;
                        table_end = ht + DELTA_OP2_LOCK_OK - DELTA_OP_LOCK_OK;
                        op &= -2;
                    }
                    for (; ht != table_end; ht++)
                        if (*ht++ == op) {
                            if (!((*ht << m_reg) & 0x80))
                                goto no_lock_error;
                            else
                                break;
                        }
                    hs->flags |= F_ERROR | F_ERROR_LOCK;
                no_lock_error:
                    ;
                }
            }

            if (hs->opcode2) {
                switch (opcode) {
                    case 0x20: case 0x22:
                        m_mod = 3;
                        if (m_reg > 4 || m_reg == 1)
                            goto error_operand;
                        else
                            goto no_error_operand;
                    case 0x21: case 0x23:
                        m_mod = 3;
                        if (m_reg == 4 || m_reg == 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                }
            } else {
                switch (opcode) {
                    case 0x8c:
                        if (m_reg > 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                    case 0x8e:
                        if (m_reg == 1 || m_reg > 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                }
            }

            if (m_mod == 3) {
                uint8_t *table_end;
                if (hs->opcode2) {
                    ht = hde64_table + DELTA_OP2_ONLY_MEM;
                    table_end = ht + sizeof(hde64_table) - DELTA_OP2_ONLY_MEM;
                } else {
                    ht = hde64_table + DELTA_OP_ONLY_MEM;
                    table_end = ht + DELTA_OP2_ONLY_MEM - DELTA_OP_ONLY_MEM;
                }
                for (; ht != table_end; ht += 2)
                    if (*ht++ == opcode) {
                        if ((*ht++ & pref) && !((*ht << m_reg) & 0x80))
                            goto error_operand;
                        else
                            break;
                    }
                goto no_error_operand;
            } else if (hs->opcode2) {
                switch (opcode) {
                    case 0x50: case 0xd7: case 0xf7:
                        if (pref & (PRE_NONE | PRE_66))
                            goto error_operand;
                        break;
                    case 0xd6:
                        if (pref & (PRE_F2 | PRE_F3))
                            goto error_operand;
                        break;
                    case 0xc5:
                        goto error_operand;
                }
                goto no_error_operand;
            } else
                goto no_error_operand;

        error_operand:
            hs->flags |= F_ERROR | F_ERROR_OPERAND;
        no_error_operand:

            c = *p++;
            if (m_reg <= 1) {
                if (opcode == 0xf6)
                    cflags |= C_IMM8;
                else if (opcode == 0xf7)
                    cflags |= C_IMM_P66;
            }

            switch (m_mod) {
                case 0:
                    if (pref & PRE_67) {
                        if (m_rm == 6)
                            disp_size = 2;
                    } else
                        if (m_rm == 5)
                            disp_size = 4;
                    break;
                case 1:
                    disp_size = 1;
                    break;
                case 2:
                    disp_size = 2;
                    if (!(pref & PRE_67))
                        disp_size <<= 1;
                    break;
            }

            if (m_mod != 3 && m_rm == 4) {
                hs->flags |= F_SIB;
                p++;
                hs->sib = c;
                hs->sib_scale = c >> 6;
                hs->sib_index = (c & 0x3f) >> 3;
                if ((hs->sib_base = c & 7) == 5 && !(m_mod & 1))
                    disp_size = 4;
            }

            p--;
            switch (disp_size) {
                case 1:
                    hs->flags |= F_DISP8;
                    hs->disp.disp8 = *p;
                    break;
                case 2:
                    hs->flags |= F_DISP16;
                    hs->disp.disp16 = *(uint16_t *)p;
                    break;
                case 4:
                    hs->flags |= F_DISP32;
                    hs->disp.disp32 = *(uint32_t *)p;
                    break;
            }
            p += disp_size;
        } else if (pref & PRE_LOCK)
            hs->flags |= F_ERROR | F_ERROR_LOCK;

        if (cflags & C_IMM_P66) {
            if (cflags & C_REL32) {
                if (pref & PRE_66) {
                    hs->flags |= F_IMM16 | F_RELATIVE;
                    hs->imm.imm16 = *(uint16_t *)p;
                    p += 2;
                    goto disasm_done;
                }
                goto rel32_ok;
            }
            if (op64) {
                hs->flags |= F_IMM64;
                hs->imm.imm64 = *(uint64_t *)p;
                p += 8;
            } else if (!(pref & PRE_66)) {
                hs->flags |= F_IMM32;
                hs->imm.imm32 = *(uint32_t *)p;
                p += 4;
            } else
                goto imm16_ok;
        }


        if (cflags & C_IMM16) {
        imm16_ok:
            hs->flags |= F_IMM16;
            hs->imm.imm16 = *(uint16_t *)p;
            p += 2;
        }
        if (cflags & C_IMM8) {
            hs->flags |= F_IMM8;
            hs->imm.imm8 = *p++;
        }

        if (cflags & C_REL32) {
        rel32_ok:
            hs->flags |= F_IMM32 | F_RELATIVE;
            hs->imm.imm32 = *(uint32_t *)p;
            p += 4;
        } else if (cflags & C_REL8) {
            hs->flags |= F_IMM8 | F_RELATIVE;
            hs->imm.imm8 = *p++;
        }

    disasm_done:

        if ((hs->len = (uint8_t)(p-(uint8_t *)code)) > 15) {
            hs->flags |= F_ERROR | F_ERROR_LENGTH;
            hs->len = 15;
        }

        return (unsigned int)hs->len;
    }
    typedef hde64s HDE;
    #define HDE_DISASM(code, hs) hde64_disasm(code, hs)
#else
    // hde32.c
    unsigned int hde32_disasm(const void *code, hde32s *hs)
    {
        uint8_t x, c, *p = (uint8_t *)code, cflags, opcode, pref = 0;
        uint8_t *ht = hde32_table, m_mod, m_reg, m_rm, disp_size = 0;

        memset(hs, 0, sizeof(hde32s));

        for (x = 16; x; x--)
            switch (c = *p++) {
                case 0xf3:
                    hs->p_rep = c;
                    pref |= PRE_F3;
                    break;
                case 0xf2:
                    hs->p_rep = c;
                    pref |= PRE_F2;
                    break;
                case 0xf0:
                    hs->p_lock = c;
                    pref |= PRE_LOCK;
                    break;
                case 0x26: case 0x2e: case 0x36:
                case 0x3e: case 0x64: case 0x65:
                    hs->p_seg = c;
                    pref |= PRE_SEG;
                    break;
                case 0x66:
                    hs->p_66 = c;
                    pref |= PRE_66;
                    break;
                case 0x67:
                    hs->p_67 = c;
                    pref |= PRE_67;
                    break;
                default:
                    goto pref_done;
            }
    pref_done:

        hs->flags = (uint32_t)pref << 23;

        if (!pref)
            pref |= PRE_NONE;

        if ((hs->opcode = c) == 0x0f) {
            hs->opcode2 = c = *p++;
            ht += DELTA_OPCODES;
        } else if (c >= 0xa0 && c <= 0xa3) {
            if (pref & PRE_67)
                pref |= PRE_66;
            else
                pref &= ~PRE_66;
        }

        opcode = c;
        cflags = ht[ht[opcode / 4] + (opcode % 4)];

        if (cflags == C_ERROR) {
            hs->flags |= F_ERROR | F_ERROR_OPCODE;
            cflags = 0;
            if ((opcode & -3) == 0x24)
                cflags++;
        }

        x = 0;
        if (cflags & C_GROUP) {
            uint16_t t;
            t = *(uint16_t *)(ht + (cflags & 0x7f));
            cflags = (uint8_t)t;
            x = (uint8_t)(t >> 8);
        }

        if (hs->opcode2) {
            ht = hde32_table + DELTA_PREFIXES;
            if (ht[ht[opcode / 4] + (opcode % 4)] & pref)
                hs->flags |= F_ERROR | F_ERROR_OPCODE;
        }

        if (cflags & C_MODRM) {
            hs->flags |= F_MODRM;
            hs->modrm = c = *p++;
            hs->modrm_mod = m_mod = c >> 6;
            hs->modrm_rm = m_rm = c & 7;
            hs->modrm_reg = m_reg = (c & 0x3f) >> 3;

            if (x && ((x << m_reg) & 0x80))
                hs->flags |= F_ERROR | F_ERROR_OPCODE;

            if (!hs->opcode2 && opcode >= 0xd9 && opcode <= 0xdf) {
                uint8_t t = opcode - 0xd9;
                if (m_mod == 3) {
                    ht = hde32_table + DELTA_FPU_MODRM + t*8;
                    t = ht[m_reg] << m_rm;
                } else {
                    ht = hde32_table + DELTA_FPU_REG;
                    t = ht[t] << m_reg;
                }
                if (t & 0x80)
                    hs->flags |= F_ERROR | F_ERROR_OPCODE;
            }

            if (pref & PRE_LOCK) {
                if (m_mod == 3) {
                    hs->flags |= F_ERROR | F_ERROR_LOCK;
                } else {
                    uint8_t *table_end, op = opcode;
                    if (hs->opcode2) {
                        ht = hde32_table + DELTA_OP2_LOCK_OK;
                        table_end = ht + DELTA_OP_ONLY_MEM - DELTA_OP2_LOCK_OK;
                    } else {
                        ht = hde32_table + DELTA_OP_LOCK_OK;
                        table_end = ht + DELTA_OP2_LOCK_OK - DELTA_OP_LOCK_OK;
                        op &= -2;
                    }
                    for (; ht != table_end; ht++)
                        if (*ht++ == op) {
                            if (!((*ht << m_reg) & 0x80))
                                goto no_lock_error;
                            else
                                break;
                        }
                    hs->flags |= F_ERROR | F_ERROR_LOCK;
                no_lock_error:
                    ;
                }
            }

            if (hs->opcode2) {
                switch (opcode) {
                    case 0x20: case 0x22:
                        m_mod = 3;
                        if (m_reg > 4 || m_reg == 1)
                            goto error_operand;
                        else
                            goto no_error_operand;
                    case 0x21: case 0x23:
                        m_mod = 3;
                        if (m_reg == 4 || m_reg == 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                }
            } else {
                switch (opcode) {
                    case 0x8c:
                        if (m_reg > 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                    case 0x8e:
                        if (m_reg == 1 || m_reg > 5)
                            goto error_operand;
                        else
                            goto no_error_operand;
                }
            }

            if (m_mod == 3) {
                uint8_t *table_end;
                if (hs->opcode2) {
                    ht = hde32_table + DELTA_OP2_ONLY_MEM;
                    table_end = ht + sizeof(hde32_table) - DELTA_OP2_ONLY_MEM;
                } else {
                    ht = hde32_table + DELTA_OP_ONLY_MEM;
                    table_end = ht + DELTA_OP2_ONLY_MEM - DELTA_OP_ONLY_MEM;
                }
                for (; ht != table_end; ht += 2)
                    if (*ht++ == opcode) {
                        if ((*ht++ & pref) && !((*ht << m_reg) & 0x80))
                            goto error_operand;
                        else
                            break;
                    }
                goto no_error_operand;
            } else if (hs->opcode2) {
                switch (opcode) {
                    case 0x50: case 0xd7: case 0xf7:
                        if (pref & (PRE_NONE | PRE_66))
                            goto error_operand;
                        break;
                    case 0xd6:
                        if (pref & (PRE_F2 | PRE_F3))
                            goto error_operand;
                        break;
                    case 0xc5:
                        goto error_operand;
                }
                goto no_error_operand;
            } else
                goto no_error_operand;

        error_operand:
            hs->flags |= F_ERROR | F_ERROR_OPERAND;
        no_error_operand:

            c = *p++;
            if (m_reg <= 1) {
                if (opcode == 0xf6)
                    cflags |= C_IMM8;
                else if (opcode == 0xf7)
                    cflags |= C_IMM_P66;
            }

            switch (m_mod) {
                case 0:
                    if (pref & PRE_67) {
                        if (m_rm == 6)
                            disp_size = 2;
                    } else
                        if (m_rm == 5)
                            disp_size = 4;
                    break;
                case 1:
                    disp_size = 1;
                    break;
                case 2:
                    disp_size = 2;
                    if (!(pref & PRE_67))
                        disp_size <<= 1;
                    break;
            }

            if (m_mod != 3 && m_rm == 4 && !(pref & PRE_67)) {
                hs->flags |= F_SIB;
                p++;
                hs->sib = c;
                hs->sib_scale = c >> 6;
                hs->sib_index = (c & 0x3f) >> 3;
                if ((hs->sib_base = c & 7) == 5 && !(m_mod & 1))
                    disp_size = 4;
            }

            p--;
            switch (disp_size) {
                case 1:
                    hs->flags |= F_DISP8;
                    hs->disp.disp8 = *p;
                    break;
                case 2:
                    hs->flags |= F_DISP16;
                    hs->disp.disp16 = *(uint16_t *)p;
                    break;
                case 4:
                    hs->flags |= F_DISP32;
                    hs->disp.disp32 = *(uint32_t *)p;
                    break;
            }
            p += disp_size;
        } else if (pref & PRE_LOCK)
            hs->flags |= F_ERROR | F_ERROR_LOCK;

        if (cflags & C_IMM_P66) {
            if (cflags & C_REL32) {
                if (pref & PRE_66) {
                    hs->flags |= F_IMM16 | F_RELATIVE;
                    hs->imm.imm16 = *(uint16_t *)p;
                    p += 2;
                    goto disasm_done;
                }
                goto rel32_ok;
            }
            if (pref & PRE_66) {
                hs->flags |= F_IMM16;
                hs->imm.imm16 = *(uint16_t *)p;
                p += 2;
            } else {
                hs->flags |= F_IMM32;
                hs->imm.imm32 = *(uint32_t *)p;
                p += 4;
            }
        }

        if (cflags & C_IMM16) {
            if (hs->flags & F_IMM32) {
                hs->flags |= F_IMM16;
                hs->disp.disp16 = *(uint16_t *)p;
            } else if (hs->flags & F_IMM16) {
                hs->flags |= F_2IMM16;
                hs->disp.disp16 = *(uint16_t *)p;
            } else {
                hs->flags |= F_IMM16;
                hs->imm.imm16 = *(uint16_t *)p;
            }
            p += 2;
        }
        if (cflags & C_IMM8) {
            hs->flags |= F_IMM8;
            hs->imm.imm8 = *p++;
        }

        if (cflags & C_REL32) {
        rel32_ok:
            hs->flags |= F_IMM32 | F_RELATIVE;
            hs->imm.imm32 = *(uint32_t *)p;
            p += 4;
        } else if (cflags & C_REL8) {
            hs->flags |= F_IMM8 | F_RELATIVE;
            hs->imm.imm8 = *p++;
        }

    disasm_done:

        if ((hs->len = (uint8_t)(p-(uint8_t *)code)) > 15) {
            hs->flags |= F_ERROR | F_ERROR_LENGTH;
            hs->len = 15;
        }

        return (unsigned int)hs->len;
    }

    typedef hde32s HDE;
    #define HDE_DISASM(code, hs) hde32_disasm(code, hs)
#endif

//-------------------------------------------------------------------------
static BOOL IsCodePadding(LPBYTE pInst, UINT size)
{
    UINT i;

    if (pInst[0] != 0x00 && pInst[0] != 0x90 && pInst[0] != 0xCC)
        return FALSE;

    for (i = 1; i < size; ++i)
    {
        if (pInst[i] != pInst[0])
            return FALSE;
    }
    return TRUE;
}

//-------------------------------------------------------------------------
VOID CreateRelayFunction(PJMP_RELAY pJmpRelay, LPVOID pDetour)
{
#if defined(_M_X64) || defined(__x86_64__)
    JMP_ABS jmp = {
        0xFF, 0x25, 0x00000000, // FF25 00000000: JMP [RIP+6]
        0x0000000000000000ULL   // Absolute destination address
    };

    jmp.address = (ULONG_PTR)pDetour;
#else
    JMP_REL jmp = {
        0xE9,                   // E9 xxxxxxxx: JMP +5+xxxxxxxx
        0x00000000              // Relative destination address
    };

    jmp.operand = (UINT32)((LPBYTE)pDetour - ((LPBYTE)pJmpRelay + sizeof(jmp)));
#endif

    memcpy(pJmpRelay, &jmp, sizeof(jmp));
}

//-------------------------------------------------------------------------
BOOL CreateTrampolineFunction(PTRAMPOLINE ct)
{
#if defined(_M_X64) || defined(__x86_64__)
    CALL_ABS call = {
        0xFF, 0x15, 0x00000002, // FF15 00000002: CALL [RIP+8]
        0xEB, 0x08,             // EB 08:         JMP +10
        0x0000000000000000ULL   // Absolute destination address
    };
    JMP_ABS jmp = {
        0xFF, 0x25, 0x00000000, // FF25 00000000: JMP [RIP+6]
        0x0000000000000000ULL   // Absolute destination address
    };
    JCC_ABS jcc = {
        0x70, 0x0E,             // 7* 0E:         J** +16
        0xFF, 0x25, 0x00000000, // FF25 00000000: JMP [RIP+6]
        0x0000000000000000ULL   // Absolute destination address
    };
#else
    CALL_REL call = {
        0xE8,                   // E8 xxxxxxxx: CALL +5+xxxxxxxx
        0x00000000              // Relative destination address
    };
    JMP_REL jmp = {
        0xE9,                   // E9 xxxxxxxx: JMP +5+xxxxxxxx
        0x00000000              // Relative destination address
    };
    JCC_REL jcc = {
        0x0F, 0x80,             // 0F8* xxxxxxxx: J** +6+xxxxxxxx
        0x00000000              // Relative destination address
    };
#endif

    UINT8     oldPos   = 0;
    UINT8     newPos   = 0;
    ULONG_PTR jmpDest  = 0;     // Destination address of an internal jump.
    BOOL      finished = FALSE; // Is the function completed?
#if defined(_M_X64) || defined(__x86_64__)
    UINT8     instBuf[16];
#endif

    ct->patchAbove = FALSE;
    ct->nIP        = 0;

    do
    {
        HDE       hs;
        UINT      copySize;
        LPVOID    pCopySrc;
        ULONG_PTR pOldInst = (ULONG_PTR)ct->pTarget     + oldPos;
        ULONG_PTR pNewInst = (ULONG_PTR)ct->pTrampoline + newPos;

        copySize = HDE_DISASM((LPVOID)pOldInst, &hs);
        if (hs.flags & F_ERROR)
            return FALSE;

        pCopySrc = (LPVOID)pOldInst;
        if (oldPos >= sizeof(JMP_REL))
        {
            // The trampoline function is long enough.
            // Complete the function with the jump to the target function.
#if defined(_M_X64) || defined(__x86_64__)
            jmp.address = pOldInst;
#else
            jmp.operand = (UINT32)(pOldInst - (pNewInst + sizeof(jmp)));
#endif
            pCopySrc = &jmp;
            copySize = sizeof(jmp);

            finished = TRUE;
        }
#if defined(_M_X64) || defined(__x86_64__)
        else if ((hs.modrm & 0xC7) == 0x05)
        {
            // Instructions using RIP relative addressing. (ModR/M = 00???101B)

            // Modify the RIP relative address.
            PUINT32 pRelAddr;

            // Avoid using memcpy to reduce the footprint.
#ifndef _MSC_VER
            memcpy(instBuf, (LPBYTE)pOldInst, copySize);
#else
            __movsb(instBuf, (LPBYTE)pOldInst, copySize);
#endif
            pCopySrc = instBuf;

            // Relative address is stored at (instruction length - immediate value length - 4).
            pRelAddr = (PUINT32)(instBuf + hs.len - ((hs.flags & 0x3C) >> 2) - 4);
            *pRelAddr
                = (UINT32)((pOldInst + hs.len + (INT32)hs.disp.disp32) - (pNewInst + hs.len));

            // Complete the function if JMP (FF /4).
            if (hs.opcode == 0xFF && hs.modrm_reg == 4)
                finished = TRUE;
        }
#endif
        else if (hs.opcode == 0xE8)
        {
            // Direct relative CALL
            ULONG_PTR dest = pOldInst + hs.len + (INT32)hs.imm.imm32;
#if defined(_M_X64) || defined(__x86_64__)
            call.address = dest;
#else
            call.operand = (UINT32)(dest - (pNewInst + sizeof(call)));
#endif
            pCopySrc = &call;
            copySize = sizeof(call);
        }
        else if ((hs.opcode & 0xFD) == 0xE9)
        {
            // Direct relative JMP (EB or E9)
            ULONG_PTR dest = pOldInst + hs.len;

            if (hs.opcode == 0xEB) // isShort jmp
                dest += (INT8)hs.imm.imm8;
            else
                dest += (INT32)hs.imm.imm32;

            // Simply copy an internal jump.
            if ((ULONG_PTR)ct->pTarget <= dest
                && dest < ((ULONG_PTR)ct->pTarget + sizeof(JMP_REL)))
            {
                if (jmpDest < dest)
                    jmpDest = dest;
            }
            else
            {
#if defined(_M_X64) || defined(__x86_64__)
                jmp.address = dest;
#else
                jmp.operand = (UINT32)(dest - (pNewInst + sizeof(jmp)));
#endif
                pCopySrc = &jmp;
                copySize = sizeof(jmp);

                // Exit the function if it is not in the branch.
                finished = (pOldInst >= jmpDest);
            }
        }
        else if ((hs.opcode & 0xF0) == 0x70
            || (hs.opcode & 0xFC) == 0xE0
            || (hs.opcode2 & 0xF0) == 0x80)
        {
            // Direct relative Jcc
            ULONG_PTR dest = pOldInst + hs.len;

            if ((hs.opcode & 0xF0) == 0x70      // Jcc
                || (hs.opcode & 0xFC) == 0xE0)  // LOOPNZ/LOOPZ/LOOP/JECXZ
                dest += (INT8)hs.imm.imm8;
            else
                dest += (INT32)hs.imm.imm32;

            // Simply copy an internal jump.
            if ((ULONG_PTR)ct->pTarget <= dest
                && dest < ((ULONG_PTR)ct->pTarget + sizeof(JMP_REL)))
            {
                if (jmpDest < dest)
                    jmpDest = dest;
            }
            else if ((hs.opcode & 0xFC) == 0xE0)
            {
                // LOOPNZ/LOOPZ/LOOP/JCXZ/JECXZ to the outside are not supported.
                return FALSE;
            }
            else
            {
                UINT8 cond = ((hs.opcode != 0x0F ? hs.opcode : hs.opcode2) & 0x0F);
#if defined(_M_X64) || defined(__x86_64__)
                // Invert the condition in x64 mode to simplify the conditional jump logic.
                jcc.opcode  = 0x71 ^ cond;
                jcc.address = dest;
#else
                jcc.opcode1 = 0x80 | cond;
                jcc.operand = (UINT32)(dest - (pNewInst + sizeof(jcc)));
#endif
                pCopySrc = &jcc;
                copySize = sizeof(jcc);
            }
        }
        else if ((hs.opcode & 0xFE) == 0xC2)
        {
            // RET (C2 or C3)

            // Complete the function if not in a branch.
            finished = (pOldInst >= jmpDest);
        }

        // Can't alter the instruction length in a branch.
        if (pOldInst < jmpDest && copySize != hs.len)
            return FALSE;

        // Trampoline function is too large.
        if ((newPos + copySize) > ct->trampolineSize)
            return FALSE;

        // Trampoline function has too many instructions.
        if (ct->nIP >= ARRAYSIZE(ct->oldIPs))
            return FALSE;

        ct->oldIPs[ct->nIP] = oldPos;
        ct->newIPs[ct->nIP] = newPos;
        ct->nIP++;

        // Avoid using memcpy to reduce the footprint.
#ifndef _MSC_VER
        memcpy((LPBYTE)ct->pTrampoline + newPos, pCopySrc, copySize);
#else
        __movsb((LPBYTE)ct->pTrampoline + newPos, (LPBYTE)pCopySrc, copySize);
#endif
        newPos += copySize;
        oldPos += hs.len;
    }
    while (!finished);

    // Is there enough place for a long jump?
    if (oldPos < sizeof(JMP_REL)
        && !IsCodePadding((LPBYTE)ct->pTarget + oldPos, sizeof(JMP_REL) - oldPos))
    {
        // Is there enough place for a short jump?
        if (oldPos < sizeof(JMP_REL_SHORT)
            && !IsCodePadding((LPBYTE)ct->pTarget + oldPos, sizeof(JMP_REL_SHORT) - oldPos))
        {
            return FALSE;
        }

        // Can we place the long jump above the function?
        if (!IsExecutableAddress((LPBYTE)ct->pTarget - sizeof(JMP_REL)))
            return FALSE;

        if (!IsCodePadding((LPBYTE)ct->pTarget - sizeof(JMP_REL), sizeof(JMP_REL)))
            return FALSE;

        ct->patchAbove = TRUE;
    }

    return TRUE;
}

// hook.c

// Initial capacity of the HOOK_ENTRY buffer.
#define INITIAL_HOOK_CAPACITY   32

// Initial capacity of the thread IDs buffer.
#define INITIAL_THREAD_CAPACITY 128

// Special hook position values.
#define INVALID_HOOK_POS UINT_MAX

// Thread access rights for suspending/resuming threads.
#define THREAD_ACCESS \
    (THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SET_CONTEXT)

// Suspended threads for Freeze()/Unfreeze().
typedef struct _FROZEN_THREADS
{
    LPHANDLE pItems;         // Data heap
    UINT     capacity;       // Size of allocated data heap, items
    UINT     size;           // Actual number of data items
} FROZEN_THREADS, *PFROZEN_THREADS;

// Thread freeze related definitions.
typedef NTSTATUS(NTAPI* NtGetNextThread_t)(
    _In_ HANDLE ProcessHandle,
    _In_opt_ HANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_ ULONG Flags,
    _Out_ PHANDLE NewThreadHandle
    );

#ifndef STATUS_NO_MORE_ENTRIES
#define STATUS_NO_MORE_ENTRIES ((NTSTATUS)0x8000001AL)
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

// Function and function pointer declarations.
typedef MH_STATUS(WINAPI *ENABLE_HOOK_LL_PROC)(UINT pos, BOOL enable, PFROZEN_THREADS pThreads);
typedef MH_STATUS(WINAPI *DISABLE_HOOK_CHAIN_PROC)(ULONG_PTR hookIdent, LPVOID pTarget, UINT parentPos, ENABLE_HOOK_LL_PROC ParentEnableHookLL, PFROZEN_THREADS pThreads);

static MH_STATUS WINAPI DisableHookChain(ULONG_PTR hookIdent, LPVOID pTarget, UINT parentPos, ENABLE_HOOK_LL_PROC ParentEnableHookLL, PFROZEN_THREADS pThreads);

// Executable buffer of a hook.
typedef struct _EXEC_BUFFER
{
    DISABLE_HOOK_CHAIN_PROC pDisableHookChain;
    ULONG_PTR hookIdent;
    JMP_RELAY jmpRelay;
    UINT8     trampoline[1]; // Uses the rest of the MEMORY_SLOT_SIZE bytes.
} EXEC_BUFFER, *PEXEC_BUFFER;

// Hook information.
typedef struct _HOOK_ENTRY
{
    ULONG_PTR hookIdent;        // Hook identifier, allows to hook the same function multiple times with different identifiers.

    LPVOID pTarget;             // Address of the target function.
    LPVOID pDetour;             // Address of the detour function.
    PEXEC_BUFFER pExecBuffer;   // Address of the executable buffer for relay and trampoline.
    UINT8  backup[8];           // Original prologue of the target function.

    UINT8  patchAbove  : 1;     // Uses the hot patch area.
    UINT8  isEnabled   : 1;     // Enabled.
    UINT8  queueEnable : 1;     // Queued for enabling/disabling when != isEnabled.

    UINT   nIP : 4;             // Count of the instruction boundaries.
    UINT8  oldIPs[8];           // Instruction boundaries of the target function.
    UINT8  newIPs[8];           // Instruction boundaries of the trampoline function.
} HOOK_ENTRY, *PHOOK_ENTRY;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------

// Mutex. If not NULL, this library is initialized.
HANDLE g_hMutex = NULL;

// Private heap handle.
HANDLE g_hHeap;

// Thread freeze related variables.
MH_THREAD_FREEZE_METHOD g_threadFreezeMethod = MH_FREEZE_METHOD_ORIGINAL;

NtGetNextThread_t NtGetNextThread;

// Hook entries.
struct
{
    PHOOK_ENTRY pItems;     // Data heap
    UINT        capacity;   // Size of allocated data heap, items
    UINT        size;       // Actual number of data items
} g_hooks;

//-------------------------------------------------------------------------
// Returns INVALID_HOOK_POS if not found.
static UINT FindHookEntry(ULONG_PTR hookIdent, LPVOID pTarget)
{
    UINT i;
    for (i = 0; i < g_hooks.size; ++i)
    {
        PHOOK_ENTRY pHook = &g_hooks.pItems[i];
        if (hookIdent == pHook->hookIdent && (ULONG_PTR)pTarget == (ULONG_PTR)pHook->pTarget)
            return i;
    }

    return INVALID_HOOK_POS;
}

//-------------------------------------------------------------------------
static PHOOK_ENTRY AddHookEntry()
{
    if (g_hooks.pItems == NULL)
    {
        g_hooks.capacity = INITIAL_HOOK_CAPACITY;
        g_hooks.pItems = (PHOOK_ENTRY)HeapAlloc(
            g_hHeap, 0, g_hooks.capacity * sizeof(HOOK_ENTRY));
        if (g_hooks.pItems == NULL)
            return NULL;
    }
    else if (g_hooks.size >= g_hooks.capacity)
    {
        PHOOK_ENTRY p = (PHOOK_ENTRY)HeapReAlloc(
            g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity * 2) * sizeof(HOOK_ENTRY));
        if (p == NULL)
            return NULL;

        g_hooks.capacity *= 2;
        g_hooks.pItems = p;
    }

    return &g_hooks.pItems[g_hooks.size++];
}

//-------------------------------------------------------------------------
static VOID DeleteHookEntry(UINT pos)
{
    if (pos < g_hooks.size - 1)
        g_hooks.pItems[pos] = g_hooks.pItems[g_hooks.size - 1];

    g_hooks.size--;

    if (g_hooks.capacity / 2 >= INITIAL_HOOK_CAPACITY && g_hooks.capacity / 2 >= g_hooks.size)
    {
        PHOOK_ENTRY p = (PHOOK_ENTRY)HeapReAlloc(
            g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity / 2) * sizeof(HOOK_ENTRY));
        if (p == NULL)
            return;

        g_hooks.capacity /= 2;
        g_hooks.pItems = p;
    }
}

//-------------------------------------------------------------------------
static DWORD_PTR FindOldIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
    // In any of the jump locations:
    // Target -> Hotpatch jump (if patchAbove) -> Relay jump
    // Restore IP to the detour. This is required for consistent behavior
    // as a part of a DisableHookChain call, otherwise, if IP is restored
    // to the target, hooks that should be called may be skipped.

    if (ip == (DWORD_PTR)pHook->pTarget)
        return (DWORD_PTR)pHook->pDetour;

    if (pHook->patchAbove && ip == ((DWORD_PTR)pHook->pTarget - sizeof(JMP_REL)))
        return (DWORD_PTR)pHook->pDetour;

    if (ip == (DWORD_PTR)&pHook->pExecBuffer->jmpRelay)
        return (DWORD_PTR)pHook->pDetour;

    UINT i;
    for (i = 0; i < pHook->nIP; ++i)
    {
        if (ip == ((DWORD_PTR)pHook->pExecBuffer->trampoline + pHook->newIPs[i]))
            return (DWORD_PTR)pHook->pTarget + pHook->oldIPs[i];
    }

    return 0;
}

//-------------------------------------------------------------------------
static DWORD_PTR FindNewIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
    UINT i;
    for (i = 0; i < pHook->nIP; ++i)
    {
        if (ip == ((DWORD_PTR)pHook->pTarget + pHook->oldIPs[i]))
            return (DWORD_PTR)pHook->pExecBuffer->trampoline + pHook->newIPs[i];
    }

    return 0;
}

//-------------------------------------------------------------------------
static VOID ProcessThreadIPs(HANDLE hThread, UINT pos, BOOL enable)
{
    // If the thread suspended in the overwritten area,
    // move IP to the proper address.

    CONTEXT     c;
#if defined(_M_X64) || defined(__x86_64__)
    DWORD64     *pIP = &c.Rip;
#else
    DWORD       *pIP = &c.Eip;
#endif
    PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
    DWORD_PTR   ip;

    c.ContextFlags = CONTEXT_CONTROL;
    if (!GetThreadContext(hThread, &c))
        return;

    if (enable)
        ip = FindNewIP(pHook, *pIP);
    else
        ip = FindOldIP(pHook, *pIP);

    if (ip != 0)
    {
        *pIP = ip;
        SetThreadContext(hThread, &c);
    }
}

//-------------------------------------------------------------------------
static BOOL EnumerateAndSuspendThreads(PFROZEN_THREADS pThreads)
{
    BOOL succeeded = FALSE;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hSnapshot, &te))
        {
            succeeded = TRUE;
            do
            {
                if (te.dwSize >= (FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(DWORD))
                    && te.th32OwnerProcessID == GetCurrentProcessId()
                    && te.th32ThreadID != GetCurrentThreadId())
                {
                    HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, te.th32ThreadID);

                    if (hThread != NULL && SuspendThread(hThread) == (DWORD)-1)
                    {
                        CloseHandle(hThread);
                        hThread = NULL;
                    }

                    if (hThread != NULL)
                    {
                        if (pThreads->pItems == NULL)
                        {
                            pThreads->capacity = INITIAL_THREAD_CAPACITY;
                            pThreads->pItems
                                = (LPHANDLE)HeapAlloc(g_hHeap, 0, pThreads->capacity * sizeof(HANDLE));
                            if (pThreads->pItems == NULL)
                                succeeded = FALSE;
                        }
                        else if (pThreads->size >= pThreads->capacity)
                        {
                            pThreads->capacity *= 2;
                            LPHANDLE p = (LPHANDLE)HeapReAlloc(
                                g_hHeap, 0, pThreads->pItems, pThreads->capacity * sizeof(HANDLE));
                            if (p)
                                pThreads->pItems = p;
                            else
                                succeeded = FALSE;
                        }

                        if (!succeeded)
                        {
                            ResumeThread(hThread);
                            CloseHandle(hThread);
                            break;
                        }

                        pThreads->pItems[pThreads->size++] = hThread;
                    }
                }

                te.dwSize = sizeof(THREADENTRY32);
            } while (Thread32Next(hSnapshot, &te));

            if (succeeded && GetLastError() != ERROR_NO_MORE_FILES)
                succeeded = FALSE;

            if (!succeeded && pThreads->pItems != NULL)
            {
                UINT i;
                for (i = 0; i < pThreads->size; ++i)
                {
                    ResumeThread(pThreads->pItems[i]);
                    CloseHandle(pThreads->pItems[i]);
                }

                HeapFree(g_hHeap, 0, pThreads->pItems);
                pThreads->pItems = NULL;
            }
        }
        CloseHandle(hSnapshot);
    }

    return succeeded;
}

//-------------------------------------------------------------------------
static BOOL EnumerateAndSuspendThreadsFast(PFROZEN_THREADS pThreads)
{
    BOOL succeeded = TRUE;

    HANDLE hThread = NULL;
    BOOL bClosePrevThread = FALSE;
    while (1)
    {
        HANDLE hNextThread;
        NTSTATUS status = NtGetNextThread(GetCurrentProcess(), hThread, THREAD_ACCESS, 0, 0, &hNextThread);
        if (bClosePrevThread)
            CloseHandle(hThread);

        if (!NT_SUCCESS(status))
        {
            if (status != STATUS_NO_MORE_ENTRIES)
                succeeded = FALSE;
            break;
        }

        hThread = hNextThread;
        bClosePrevThread = TRUE;

        if (GetThreadId(hThread) == GetCurrentThreadId())
            continue;

        if (SuspendThread(hThread) == (DWORD)-1)
            continue;

        bClosePrevThread = FALSE;

        if (pThreads->pItems == NULL)
        {
            pThreads->capacity = INITIAL_THREAD_CAPACITY;
            pThreads->pItems
                = (LPHANDLE)HeapAlloc(g_hHeap, 0, pThreads->capacity * sizeof(HANDLE));
            if (pThreads->pItems == NULL)
                succeeded = FALSE;
        }
        else if (pThreads->size >= pThreads->capacity)
        {
            pThreads->capacity *= 2;
            LPHANDLE p = (LPHANDLE)HeapReAlloc(
                g_hHeap, 0, pThreads->pItems, pThreads->capacity * sizeof(HANDLE));
            if (p)
                pThreads->pItems = p;
            else
                succeeded = FALSE;
        }

        if (!succeeded)
        {
            ResumeThread(hThread);
            CloseHandle(hThread);
            break;
        }

        // Perform a synchronous operation to make sure the thread really is suspended.
        // https://devblogs.microsoft.com/oldnewthing/20150205-00/?p=44743
        CONTEXT c;
        c.ContextFlags = CONTEXT_CONTROL;
        GetThreadContext(hThread, &c);

        pThreads->pItems[pThreads->size++] = hThread;
    }

    if (!succeeded && pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            ResumeThread(pThreads->pItems[i]);
            CloseHandle(pThreads->pItems[i]);
        }

        HeapFree(g_hHeap, 0, pThreads->pItems);
        pThreads->pItems = NULL;
    }

    return succeeded;
}

//-------------------------------------------------------------------------
static VOID ProcessFrozenThreads(PFROZEN_THREADS pThreads, UINT pos, BOOL enable)
{
    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            ProcessThreadIPs(pThreads->pItems[i], pos, enable);
        }
    }
}

//-------------------------------------------------------------------------
static MH_STATUS Freeze(PFROZEN_THREADS pThreads)
{
    MH_STATUS status = MH_OK;

    pThreads->pItems   = NULL;
    pThreads->capacity = 0;
    pThreads->size     = 0;

    switch (g_threadFreezeMethod)
    {
    case MH_FREEZE_METHOD_ORIGINAL:
        if (!EnumerateAndSuspendThreads(pThreads))
            status = MH_ERROR_MEMORY_ALLOC;
        break;

    case MH_FREEZE_METHOD_FAST_UNDOCUMENTED:
        if (!EnumerateAndSuspendThreadsFast(pThreads))
            status = MH_ERROR_MEMORY_ALLOC;
        break;

    case MH_FREEZE_METHOD_NONE_UNSAFE:
        // Nothing to do.
        break;
    }

    return status;
}

//-------------------------------------------------------------------------
static VOID Unfreeze(PFROZEN_THREADS pThreads)
{
    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            ResumeThread(pThreads->pItems[i]);
            CloseHandle(pThreads->pItems[i]);
        }

        HeapFree(g_hHeap, 0, pThreads->pItems);
    }
}

//-------------------------------------------------------------------------
static MH_STATUS CreateHookTrampoline(UINT pos)
{
    PHOOK_ENTRY pHook = &g_hooks.pItems[pos];

    TRAMPOLINE ct;
    ct.pTarget = pHook->pTarget;
    ct.pTrampoline = pHook->pExecBuffer->trampoline;
    ct.trampolineSize = MEMORY_SLOT_SIZE - offsetof(EXEC_BUFFER, trampoline);
    if (!CreateTrampolineFunction(&ct))
    {
        return MH_ERROR_UNSUPPORTED_FUNCTION;
    }

    // Back up the target function.
    if (ct.patchAbove)
    {
        memcpy(
            pHook->backup,
            (LPBYTE)pHook->pTarget - sizeof(JMP_REL),
            sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
    }
    else
    {
        memcpy(pHook->backup, pHook->pTarget, sizeof(JMP_REL));
    }

    pHook->patchAbove = ct.patchAbove;
    pHook->nIP = ct.nIP;
    memcpy(pHook->oldIPs, ct.oldIPs, ARRAYSIZE(ct.oldIPs));
    memcpy(pHook->newIPs, ct.newIPs, ARRAYSIZE(ct.newIPs));

    return MH_OK;
}

//-------------------------------------------------------------------------
static MH_STATUS WINAPI EnableHookLL(UINT pos, BOOL enable, PFROZEN_THREADS pThreads)
{
    PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
    DWORD  oldProtect;
    SIZE_T patchSize    = sizeof(JMP_REL);
    LPBYTE pPatchTarget = (LPBYTE)pHook->pTarget;

    if (enable)
    {
        MH_STATUS status = CreateHookTrampoline(pos);
        if (status != MH_OK)
            return status;
    }

    if (pHook->patchAbove)
    {
        pPatchTarget -= sizeof(JMP_REL);
        patchSize    += sizeof(JMP_REL_SHORT);
    }

    if (!enable)
    {
        PJMP_REL pJmp = (PJMP_REL)pPatchTarget;
        if (pJmp->opcode == 0xE9)
        {
            PJMP_RELAY pJmpRelay = (PJMP_RELAY)(((LPBYTE)pJmp + sizeof(JMP_REL)) + (INT32)pJmp->operand);
            if (&pHook->pExecBuffer->jmpRelay != pJmpRelay)
            {
                PEXEC_BUFFER pOtherExecBuffer = (PEXEC_BUFFER)((LPBYTE)pJmpRelay - offsetof(EXEC_BUFFER, jmpRelay));
                return pOtherExecBuffer->pDisableHookChain(pOtherExecBuffer->hookIdent, pHook->pTarget, pos, EnableHookLL, pThreads);
            }
        }
    }

    if (!VirtualProtect(pPatchTarget, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect))
        return MH_ERROR_MEMORY_PROTECT;

    if (enable)
    {
        PJMP_REL pJmp = (PJMP_REL)pPatchTarget;
        pJmp->opcode = 0xE9;
        pJmp->operand = (UINT32)((LPBYTE)&pHook->pExecBuffer->jmpRelay - (pPatchTarget + sizeof(JMP_REL)));

        if (pHook->patchAbove)
        {
            PJMP_REL_SHORT pShortJmp = (PJMP_REL_SHORT)pHook->pTarget;
            pShortJmp->opcode = 0xEB;
            pShortJmp->operand = (UINT8)(0 - (sizeof(JMP_REL_SHORT) + sizeof(JMP_REL)));
        }
    }
    else
    {
        if (pHook->patchAbove)
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        else
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL));
    }

    VirtualProtect(pPatchTarget, patchSize, oldProtect, &oldProtect);

    // Just-in-case measure.
    FlushInstructionCache(GetCurrentProcess(), pPatchTarget, patchSize);

    ProcessFrozenThreads(pThreads, pos, enable);

    pHook->isEnabled   = enable;
    pHook->queueEnable = enable;

    return MH_OK;
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHooksLL(ULONG_PTR hookIdent, LPVOID pTarget, BOOL enable)
{
    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;

    for (i = 0; i < g_hooks.size; ++i)
    {
        PHOOK_ENTRY pHook = &g_hooks.pItems[i];
        if (pHook->isEnabled != enable &&
            (hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
            (pTarget == MH_ALL_HOOKS || (ULONG_PTR)pTarget == (ULONG_PTR)pHook->pTarget))
        {
            first = i;
            break;
        }
    }

    if (first != INVALID_HOOK_POS)
    {
        FROZEN_THREADS threads;
        status = Freeze(&threads);
        if (status == MH_OK)
        {
            for (i = first; i < g_hooks.size; ++i)
            {
                PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                if (pHook->isEnabled != enable &&
                    (hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
                    (pTarget == MH_ALL_HOOKS || (ULONG_PTR)pTarget == (ULONG_PTR)pHook->pTarget))
                {
                    MH_STATUS enable_status = EnableHookLL(i, enable, &threads);

                    // Instead of stopping on the first error, we enable as much
                    // hooks as we can, and return the last error, if any.
                    if (enable_status != MH_OK)
                        status = enable_status;
                }
            }

            Unfreeze(&threads);
        }
    }

    return status;
}

//-------------------------------------------------------------------------
static HANDLE CreateProcessMutex(VOID)
{
    TCHAR szMutexName[sizeof("minhook_multihook_12345678")] = TEXT("minhook_multihook_");
    UINT mutexNameLen = sizeof("minhook_multihook_") - 1;
    DWORD dw = GetCurrentProcessId();
    UINT i;

    // Build szMutexName in the following format:
    // printf("minhook_multihook_%08X", GetCurrentProcessId());

    for (i = 0; i < 8; i++)
    {
        TCHAR ch;
        BYTE b = dw >> (32 - 4);

        if (b < 0x0A)
            ch = b + TEXT('0');
        else
            ch = b - 0x0A + TEXT('A');

        szMutexName[mutexNameLen++] = ch;
        dw <<= 4;
    }

    szMutexName[mutexNameLen] = TEXT('\0');

    return CreateMutex(NULL, FALSE, szMutexName);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Initialize(VOID)
{
    if (g_hMutex != NULL)
        return MH_ERROR_ALREADY_INITIALIZED;

    g_hMutex = CreateProcessMutex();
    if (g_hMutex == NULL)
        return MH_ERROR_MUTEX_FAILURE;

    g_hHeap = HeapCreate(0, 0, 0);
    if (g_hHeap == NULL)
    {
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
        return MH_ERROR_MEMORY_ALLOC;
    }

    // Initialize the internal function buffer.
    InitializeBuffer();

    return MH_OK;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Uninitialize(VOID)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = EnableHooksLL(MH_ALL_IDENTS, MH_ALL_HOOKS, FALSE);

    ReleaseMutex(g_hMutex);

    if (status != MH_OK)
        return status;

    // Free the internal function buffer.
    // HeapFree is actually not required, but some tools detect a false
    // memory leak without HeapFree.
    UninitializeBuffer();
    HeapFree(g_hHeap, 0, g_hooks.pItems);
    HeapDestroy(g_hHeap);
    g_hHeap = NULL;

    g_hooks.pItems = NULL;
    g_hooks.capacity = 0;
    g_hooks.size = 0;

    CloseHandle(g_hMutex);
    g_hMutex = NULL;

    return MH_OK;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_SetThreadFreezeMethod(MH_THREAD_FREEZE_METHOD method)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    if (method == MH_FREEZE_METHOD_FAST_UNDOCUMENTED && !NtGetNextThread)
    {
        HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
        if (hNtdll)
            NtGetNextThread = (NtGetNextThread_t)GetProcAddress(hNtdll, "NtGetNextThread");

        if (!NtGetNextThread)
        {
            // Fall back to the original method.
            method = MH_FREEZE_METHOD_ORIGINAL;
        }
    }

    g_threadFreezeMethod = method;

    ReleaseMutex(g_hMutex);

    return MH_OK;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHookEx(ULONG_PTR hookIdent, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;

    if (IsExecutableAddress(pTarget) && IsExecutableAddress(pDetour))
    {
        UINT pos = FindHookEntry(hookIdent, pTarget);
        if (pos == INVALID_HOOK_POS)
        {
            PEXEC_BUFFER pBuffer = (PEXEC_BUFFER)AllocateBuffer(pTarget);
            if (pBuffer != NULL)
            {
                PHOOK_ENTRY pHook = AddHookEntry();
                if (pHook != NULL)
                {
                    pBuffer->hookIdent = hookIdent;
                    pBuffer->pDisableHookChain = DisableHookChain;
                    CreateRelayFunction(&pBuffer->jmpRelay, pDetour);

                    pHook->hookIdent = hookIdent;
                    pHook->pTarget = pTarget;
                    pHook->pDetour = pDetour;
                    pHook->pExecBuffer = pBuffer;
                    pHook->isEnabled = FALSE;
                    pHook->queueEnable = FALSE;

                    if (ppOriginal != NULL)
                        *ppOriginal = pBuffer->trampoline;
                }
                else
                {
                    status = MH_ERROR_MEMORY_ALLOC;
                }

                if (status != MH_OK)
                {
                    FreeBuffer(pBuffer);
                }
            }
            else
            {
                status = MH_ERROR_MEMORY_ALLOC;
            }
        }
        else
        {
            status = MH_ERROR_ALREADY_CREATED;
        }
    }
    else
    {
        status = MH_ERROR_NOT_EXECUTABLE;
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
    return MH_CreateHookEx(MH_DEFAULT_IDENT, pTarget, pDetour, ppOriginal);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveHookEx(ULONG_PTR hookIdent, LPVOID pTarget)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;

    if (hookIdent == MH_ALL_IDENTS || pTarget == MH_ALL_HOOKS)
    {
        status = EnableHooksLL(hookIdent, pTarget, FALSE);
        if (status == MH_OK)
        {
            UINT i = 0;
            while (i < g_hooks.size)
            {
                PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                if ((hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
                    (pTarget == MH_ALL_HOOKS || (ULONG_PTR)pTarget == (ULONG_PTR)pHook->pTarget))
                {
                    FreeBuffer(pHook->pExecBuffer);
                    DeleteHookEntry(i);
                }
                else
                {
                    ++i;
                }
            }
        }
    }
    else
    {
        UINT pos = FindHookEntry(hookIdent, pTarget);
        if (pos != INVALID_HOOK_POS)
        {
            if (g_hooks.pItems[pos].isEnabled)
            {
                FROZEN_THREADS threads;
                status = Freeze(&threads);
                if (status == MH_OK)
                {
                    status = EnableHookLL(pos, FALSE, &threads);

                    Unfreeze(&threads);
                }
            }

            if (status == MH_OK)
            {
                FreeBuffer(g_hooks.pItems[pos].pExecBuffer);
                DeleteHookEntry(pos);
            }
        }
        else
        {
            status = MH_ERROR_NOT_CREATED;
        }
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget)
{
    return MH_RemoveHookEx(MH_DEFAULT_IDENT, pTarget);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveDisabledHooksEx(ULONG_PTR hookIdent)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;

    UINT i = 0;
    while (i < g_hooks.size)
    {
        PHOOK_ENTRY pHook = &g_hooks.pItems[i];
        if ((hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
            !pHook->isEnabled)
        {
            FreeBuffer(pHook->pExecBuffer);
            DeleteHookEntry(i);
        }
        else
        {
            ++i;
        }
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveDisabledHooks()
{
    return MH_RemoveDisabledHooksEx(MH_DEFAULT_IDENT);
}

//-------------------------------------------------------------------------
static MH_STATUS WINAPI DisableHookChain(ULONG_PTR hookIdent, LPVOID pTarget, UINT parentPos, ENABLE_HOOK_LL_PROC ParentEnableHookLL, PFROZEN_THREADS pThreads)
{
    UINT pos = FindHookEntry(hookIdent, pTarget);
    if (pos == INVALID_HOOK_POS)
        return MH_ERROR_NOT_CREATED;

    if (!g_hooks.pItems[pos].isEnabled)
        return MH_ERROR_DISABLED;

    // We're not Freeze()-ing the threads here, because we assume that the function
    // was called from a different MinHook module, which already suspended all threads.

    MH_STATUS status = EnableHookLL(pos, FALSE, pThreads);
    if (status != MH_OK)
        return status;

    status = ParentEnableHookLL(parentPos, FALSE, pThreads);
    if (status != MH_OK)
        return status;

    return EnableHookLL(pos, TRUE, pThreads);
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHook(ULONG_PTR hookIdent, LPVOID pTarget, BOOL enable)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;

    if (hookIdent == MH_ALL_IDENTS || pTarget == MH_ALL_HOOKS)
    {
        status = EnableHooksLL(hookIdent, pTarget, enable);
    }
    else
    {
        UINT pos = FindHookEntry(hookIdent, pTarget);
        if (pos != INVALID_HOOK_POS)
        {
            if (g_hooks.pItems[pos].isEnabled != enable)
            {
                FROZEN_THREADS threads;
                status = Freeze(&threads);
                if (status == MH_OK)
                {
                    status = EnableHookLL(pos, enable, &threads);

                    Unfreeze(&threads);
                }
            }
            else
            {
                status = enable ? MH_ERROR_ENABLED : MH_ERROR_DISABLED;
            }
        }
        else
        {
            status = MH_ERROR_NOT_CREATED;
        }
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_EnableHookEx(ULONG_PTR hookIdent, LPVOID pTarget)
{
    return EnableHook(hookIdent, pTarget, TRUE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget)
{
    return MH_EnableHookEx(MH_DEFAULT_IDENT, pTarget);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_DisableHookEx(ULONG_PTR hookIdent, LPVOID pTarget)
{
    return EnableHook(hookIdent, pTarget, FALSE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget)
{
    return MH_DisableHookEx(MH_DEFAULT_IDENT, pTarget);
}

//-------------------------------------------------------------------------
static MH_STATUS QueueHook(ULONG_PTR hookIdent, LPVOID pTarget, BOOL queueEnable)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;

    if (hookIdent == MH_ALL_IDENTS || pTarget == MH_ALL_HOOKS)
    {
        UINT i;
        for (i = 0; i < g_hooks.size; ++i)
        {
            PHOOK_ENTRY pHook = &g_hooks.pItems[i];
            if ((hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
                (pTarget == MH_ALL_HOOKS || (ULONG_PTR)pTarget == (ULONG_PTR)pHook->pTarget))
            {
                pHook->queueEnable = queueEnable;
            }
        }
    }
    else
    {
        UINT pos = FindHookEntry(hookIdent, pTarget);
        if (pos != INVALID_HOOK_POS)
        {
            g_hooks.pItems[pos].queueEnable = queueEnable;
        }
        else
        {
            status = MH_ERROR_NOT_CREATED;
        }
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueEnableHookEx(ULONG_PTR hookIdent, LPVOID pTarget)
{
    return QueueHook(hookIdent, pTarget, TRUE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueEnableHook(LPVOID pTarget)
{
    return MH_QueueEnableHookEx(MH_DEFAULT_IDENT, pTarget);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueDisableHookEx(ULONG_PTR hookIdent, LPVOID pTarget)
{
    return QueueHook(hookIdent, pTarget, FALSE);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueDisableHook(LPVOID pTarget)
{
    return MH_QueueDisableHookEx(MH_DEFAULT_IDENT, pTarget);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_ApplyQueuedEx(ULONG_PTR hookIdent)
{
    if (g_hMutex == NULL)
        return MH_ERROR_NOT_INITIALIZED;

    if (WaitForSingleObject(g_hMutex, INFINITE) != WAIT_OBJECT_0)
        return MH_ERROR_MUTEX_FAILURE;

    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;

    for (i = 0; i < g_hooks.size; ++i)
    {
        PHOOK_ENTRY pHook = &g_hooks.pItems[i];
        if ((hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
            pHook->isEnabled != pHook->queueEnable)
        {
            first = i;
            break;
        }
    }

    if (first != INVALID_HOOK_POS)
    {
        FROZEN_THREADS threads;
        status = Freeze(&threads);
        if (status == MH_OK)
        {
            for (i = first; i < g_hooks.size; ++i)
            {
                PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                if ((hookIdent == MH_ALL_IDENTS || pHook->hookIdent == hookIdent) &&
                    pHook->isEnabled != pHook->queueEnable)
                {
                    MH_STATUS enable_status = EnableHookLL(i, pHook->queueEnable, &threads);

                    // Instead of stopping on the first error, we apply as much
                    // hooks as we can, and return the last error, if any.
                    if (enable_status != MH_OK)
                        status = enable_status;
                }
            }

            Unfreeze(&threads);
        }
    }

    ReleaseMutex(g_hMutex);

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_ApplyQueued(VOID)
{
    return MH_ApplyQueuedEx(MH_DEFAULT_IDENT);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHookApiEx(
    LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour,
    LPVOID *ppOriginal, LPVOID *ppTarget)
{
    HMODULE hModule;
    LPVOID  pTarget;

    hModule = GetModuleHandleW(pszModule);
    if (hModule == NULL)
        return MH_ERROR_MODULE_NOT_FOUND;

    pTarget = (LPVOID)GetProcAddress(hModule, pszProcName);
    if (pTarget == NULL)
        return MH_ERROR_FUNCTION_NOT_FOUND;

    if(ppTarget != NULL)
        *ppTarget = pTarget;

    return MH_CreateHook(pTarget, pDetour, ppOriginal);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHookApi(
    LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID *ppOriginal)
{
   return MH_CreateHookApiEx(pszModule, pszProcName, pDetour, ppOriginal, NULL);
}

//-------------------------------------------------------------------------
const char * WINAPI MH_StatusToString(MH_STATUS status)
{
#define MH_ST2STR(x)    \
    case x:             \
        return #x;

    switch (status) {
        MH_ST2STR(MH_UNKNOWN)
        MH_ST2STR(MH_OK)
        MH_ST2STR(MH_ERROR_ALREADY_INITIALIZED)
        MH_ST2STR(MH_ERROR_NOT_INITIALIZED)
        MH_ST2STR(MH_ERROR_ALREADY_CREATED)
        MH_ST2STR(MH_ERROR_NOT_CREATED)
        MH_ST2STR(MH_ERROR_ENABLED)
        MH_ST2STR(MH_ERROR_DISABLED)
        MH_ST2STR(MH_ERROR_NOT_EXECUTABLE)
        MH_ST2STR(MH_ERROR_UNSUPPORTED_FUNCTION)
        MH_ST2STR(MH_ERROR_MEMORY_ALLOC)
        MH_ST2STR(MH_ERROR_MEMORY_PROTECT)
        MH_ST2STR(MH_ERROR_MODULE_NOT_FOUND)
        MH_ST2STR(MH_ERROR_FUNCTION_NOT_FOUND)
        MH_ST2STR(MH_ERROR_MUTEX_FAILURE)
    }

#undef MH_ST2STR

    return "(unknown)";
}

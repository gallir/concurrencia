/**
 * Opcodes definitions for AMD's ASF (Advanced Synchronization Facility)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * Copyright (c) 2007-2009 Advanced Micro Devices, Inc.
 * Contributed by Stephan Diestelhorst <stephan.diestelhorst@amd.com>
 *
 * @author stephan.diestelhorst@amd.com
 * @date 21.06.2007
 */
#ifndef __ASF_OPCODES_H_
#define __ASF_OPCODES_H_

/* First: The registers for ModR/M.rm field */
#define ASF_REG_AX "(0x0)"
#define ASF_REG_CX "(0x1)"
#define ASF_REG_DX "(0x2)"
#define ASF_REG_BX "(0x3)"

#define ASF_REG_SP "(0x4)"
#define ASF_REG_BP "(0x5)"
#define ASF_REG_SI "(0x6)"
#define ASF_REG_DI "(0x7)"

/* ModRM byte helpers */
/* ModRM mod = 3: direct registers in ModR/M.rm */
#define ASF_MOD_MEM "(0x0)"
#define ASF_MOD_REG "(0x3)"
#define ASF_MOD_RM(mod, reg, rm)\
	"(((" mod ")<<6)+((" reg ")<<3)+(" rm "))"

/* REX Prefix */
#define ASF_REX(w,r,x,b)\
	".byte 0x40+(" w "<<3)+(" r "<<2)+(" x "<<1)+" b "\n\t"
#define ASF_REX64 REX("1","0","0","0")

/* Operand Size prefix */
#define ASF_DATA16\
	".byte 0x66\n\t"

/* LOCK prefix */
#define ASF_LOCK\
	".byte 0xF0\n\t"

/* Encodings in XOP-space */
#define ASF_XOP1\
    ".byte 0x8F \n\t"
#define ASF_XOP2(R_ext_inv, X_ext_inv, B_ext_inv, mmmm)\
    ".byte ((" R_ext_inv ") << 7) | ((" X_ext_inv ") << 6) | ((" B_ext_inv ") << 5) | (" mmmm ") \n\t"
#define ASF_XOP3(W, vvvv, L, pp)\
    ".byte ((" W ") << 7) | ((" vvvv ") << 3) | ((" L ") << 2) | (" pp ") \n\t"

#define ASF_SPECULATE\
    "#speculate \n\t"\
    ASF_XOP1 \
    ASF_XOP2("1", "1", "1", "0x9") \
    ASF_XOP3("0", "0xF", "0", "0") \
    ".byte 0x0F \n\t" \
    ".byte " ASF_MOD_RM( ASF_MOD_REG, ASF_REG_AX, "5" ) "\n\t"

#define ASF_ABORT\
    "#abort\n\t"\
    ASF_XOP1 \
    ASF_XOP2("1", "1", "1", "0x9") \
    ASF_XOP3("0", "0xF", "0", "0") \
    ".byte 0x0F \n\t" \
    ".byte "  ASF_MOD_RM( ASF_MOD_REG, ASF_REG_AX, "6") "\n\t"

#define ASF_COMMIT\
    "#commit\n\t"\
    ASF_XOP1 \
    ASF_XOP2("1", "1", "1", "0x9") \
    ASF_XOP3("0", "0xF", "0", "0") \
    ".byte 0x0F \n\t" \
    ".byte " ASF_MOD_RM( ASF_MOD_REG, ASF_REG_AX, "7") "\n\t"

/* This is a simplified version with support only for simple indirect register
   addressing modes */
#define ASF_RELEASE(ind_reg)\
    "#release\n\t"\
    ASF_XOP1 \
    ASF_XOP2("1", "1", "1", "0x9") \
    ASF_XOP3("0", "0xF", "0", "0") \
    ".byte 0x12 \n\t" \
    ".byte " ASF_MOD_RM( ASF_MOD_MEM, "0", ind_reg) "\n\t"

#endif


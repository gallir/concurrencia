/**
 * Definitions of high-level helper functions which can
 * be helpful to use traditional C / C++ code for ASF
 * critical sections.
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
 * @date 27.7.2007
 */

#ifndef __ASF_HIGHLEVEL_H_
#define __ASF_HIGHLEVEL_H_

#include "asf-opcodes.h"

/* Size of lines for ASF */
#define ASF_LINE_SIZE (64L)
#define ASF_LINE_MASK (~(ASF_LINE_SIZE - 1))

/**
 * Specifies a line to be used in ASF, w/o reading the data.
 * @param m Pointer to data to be fetched.
 */
static inline void asf_lock_prefetchw(void *m) {
	// NOTE: using "m"(*ptr) allows also immediate addressed locations
	//       but sometimes uses an additional register :(
	asm volatile(
		ASF_LOCK
		"prefetchw %0"
		:
		:"m"(*(long*)m));
}
static inline void asf_lock_prefetch(void *m) {
    // NOTE: using "m"(*ptr) allows also immediate addressed locations
    //       but sometimes uses an additional register :(
    asm volatile(
        ASF_LOCK
        "prefetch %0"
        :
        :"m"(*(long*)m));
}
// XXX annotate with __attribute__((noreturn))
static inline void asf_abort(int abort_code) {
    asm volatile(
        ASF_ABORT
        :
        : "a" (abort_code));
}
enum ASFStatusCode {
    ASF_STATUS_SUCCESS = 0,
    ASF_STATUS_CONTENTION = 16,
    ASF_STATUS_ABORT = 18,
    ASF_STATUS_FAR = 19,
    ASF_STATUS_DISALLOWED_OP = -2,
    ASF_STATUS_CAPACITY = -1
};
static const int ASF_ERROR_MASK = 0xFF;

static const int ASF_HARD_ERROR_MASK = (1 << 7);

static const int ASF_NEST_LEVEL_SHIFT = 8;
static const int ASF_NEST_LEVEL_MASK  = 0xFF;

static const int ASF_ABORT_CODE_SHIFT = 16;
static const int ASF_ABORT_CODE_MASK  = 0xFFFF;

static inline int asf_status_code(int speculate_return) {
    return (int)((signed char)(speculate_return & ASF_ERROR_MASK));
}
static inline int asf_hard_error(int speculate_return) {
    return speculate_return & ASF_HARD_ERROR_MASK;
}
static inline int asf_nest_level(int speculate_return) {
    return (speculate_return >> ASF_NEST_LEVEL_SHIFT) & ASF_NEST_LEVEL_MASK;
}
static inline int asf_abort_code(int speculate_return) {
    return (speculate_return >> ASF_ABORT_CODE_SHIFT) & ASF_ABORT_CODE_MASK;
}

//TODO: Other accessors: abort code, hard vs soft, nesting level
static inline void asf_release(void *m) {
    /* It is painful to hand the fully flexible addressing mode to an unknown
       opcode. */
    asm volatile(
        ASF_RELEASE(ASF_REG_BX)
        :
        :"b"(m));
}
/**
 * Reads a location and specifies its subsequent usage in an ASF
 * transaction.
 * @param m Pointer to memory.
 * @return Data at m.
 */
#define ASF_LOCK_LOAD(BITS, TYPE, POSTFIX)				\
static inline TYPE asf_lock_load##BITS(const TYPE* m) {	\
	TYPE t;					                            \
	asm ( ASF_LOCK "mov"#POSTFIX" %1,%0"                \
		:"=r"(t):"m"(*m));                              \
	return t;                                           \
}
ASF_LOCK_LOAD( 8, unsigned  char, b);
ASF_LOCK_LOAD(16, unsigned short, w);
ASF_LOCK_LOAD(32, unsigned   int, l);
ASF_LOCK_LOAD(64, unsigned  long, q);

/**
 * Stores data speculatively to a location
 * @param m Pointer to destination.
 * @param d Data to store.
 */
#define ASF_LOCK_STORE(BITS, TYPE, POSTFIX)                 \
static inline void asf_lock_store##BITS(TYPE* m, TYPE d) {  \
	asm ( ASF_LOCK "mov"#POSTFIX" %0,%1"                    \
	::"r"(d),"m"(*m));                                      \
}
ASF_LOCK_STORE( 8, unsigned  char, b);
ASF_LOCK_STORE(16, unsigned short, w);
ASF_LOCK_STORE(32, unsigned   int, l);
ASF_LOCK_STORE(64, unsigned  long, q);

#define asf_speculate_push(fail, _ign_)					\
	asm volatile(										\
		"push %%rbp \n\t"								\
		"8: "ASF_SPECULATE								\
		:"=a"(fail))

#define asf_speculate_fail_clobber_pop(fail, _ign_)\
	asm volatile( 										\
		"mov %1, %0 \n\t"								\
		"pop %%rbp"										\
		:"=a"(fail)										\
		:"r"(fail)										\
		:"memory","rbx","rcx","rdx","rsi", "rdi",	\
		"r8", "r9","r10","r11","r12","r13", "r14","r15" )

#define asf_speculate_stack(fail, state_save)			\
	asm volatile(										\
		"mov  %%rbp, %1 \n\t"							\
		"8: "ASF_SPECULATE								\
		:"=a"(fail),"=m"(state_save))

#define asf_speculate_fail_clobber_stack(fail, state_save)	\
	asm volatile( 										\
		"mov %2, %0 \n\t"								\
		"mov %1, %%rbp"									\
		: "=a"(fail)									\
		:"m"(state_save),"r"(fail)						\
		:"memory","rbx","rcx","rdx","rsi", "rdi",	\
		 "r8", "r9","r10","r11","r12","r13", "r14","r15" )


/**
 * Commits all modifications made to the cachelines.
 */
static inline long asf_commit_() {
  long res;
  asm volatile("9: "ASF_COMMIT :"=a"(res) : :"memory");
  return res;
}
static inline long asf_commit_pop() {
  long res;
  asm volatile("9: "ASF_COMMIT "\n\t"
	           "add $8,%%rsp"
	           :"=a"(res) : :"memory");
  return res;
}
#ifndef ASF_PUSH
#ifndef ASF_STACK
#error Specify either "-DASF_STACK -fomit-frame-pointer" or "-DASF_PUSH -fno-omit-frame-pointer".
#endif
#endif

#ifdef ASF_PUSH
  #warning "ASF uses the push based interface. Make sure to ENABLE frame-pointers."
  #define asf_speculate              asf_speculate_push
  #define asf_speculate_fail_clobber asf_speculate_fail_clobber_pop
  #define asf_commit                 asf_commit_pop
#endif
#ifdef ASF_STACK
//  #warning "ASF uses the stack based interface. Make sure to DISABLE frame-pointers."
  #define asf_speculate              asf_speculate_stack
  #define asf_speculate_fail_clobber asf_speculate_fail_clobber_stack 
  #define asf_commit                 asf_commit_
#endif

/**
 * Touches a certain number of arguments-
 */
#define touch1(arg1) \
	asm volatile (""::"g"(arg1))
#define touch2(arg1,arg2) \
	touch1(arg1);         \
	touch1(arg2)
#define touch3(arg1,arg2,arg3) \
	touch1(arg1);              \
	touch2(arg2, arg3)
#define touch4(arg1,arg2,arg3,arg4) \
	touch1(arg1);                   \
	touch3(arg2, arg3, arg4)
#define touch5(arg1,arg2,arg3,arg4,arg5) \
	touch1(arg1);                        \
	touch4(arg2, arg3, arg4, arg5)
#define touch6(arg1,arg2,arg3,arg4,arg5,arg6) \
	touch1(arg1);                             \
	touch5(arg2, arg3, arg4, arg5, arg6)
#define touch7(arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
	touch1(arg1);                                  \
	touch6(arg2, arg3, arg4, arg5, arg6, arg7)
#define touch8(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8) \
	touch1(arg1);                                       \
	touch7(arg2, arg3, arg4, arg5, arg6, arg7, arg8)

/**
 * Returns the start / end of the next / previous
 * transaction. Relies on the fact, that nobody uses
 * the local labels 8 and 9, between these functions
 * and the transactions themselves.
 */
static inline long start_prev_txn() {
	long start;
	asm volatile ("movq $8b, %0":"=r"(start));
	return start;
}
static inline long end_prev_txn() {
	long start;
	asm volatile ("movq $9b, %0":"=r"(start));
	return start;
}
static inline long start_next_txn() {
	long start;
	asm volatile ("movq $8f, %0":"=r"(start));
	return start;
}
static inline long end_next_txn() {
	long start;
	asm volatile ("movq $9f, %0":"=r"(start));
	return start;
}
static inline long start_this_txn() {
	long start;
	asm volatile ("movq $8b, %0":"=r"(start));
	return start;
}
static inline long end_this_txn() {
	long start;
	asm volatile ("movq $9f, %0":"=r"(start));
	return start;
}
#endif

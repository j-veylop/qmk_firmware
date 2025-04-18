/*
    ChibiOS - Copyright (C) 2021 Stefan Kerkmann.
    ChibiOS - Copyright (C) 2020 Patrick Seidel.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    RISCV/chcore.h
 * @brief   Port related template macros and structures.
 *
 * @addtogroup port_core
 * @{
 */

#ifndef CHCORE_H
#define CHCORE_H

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @name    Port Capabilities and Constants
 * @{
 */
/**
 * @brief   This port supports a realtime counter.
 */
#define PORT_SUPPORTS_RT TRUE

/**
 * @brief   Natural alignment constant.
 * @note    It is the minimum alignment for pointer-size variables.
 */
#define PORT_NATURAL_ALIGN sizeof(void *)

/**
 * @brief   Stack alignment constant.
 * @note    It is the alignment required for the stack pointer.
 */
#define PORT_STACK_ALIGN sizeof(stkalign_t)

/**
 * @brief   Working Areas alignment constant.
 * @note    It is the alignment to be enforced for thread working areas.
 */
#define PORT_WORKING_AREA_ALIGN sizeof(stkalign_t)
/** @} */

/**
 * @name    Architecture and Compiler
 * @{
 */
/**
 * @brief   Macro defining a RISCV architecture.
 */
#define PORT_ARCHITECTURE_RISCV

/**
 * @brief   Macro defining the specific RISCV architecture.
 */
#define PORT_ARCHITECTURE_RISCV_YYY

/**
 * @brief   Name of the implemented architecture.
 */
#define PORT_ARCHITECTURE_NAME          "RISC-V"

/**
 * @brief   Name of the architecture variant.
 */
#define PORT_CORE_VARIANT_NAME          "RV32IMAC"

/**
 * @brief   Compiler name and version.
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
#define PORT_COMPILER_NAME "GCC " __VERSION__

#else
#error "unsupported compiler"
#endif

/**
 * @brief   Port-specific information string.
 */
#define PORT_INFO "RV32IMAC - ECLIC"
/** @} */

/* Inclusion of the RISC-V implementation specific parameters.*/
#if !defined(_FROM_ASM_)

#include "eclic_parameters.h"
#include "nmsis_core.h"

#endif

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Stack size for the system idle thread.
 * @details This size depends on the idle thread implementation, usually
 *          the idle thread should take no more space than those reserved
 *          by @p PORT_INT_REQUIRED_STACK.
 */
#if !defined(PORT_IDLE_THREAD_STACK_SIZE) || defined(__DOXYGEN__)
#define PORT_IDLE_THREAD_STACK_SIZE 64
#endif

/**
 * @brief   Per-thread stack overhead for interrupts servicing.
 * @details This constant is used in the calculation of the correct working
 *          area size.
 */
#if !defined(PORT_INT_REQUIRED_STACK) || defined(__DOXYGEN__)
#define PORT_INT_REQUIRED_STACK 256
#endif

/**
 * @brief   Enables an alternative timer implementation.
 * @details Usually the port uses a timer interface defined in the file
 *          @p chcore_timer.h, if this option is enabled then the file
 *          @p chcore_timer_alt.h is included instead.
 */
#if !defined(PORT_USE_ALT_TIMER) || defined(__DOXYGEN__)
#define PORT_USE_ALT_TIMER FALSE
#endif

/**
 * @brief   Enables a "wait for interrupt" instruction in the idle loop.
 */
#if !defined(PORT_RISCV_WFI_SLEEP_IDLE) || defined(__DOXYGEN__)
#define PORT_RISCV_ENABLE_WFI_IDLE FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Type of stack and memory alignment enforcement.
 * @note    In this architecture the stack alignment is enforced to 128 bits.
 */
typedef struct {
  uint8_t padding[16];
} stkalign_t ALIGNED_VAR(16);

/**
 * @brief   Interrupt saved context.
 * @details This structure represents the stack frame saved during a
 *          preemption-capable interrupt handler.
 * @note    GP is not saved because it is assumed to be immutable
 *          during the system life cycle.
 */
struct port_extctx {
  uint32_t ra;
  uint32_t tp;
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  uint32_t a0;
  uint32_t a1;
  uint32_t a2;
  uint32_t a3;
  uint32_t a4;
  uint32_t a5;
  uint32_t mepc;
  uint32_t mcause;
  uint32_t msubm;
#if !defined(__riscv_32e)
  uint32_t a6;
  uint32_t a7;
  uint32_t t3;
  uint32_t t4;
  uint32_t t5;
  uint32_t t6;
#endif
};

/**
 * @brief   System saved context.
 * @details This structure represents the inner stack frame during a context
 *          switching.
 * @note    GP and TP are not saved because those are assumed to be immutable
 *          during the system life cycle.
 */
struct port_intctx {
  uint32_t ra;
  uint32_t s0;
  uint32_t s1;
#if !defined(__riscv_32e)
  uint32_t s2;
  uint32_t s3;
  uint32_t s4;
  uint32_t s5;
  uint32_t s6;
  uint32_t s7;
  uint32_t s8;
  uint32_t s9;
  uint32_t s10;
  uint32_t s11;
  uint32_t padding_2[2];
#endif
  uint32_t padding_1;
};

/**
 * @brief   Platform dependent part of the @p thread_t structure.
 * @details This structure usually contains just the saved stack pointer
 *          defined as a pointer to a @p port_intctx structure.
 */
struct port_context {
  struct port_intctx *sp;
};

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Platform dependent part of the @p chThdCreateI() API.
 * @details This code usually setup the context switching frame represented
 *          by an @p port_intctx structure.
 */
#define PORT_SETUP_CONTEXT(tp, wbase, wtop, pf, arg)                           \
  {                                                                            \
    (tp)->ctx.sp = (struct port_intctx *)((uint8_t *)(wtop) -                  \
                                          sizeof(struct port_intctx));         \
    (tp)->ctx.sp->ra = (uint32_t)_port_thread_start;                           \
    (tp)->ctx.sp->s0 = (uint32_t)(pf);                                         \
    (tp)->ctx.sp->s1 = (uint32_t)(arg);                                        \
  }

/**
 * @brief   Computes the thread working area global size.
 * @note    There is no need to perform alignments in this macro.
 */
#define PORT_WA_SIZE(n)                                                        \
  (sizeof(struct port_intctx) + sizeof(struct port_extctx) + ((size_t)(n)) +   \
   ((size_t)(PORT_INT_REQUIRED_STACK)))

/**
 * @brief   Static working area allocation.
 * @details This macro is used to allocate a static thread working area
 *          aligned as both position and size.
 *
 * @param[in] s         the name to be assigned to the stack array
 * @param[in] n         the stack size to be assigned to the thread
 */
#define PORT_WORKING_AREA(s, n)                                                \
  stkalign_t s[THD_WORKING_AREA_SIZE(n) / sizeof(stkalign_t)]

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_PRIORITY(n) true

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_KERNEL_PRIORITY(n) true

/**
 * @brief   IRQ prologue code.
 * @details This macro must be inserted at the start of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_PROLOGUE()

/**
 * @brief   IRQ epilogue code.
 * @details This macro must be inserted at the end of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_EPILOGUE() do {                                                                          \
                            port_lock_from_isr();                                                         \
                            /* Check if we are the tail of a possible interrupt chain. */                 \
                            bool is_preemption_required = ((__RV_CSR_READ(CSR_MSUBM) & MSUBM_PTYP) == 0)  \
                                  && chSchIsPreemptionRequired();                                         \
                            port_unlock_from_isr();                                                       \
                            return is_preemption_required;                                                \
                            } while(0)

/**
 * @brief   IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#ifdef __cplusplus
#define PORT_IRQ_HANDLER(id) extern "C" bool id(void)
#else
#define PORT_IRQ_HANDLER(id) bool id(void)
#endif

/**
 * @brief   Fast IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#ifdef __cplusplus
#define PORT_FAST_IRQ_HANDLER(id)                                              \
  extern "C" __attribute__((interrupt)) void id(void)
#else
#define PORT_FAST_IRQ_HANDLER(id) __attribute__((interrupt)) void id(void)
#endif

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 */
#if !CH_DBG_ENABLE_STACK_CHECK || defined(__DOXYGEN__)
#define port_switch(ntp, otp) _port_switch(ntp, otp)
#else
#define port_switch(ntp, otp)                                                  \
  {                                                                            \
    register struct port_intctx *sp asm("%sp");                                \
    if ((stkalign_t *)(sp - 1) < otp->wabase) {                                \
      chSysHalt("stackoverflow");                                              \
    }                                                                          \
    _port_switch(ntp, otp);                                                    \
  }
#endif

/**
 * @brief   Returns a word representing a critical section status.
 *
 * @return              The critical section status.
 */
#define port_get_lock_status() port_get_irq_status()

/**
 * @brief   Determines if in a critical section.
 *
 * @param[in] sts       status word returned by @p port_get_lock_status()
 * @return              The current status.
 * @retval false        if running outside a critical section.
 * @retval true         if running within a critical section.
 */
#define port_is_locked(sts) !port_irq_enabled(sts)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

#ifdef __cplusplus
extern "C" {
#endif
void _port_switch(thread_t *ntp, thread_t *otp);
void _port_switch_after_isr(void);
void _port_switch_from_isr(void);
void _port_exit_from_isr(void);
void _port_thread_start(void);
bool _port_irq_epilogue(void);
#ifdef __cplusplus
}
#endif

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Port-related initialization code.
 */
static inline void port_init(os_instance_t *oip){}

/**
 * @brief   Returns a word encoding the current interrupts status.
 *
 * @return              The interrupts status.
 */
static inline syssts_t port_get_irq_status(void) {
  return (syssts_t)__RV_CSR_READ(CSR_MSTATUS);
}

/**
 * @brief   Checks the interrupt status.
 *
 * @param[in] sts       the interrupt status word
 *
 * @return              The interrupt status.
 * @retval false        the word specified a disabled interrupts status.
 * @retval true         the word specified an enabled interrupts status.
 */
static inline bool port_irq_enabled(syssts_t sts) { return sts & MSTATUS_MIE; }

/**
 * @brief   Determines the current execution context.
 *
 * @return              The execution context.
 * @retval false        not running in ISR mode.
 * @retval true         running in ISR mode.
 */
static inline bool port_is_isr_context(void) {
  /* msubm.typ == 1 is interrupt handling mode. */
  return __RV_CSR_READ(CSR_MSUBM) & (0x1 << 6);
}

/**
 * @brief   Kernel-lock action.
 * @details Usually this function just disables interrupts but may perform more
 *          actions.
 */
static inline void port_lock(void) { __disable_irq(); }

/**
 * @brief   Kernel-unlock action.
 * @details Usually this function just enables interrupts but may perform more
 *          actions.
 */
static inline void port_unlock(void) { __enable_irq(); }

/**
 * @brief   Kernel-lock action from an interrupt handler.
 * @details This function is invoked before invoking I-class APIs from
 *          interrupt handlers. The implementation is architecture dependent,
 *          in its simplest form it is void.
 */
static inline void port_lock_from_isr(void) { port_lock(); }

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 * @details This function is invoked after invoking I-class APIs from interrupt
 *          handlers. The implementation is architecture dependent, in its
 *          simplest form it is void.
 */
static inline void port_unlock_from_isr(void) { port_unlock(); }

/**
 * @brief   Disables all the interrupt sources.
 * @note    Of course non-maskable interrupt sources are not included.
 */
static inline void port_disable(void) { __disable_irq(); }

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 * @note    Interrupt sources above kernel level remains enabled.
 */
static inline void port_suspend(void) { __disable_irq(); }

/**
 * @brief   Enables all the interrupt sources.
 */
static inline void port_enable(void) { __enable_irq(); }

/**
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 */
static inline void port_wait_for_interrupt(void) {
#if PORT_RISCV_ENABLE_WFI_IDLE
  __WFI();
#endif
}

/**
 * @brief   Returns the current value of the realtime counter.
 *
 * @return  The realtime counter value.
 */
static inline rtcnt_t port_rt_get_counter_value(void) {
  return (rtcnt_t)__get_rv_cycle();
}

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module late inclusions.                                                   */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

#if CH_CFG_ST_TIMEDELTA > 0
#include "chcore_timer.h"
#endif /* CH_CFG_ST_TIMEDELTA > 0 */

#endif /* !defined(_FROM_ASM_) */

#endif /* CHCORE_H */

/** @} */

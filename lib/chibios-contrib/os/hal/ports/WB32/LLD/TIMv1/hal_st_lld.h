/*
    Copyright (C) 2021 Westberry Technology (ChangZhou) Corp., Ltd

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    TIMv1/hal_st_lld.h
 * @brief   ST Driver subsystem low level driver header.
 * @details This header is designed to be include-able without having to
 *          include other files from the HAL.
 *
 * @addtogroup ST
 * @{
 */

#ifndef HAL_ST_LLD_H
#define HAL_ST_LLD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/* Feature currently disabled.*/
#define WB32_ST_ENFORCE_ALARMS 1

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   SysTick timer IRQ priority.
 */
#if !defined(WB32_ST_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define WB32_ST_IRQ_PRIORITY                   8
#endif

/**
 * @brief   TIMx unit (by number) to be used for free running operations.
 * @note    You must select a 32 bits timer if a 32 bits @p systick_t type
 *          is required.
 * @note    Timers 2, 3, 4, 5, 21 and 22 are supported.
 */
#if !defined(WB32_ST_USE_TIMER) || defined(__DOXYGEN__)
#define WB32_ST_USE_TIMER                      2
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/* This has to go after transition to shared handlers is complete for all
   platforms.*/
#if !defined(WB32_HAS_TIM2)
#define WB32_HAS_TIM2                          FALSE
#endif

#if !defined(WB32_HAS_TIM3)
#define WB32_HAS_TIM3                          FALSE
#endif

#if !defined(WB32_HAS_TIM4)
#define WB32_HAS_TIM4                          FALSE
#endif

/**/

#if OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING

#if WB32_ST_USE_TIMER == 2

#if defined(WB32_TIM2_IS_USED)
#error "ST requires TIM2 but the timer is already used"
#else
#define WB32_TIM2_IS_USED
#endif

#if defined(WB32_TIM2_SUPPRESS_ISR)
#define WB32_SYSTICK_SUPPRESS_ISR
#endif

#define WB32_ST_TIM                            WB32_TIM2
#define ST_LLD_NUM_ALARMS                      WB32_TIM2_CHANNELS
#define WB32_ST_USE_SYSTICK                    FALSE
#define WB32_ST_USE_TIM2                       TRUE
#define WB32_ST_USE_TIM3                       FALSE
#define WB32_ST_USE_TIM4                       FALSE


#elif WB32_ST_USE_TIMER == 3

#if defined(WB32_TIM3_IS_USED)
#error "ST requires TIM3 but the timer is already used"
#else
#define WB32_TIM3_IS_USED
#endif

#if defined(WB32_TIM3_SUPPRESS_ISR)
#define WB32_SYSTICK_SUPPRESS_ISR
#endif

#define WB32_ST_TIM                            WB32_TIM3
#define ST_LLD_NUM_ALARMS                      WB32_TIM3_CHANNELS
#define WB32_ST_USE_SYSTICK                    FALSE
#define WB32_ST_USE_TIM2                       FALSE
#define WB32_ST_USE_TIM3                       TRUE
#define WB32_ST_USE_TIM4                       FALSE

#elif WB32_ST_USE_TIMER == 4

#if defined(WB32_TIM4_IS_USED)
#error "ST requires TIM4 but the timer is already used"
#else
#define WB32_TIM4_IS_USED
#endif

#if defined(WB32_TIM4_SUPPRESS_ISR)
#define WB32_SYSTICK_SUPPRESS_ISR
#endif

#define WB32_ST_TIM                            WB32_TIM4
#define ST_LLD_NUM_ALARMS                      WB32_TIM4_CHANNELS
#define WB32_ST_USE_SYSTICK                    FALSE
#define WB32_ST_USE_TIM2                       FALSE
#define WB32_ST_USE_TIM3                       FALSE
#define WB32_ST_USE_TIM4                       TRUE

#else
#error "WB32_ST_USE_TIMER specifies an unsupported timer"
#endif

#if defined(WB32_ST_ENFORCE_ALARMS)

#if (WB32_ST_ENFORCE_ALARMS < 1) || (WB32_ST_ENFORCE_ALARMS > ST_LLD_NUM_ALARMS)
#error "invalid WB32_ST_ENFORCE_ALARMS value"
#endif

#undef ST_LLD_NUM_ALARMS
#define ST_LLD_NUM_ALARMS                      WB32_ST_ENFORCE_ALARMS
#endif

#elif OSAL_ST_MODE == OSAL_ST_MODE_PERIODIC

#define WB32_ST_USE_SYSTICK                    TRUE
#define WB32_ST_USE_TIM2                       FALSE
#define WB32_ST_USE_TIM3                       FALSE
#define WB32_ST_USE_TIM4                       FALSE

#else

#define WB32_ST_USE_SYSTICK                    FALSE
#define WB32_ST_USE_TIM2                       FALSE
#define WB32_ST_USE_TIM3                       FALSE
#define WB32_ST_USE_TIM4                       FALSE

#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void st_lld_init(void);
  void st_lld_serve_interrupt(void);
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Driver inline functions.                                                  */
/*===========================================================================*/


#if (OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING) || defined(__DOXYGEN__)

/**
 * @brief   Returns the time counter value.
 *
 * @return              The counter value.
 *
 * @notapi
 */
static inline systime_t st_lld_get_counter(void) {

  return (systime_t)WB32_ST_TIM->CNT;
}

/**
 * @brief   Starts the alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 *
 * @param[in] abstime   the time to be set for the first alarm
 *
 * @notapi
 */
static inline void st_lld_start_alarm(systime_t abstime) {

  WB32_ST_TIM->CCR[0] = (uint32_t)abstime;
  WB32_ST_TIM->SR     = 0;
#if ST_LLD_NUM_ALARMS == 1
  WB32_ST_TIM->DIER   = WB32_TIM_DIER_CC1IE;
  WB32_ST_TIM->CCER   = WB32_TIM_CCER_CC1E;
#else
  WB32_ST_TIM->DIER  |= WB32_TIM_DIER_CC1IE;
  WB32_ST_TIM->CCER  |= WB32_TIM_CCER_CC1E;
#endif
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
static inline void st_lld_stop_alarm(void) {

#if ST_LLD_NUM_ALARMS == 1
  WB32_ST_TIM->DIER = 0U;
  WB32_ST_TIM->CCER = 0U;
#else
  WB32_ST_TIM->DIER &= ~WB32_TIM_DIER_CC1IE;
  WB32_ST_TIM->CCER &= ~WB32_TIM_CCER_CC1E;
#endif
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] abstime   the time to be set for the next alarm
 *
 * @notapi
 */
static inline void st_lld_set_alarm(systime_t abstime) {

  WB32_ST_TIM->CCR[0] = (uint32_t)abstime;
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
static inline systime_t st_lld_get_alarm(void) {

  return (systime_t)WB32_ST_TIM->CCR[0];
}

/**
 * @brief   Determines if the alarm is active.
 *
 * @return              The alarm status.
 * @retval false        if the alarm is not active.
 * @retval true         is the alarm is active
 *
 * @notapi
 */
static inline bool st_lld_is_alarm_active(void) {

  return (bool)((WB32_ST_TIM->DIER & WB32_TIM_DIER_CC1IE) != 0);
}

#if (ST_LLD_NUM_ALARMS > 1) || defined(__DOXYGEN__)
/**
 * @brief   Starts an alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 * @note    This functionality is only available in free running mode, the
 *          behavior in periodic mode is undefined.
 *
 * @param[in] abstime   the time to be set for the first alarm
 * @param[in] alarm     alarm channel number
 *
 * @notapi
 */
static inline void st_lld_start_alarm_n(unsigned alarm, systime_t abstime) {

  WB32_ST_TIM->CCR[alarm] = (uint32_t)abstime;
  WB32_ST_TIM->SR         = 0;
  WB32_ST_TIM->DIER      |= (WB32_TIM_DIER_CC1IE << alarm);
  WB32_ST_TIM->CCER      |= (WB32_TIM_CCER_CC1E << (alarm * 4));
}

/**
 * @brief   Stops an alarm interrupt.
 * @note    This functionality is only available in free running mode, the
 *          behavior in periodic mode is undefined.
 *
 * @param[in] alarm     alarm channel number
 *
 * @notapi
 */
static inline void st_lld_stop_alarm_n(unsigned alarm) {

  WB32_ST_TIM->DIER &= ~(WB32_TIM_DIER_CC1IE << alarm);
  WB32_ST_TIM->CCER &= ~(WB32_TIM_CCER_CC1E << (alarm * 4));
}

/**
 * @brief   Sets an alarm time.
 * @note    This functionality is only available in free running mode, the
 *          behavior in periodic mode is undefined.
 *
 * @param[in] alarm     alarm channel number
 * @param[in] abstime   the time to be set for the next alarm
 *
 * @notapi
 */
static inline void st_lld_set_alarm_n(unsigned alarm, systime_t abstime) {

  WB32_ST_TIM->CCR[alarm] = (uint32_t)abstime;
}

/**
 * @brief   Returns an alarm current time.
 * @note    This functionality is only available in free running mode, the
 *          behavior in periodic mode is undefined.
 *
 * @param[in] alarm     alarm channel number
 * @return              The currently set alarm time.
 *
 * @notapi
 */
static inline systime_t st_lld_get_alarm_n(unsigned alarm) {

  return (systime_t)WB32_ST_TIM->CCR[alarm];
}

/**
 * @brief   Determines if an alarm is active.
 *
 * @param[in] alarm     alarm channel number
 * @return              The alarm status.
 * @retval false        if the alarm is not active.
 * @retval true         is the alarm is active
 *
 * @notapi
 */
static inline bool st_lld_is_alarm_active_n(unsigned alarm) {

  return (bool)((WB32_ST_TIM->DIER & (WB32_TIM_DIER_CC1IE << alarm)) != 0);
}
#endif /* ST_LLD_NUM_ALARMS > 1 */

#endif /* OSAL_ST_MODE == OSAL_ST_MODE_FREERUNNING */

#endif /* HAL_ST_LLD_H */

/** @} */

/* 
 * File:   ccp1_hal.h
 * Author: Kareem Taha
 *
 * Created on November 19, 2022, 3:25 PM
 */

#ifndef CCP1_HAL_H
#define	CCP1_HAL_H


 /* Section : Includes */
#include "../hal_gpio.h"
#include "../std_types.h"
#include "pic18f4620.h"
#include "../Interrupt/hal_internal_interrupt.h"
#include "ccp1_hal_cfg.h"
#include "../TIMER2/timer2.h"
 /* Section : Macros Definition */

/* CCP1 Capture Mode State*/
#define CCP1_CAPTURE_NOT_READY      0x00
#define CCP1_CAPTURE_READY          0x01


/* CCP1 Compare Mode State*/
#define CCP1_COMPARE_NOT_READY      0x00
#define CCP1_COMPARE_READY          0x01


/*CCP1M Register Select MODE*/
#define CCP1_MODULE_DISABLE                  (uint8)0x00
#define CCP1_CAPTURE_MODE_1_FALLING_EDGE     (uint8)0x04
#define CCP1_CAPTURE_MODE_1_RAISING_EDGE     (uint8)0x05
#define CCP1_CAPTURE_MODE_4_RAISING_EDGE     (uint8)0x06
#define CCP1_CAPTURE_MODE_16_RAISING_EDGE    (uint8)0x07
#define CCP1_COMPARE_MODE_SET_PIN_LOW        (uint8)0x08
#define CCP1_COMPARE_MODE_SET_PIN_HIGH       (uint8)0x09
#define CCP1_COMPARE_MODE_TOGGLE_ON_MATCH    (uint8)0x02
#define CCP1_COMPARE_MODE_GEN_SW_INTERRUPT   (uint8)0x0A
#define CCP1_COMPARE_MODE_GEN_SW_EVENT       (uint8)0x0B
#define CCP1_PWM_MODE                        (uint8)0x0C


 /* Section : Macros Functions Declaration */


#define CCP1_SET_MODE(_CONFIG)              (CCP1CONbits.CCP1M = _CONFIG)
 /* Section : Data Types Declaration */


typedef enum{
    CCP1_PWM_MODE_ = 0,
    CCP1_CAPTURE_MODE,
    CCP1_COMPARE_MODE,
}ccp1_mode_t;


typedef union{
    struct{
        uint8 ccpr1_low;
        uint8 ccpr1_high;
    };
    struct{
        uint16 ccpr1_16bit;
    };
}ccp1_reg_t;

typedef struct {
    ccp1_mode_t ccp1_mode;
    uint8 ccp1_mode_varient;
    pin_config_t pin_ccp1;
    #if CCP1_INTERRUPT_FEATURE_ENABLE == Interrupt_Feature_Enable
    void(*CCP1_InterruptHandler)(void);
    Interrupt_Priority_cfg priority;
    #endif 
    #if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_PWM_MODE)
    uint32 PWM_Frequency;
    uint8 timer2_postscaler: 4; 
    uint8 timer2_prescaler: 2; 
    #endif
            
}ccp1_t;

 /* Section : Function Declaration */

STD_ReturnType CCP1_Init(const ccp1_t *_ccp1_object);
STD_ReturnType CCP1_DeInit(const ccp1_t *_ccp1_object);

#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_PWM_MODE)
STD_ReturnType CCP1_PWM_Set_Duty(const uint8 _duty);
STD_ReturnType CCP1_PWM_Start(void);
STD_ReturnType CCP1_PWM_Stop(void);
#endif


#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_CAPTURE_MODE)
STD_ReturnType CCP1_IsCaptureDataReady(uint8 *capture_status);
STD_ReturnType CCP1_CaptureMode_Read_Value(uint16 *capture_value);
#endif



#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_COMPARE_MODE)
STD_ReturnType CCP1_IsCompareCompelete(uint8 *compare_status);
STD_ReturnType CCP1_CompareMode_set_Value(uint16 compare_value);
#endif



#endif	/* CCP1_HAL_H */


/* 
 * File:   ccp1.c
 * Author: Kareem Taha
 *
 * Created on November 19, 2022, 3:25 PM
 */



 /* Section : Includes */
#include "ccp1_hal.h"

 /* Section : Macros Definition */

 /* Section : Macros Functions Declaration */

 /* Section : Data Types Declaration */

 /* Section : Function Declaration */
#if CCP1_INTERRUPT_FEATURE_ENABLE == Interrupt_Feature_Enable
static void(*CCP1_InterruptHandler)(void) = NULL;
#endif

STD_ReturnType CCP1_Init(const ccp1_t *_ccp1_object){
    STD_ReturnType ret = E_OK;
    if(_ccp1_object == NULL) ret = E_NOT_OK;
    else{
        /*Disable CCP1*/
        CCP1CONbits.CCP1M = CCP1_MODULE_DISABLE;
        /*Initialize Pin of ccp1*/
        gpio_pin_direction_init(&(_ccp1_object->pin_ccp1));
        /*Enable CCP1 with Configuration*/
        if(_ccp1_object->ccp1_mode == CCP1_PWM_MODE_){
            #if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_PWM_MODE)
            CCP1CONbits.CCP1M = CCP1_PWM_MODE;
            /*initialize PWM FREQ*/
            PR2 = (uint8)((_XTAL_FREQ / ((_ccp1_object->PWM_Frequency) * 4.0 * (_ccp1_object->timer2_postscaler) *
                    (_ccp1_object->timer2_prescaler))) - 1);
            #endif
        }
        else if(_ccp1_object->ccp1_mode == CCP1_CAPTURE_MODE){
            switch(_ccp1_object->ccp1_mode_varient){
                case CCP1_CAPTURE_MODE_1_FALLING_EDGE:
                    CCP1CONbits.CCP1M = CCP1_CAPTURE_MODE_1_FALLING_EDGE;
                    break;
                case CCP1_CAPTURE_MODE_1_RAISING_EDGE:
                    CCP1CONbits.CCP1M = CCP1_CAPTURE_MODE_1_RAISING_EDGE;
                    break;
                case CCP1_CAPTURE_MODE_4_RAISING_EDGE:
                    CCP1CONbits.CCP1M = CCP1_CAPTURE_MODE_4_RAISING_EDGE;
                    break;
                case CCP1_CAPTURE_MODE_16_RAISING_EDGE:
                    CCP1CONbits.CCP1M = CCP1_CAPTURE_MODE_16_RAISING_EDGE;
                    break;
                default:
                    break;
            }
        }
        else if(_ccp1_object->ccp1_mode == CCP1_COMPARE_MODE){
            switch(_ccp1_object->ccp1_mode_varient){
                case CCP1_COMPARE_MODE_SET_PIN_LOW:
                    CCP1CONbits.CCP1M = CCP1_COMPARE_MODE_SET_PIN_LOW;
                    break;
                case CCP1_COMPARE_MODE_SET_PIN_HIGH:
                    CCP1CONbits.CCP1M = CCP1_COMPARE_MODE_SET_PIN_HIGH;
                    break;
                case CCP1_COMPARE_MODE_TOGGLE_ON_MATCH:
                    CCP1CONbits.CCP1M = CCP1_COMPARE_MODE_TOGGLE_ON_MATCH;
                    break;
                case CCP1_COMPARE_MODE_GEN_SW_INTERRUPT:
                    CCP1CONbits.CCP1M = CCP1_COMPARE_MODE_GEN_SW_INTERRUPT;
                    break;
                case CCP1_COMPARE_MODE_GEN_SW_EVENT:
                    CCP1CONbits.CCP1M = CCP1_COMPARE_MODE_GEN_SW_EVENT;
                    break; 
                default:
                    break;
            }
        }
        /*interrupt configuration*/
        #if CCP1_INTERRUPT_FEATURE_ENABLE == Interrupt_Feature_Enable
        CCP1_Interrupt_Enable();
        CCP1_Interrupt_Flag_Clear();
        CCP1_InterruptHandler = (_ccp1_object->CCP1_InterruptHandler);
        #if Interrupt_Priority_Levels_Enable == Interrupt_Feature_Enable
        Interrupt_PriorityLevelsEnable();
        if(_timer->priority == INTERRUPT_High_Priority){
            Interrupt_GlobalInterruptHighEnable();
            CCP1_Interrupt_Prior ity_High();
        }
        else{
            Interrupt_GlobalInterruptLowEnable();
            CCP1_Interrupt_Priority_Low();
        }
        #else
        /*Enable Global Interrupt */
        Interrupt_GlobalInterruptEnable();
        /*Enable Peripherals Interrupt */
        Interrupt_PeripheralInterruptEnable();
        #endif
        #endif
    }
    return ret;
}

STD_ReturnType CCP1_DeInit(const ccp1_t *_ccp1_object){
    STD_ReturnType ret = E_OK;
    if(_ccp1_object == NULL) ret = E_NOT_OK;
    else{
        /*Disable CCP1*/
        CCP1CONbits.CCP1M = CCP1_MODULE_DISABLE;
        #if CCP1_INTERRUPT_FEATURE_ENABLE == Interrupt_Feature_Enable
        CCP1_Interrupt_Disable();
        #endif
    }
    return ret;  
}

#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_PWM_MODE)
STD_ReturnType CCP1_PWM_Set_Duty(const uint8 _duty){
    STD_ReturnType ret = E_OK;
    uint16 l_duty_temp = 0;
    l_duty_temp = (uint16)((PR2 + 1) * (_duty / 100.0) * 4.0);
    CCP1CONbits.DC1B = (uint8)(l_duty_temp & 0x0003);
    CCPR1L = (uint8)(l_duty_temp >> 2);
    return ret;
}
STD_ReturnType CCP1_PWM_Start(void){
    STD_ReturnType ret = E_OK;
    CCP1CONbits.CCP1M = CCP1_PWM_MODE;
    return ret;
}
STD_ReturnType CCP1_PWM_Stop(void){
    STD_ReturnType ret = E_OK;
    CCP1CONbits.CCP1M = CCP1_MODULE_DISABLE;
    return ret;
}
#endif


#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_CAPTURE_MODE)
STD_ReturnType CCP1_IsCaptureDataReady(uint8 *capture_status){
    STD_ReturnType ret = E_OK;
    if(capture_status == NULL) ret = E_NOT_OK;
    else{
        if(PIR1bits.CCP1IF == CCP1_CAPTURE_READY){
            CCP1_Interrupt_Flag_Clear();
            *capture_status = CCP1_CAPTURE_READY;
            
        }else{
            *capture_status = CCP1_CAPTURE_NOT_READY;
        }
    }
    return ret;
}
STD_ReturnType CCP1_CaptureMode_Read_Value(uint16 *capture_value){
    STD_ReturnType ret = E_OK;
    ccp1_reg_t capture_temporary_values = {.ccpr1_low = 0, .ccpr1_high = 0};
    if(capture_value == NULL) ret = E_NOT_OK;
    else{
        capture_temporary_values.ccpr1_low = CCPR1L;
        capture_temporary_values.ccpr1_high = CCPR1H;
        *capture_value = capture_temporary_values.ccpr1_16bit;
    }
    return ret;
}
#endif



#if(CCP1_CFG_MODE_SELECTED == CCP1_CFG_COMPARE_MODE)
STD_ReturnType CCP1_IsCompareCompelete(uint8 *compare_status){
    STD_ReturnType ret = E_OK;
    if(compare_status == NULL) ret = E_NOT_OK;
    else{
        if(PIR1bits.CCP1IF == CCP1_COMPARE_READY){
            CCP1_Interrupt_Flag_Clear();
            *compare_status = CCP1_COMPARE_READY;
            
        }else{
            *compare_status = CCP1_COMPARE_NOT_READY;
        }
    }
    return ret;
}
STD_ReturnType CCP1_CompareMode_set_Value(uint16 compare_value){
    STD_ReturnType ret = E_OK;
    ccp1_reg_t capture_temporary_values = {.ccpr1_low = 0, .ccpr1_high = 0};
    capture_temporary_values.ccpr1_16bit = compare_value;
    CCPR1L = capture_temporary_values.ccpr1_low;
    CCPR1H = capture_temporary_values.ccpr1_high;
    return ret;
}
#endif

void CCP1_ISR(void){
    CCP1_Interrupt_Flag_Clear();
    if(CCP1_InterruptHandler){
        CCP1_InterruptHandler();
    }
}
/* 
 * File:   ADC.h
 * Author: GEII Robot
 *
 * Created on 4 octobre 2023, 09:39
 */

#ifndef ADC_H
#define	ADC_H

void InitADC1(void);
void ADC1StartConversionSequence();
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void);
unsigned int * ADCGetResult(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);


#endif	/* ADC_H */


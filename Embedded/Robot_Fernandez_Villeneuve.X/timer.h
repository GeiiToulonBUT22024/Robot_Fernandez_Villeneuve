#ifndef TIMER_H
#define TIMER_H


void InitTimer23(void);
void InitTimer1(void);
void InitTimer4(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void);
void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);

extern unsigned long timestamp;

#endif /* TIMER_H */

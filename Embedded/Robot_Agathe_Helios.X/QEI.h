/* 
 * File:   QEI.h
 * Author: GEII Robot
 *
 * Created on 29 janvier 2024, 14:21
 */

#ifndef QEI_H
#define	QEI_H

#define DISTROUES 0.2812
#define FREQ_ECH_QEI 250

void InitQEI1();
void InitQEI2();
void QEIUpdateData();
void SendPositionData();


#endif	/* QEI_H */


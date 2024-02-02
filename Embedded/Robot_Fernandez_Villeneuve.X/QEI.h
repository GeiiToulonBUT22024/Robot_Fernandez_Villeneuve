/* 
 * File:   QEI.h
 * Author: GEII Robot
 *
 * Created on 29 janvier 2024, 14:21
 */

#ifndef QEI_H
#define	QEI_H

void InitQEI1();
void InitQEI2();
void QEIUpdateData();
void SendPositionData();

float QeiDroitPosition_T_1;
float QeiGauchePosition_T_1;
float QeiDroitPosition;
float QeiGauchePosition;
float delta_d;
float delta_g;
float FREQ_ECH_QEI = 0.004;

#endif	/* QEI_H */


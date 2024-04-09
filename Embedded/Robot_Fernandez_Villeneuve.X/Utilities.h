/* 
 * File:   Utilities.h
 * Author: GEII Robot
 *
 * Created on 29 janvier 2024, 16:25
 */

#ifndef UTILITIES_H
#define	UTILITIES_H

#define PI 3.141592653589793

double Abs(double value);
double Max(double value, double value2);
double Min(double value, double value2);
float LimitToInterval(float value, float min, float max);
double Modulo2PIAngleRadian(double angleRadian);
float getFloat(unsigned char *p, int index);
double getDouble(unsigned char *p, int index);
void getBytesFromFloat(unsigned char *p, int index, float f);
void getBytesFromInt32(unsigned char *p, int index, long in);
void getBytesFromDouble(unsigned char *p, int index, double d);
double ModuloByAngle(double angleVoulu , double angleActuel);

#endif /*UTILITIES_H*/


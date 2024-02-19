#include "robot.h"
#include "PWM.h"
#include "IO.h"
#include "Toolbox.h"
#include "Utilities.h"

volatile ROBOT_STATE_BITS robotState;
#define PWMPER 40.0

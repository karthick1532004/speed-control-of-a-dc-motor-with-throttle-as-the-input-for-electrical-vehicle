#include "DSP28x_Project.h"

void motor_drive_gpio(void)
{
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;  // Set as output
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioDataRegs.GPASET.bit.GPIO19 = 1;  // Set GPIO19 high
    GpioDataRegs.GPASET.bit.GPIO2 = 0;   // Set GPIO2 low
}

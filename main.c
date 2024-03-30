#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// Prototype statements for functions found within this file.
__interrupt void adc_isr(void);
void Adc_Config(void);
void pwm_setup(void);
//void motordriver_setup(void);
void
delay_ms1()
{
    double j;
       for (j = 0; j < 20000; j++)
       {

       }
}

// Global variables used in this example:
Uint16 LoopCount;
Uint16 ConversionCount;
Uint16 Voltage1[10];
Uint16 Voltage2[10];
Uint16 j;


int adc()
{

   InitSysCtrl();
   DINT;
   InitPieCtrl();
   IER = 0x0000;
   IFR = 0x0000;
   InitPieVectTable();
   EALLOW;  // This is needed to write to EALLOW protected register
   pwm_setup();
   PieVectTable.ADCINT1 = &adc_isr;
      // This is needed to disable write to EALLOW protected registers
   InitAdc();  // For this example, init the ADC
   motor_drive_gpio();
   AdcOffsetSelfCal();
   //motordriver_setup();
   //GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
   //GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
   //GpioDataRegs.GPASET.bit.GPIO2 = 1;
   //GpioDataRegs.GPASET.bit.GPIO3 = 0;
   EDIS;

// Step 5. User specific code, enable interrupts:

// Enable ADCINT1 in PIE
   PieCtrlRegs.PIEIER1.bit.INTx1 = 1;   // Enable INT 1.1 in the PIE
   IER |= M_INT1;                       // Enable CPU Interrupt 1
   EINT;                                // Enable Global interrupt INTM
   ERTM;                                // Enable Global realtime interrupt DBGM

   LoopCount = 0;
   ConversionCount = 0;

// Configure ADC
    EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;  // Enable non-overlap mode
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;    // ADCINT1 trips after AdcResults latch
    AdcRegs.INTSEL1N2.bit.INT1E     = 1;    // Enabled ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;    // Disable ADCINT1 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 1;    // setup EOC1 to trigger ADCINT1 to fire
    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 4;    // set SOC0 channel select to ADCINA4
    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 2;    // set SOC1 channel select to ADCINA2
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;    // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;    // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    EDIS;

// Assumes ePWM1 clock is already enabled in InitSysCtrl();
   EPwm1Regs.ETSEL.bit.SOCAEN   = 1;        // Enable SOC on A group
   EPwm1Regs.ETSEL.bit.SOCASEL  = 4;        // Select SOC from CMPA on upcount
   EPwm1Regs.ETPS.bit.SOCAPRD   = 1;        // Generate pulse on 1st event
   EPwm1Regs.CMPA.half.CMPA     = 0x0080;   // Set compare A value
   EPwm1Regs.TBPRD              = 0xFFFF;   // Set period for ePWM1
   EPwm1Regs.TBCTL.bit.CTRMODE  = 0;        // count up and start

// Wait for ADC interrupt
   for(;;)
   {
      LoopCount++;
      j= Voltage1[ConversionCount]/8;
      EPwm3Regs.CMPA.half.CMPA=j;
      delay_ms1();
   }

}


__interrupt void  adc_isr(void)
{

  Voltage1[ConversionCount] = AdcResult.ADCRESULT0;
  Voltage2[ConversionCount] = AdcResult.ADCRESULT1;

  // If 20 conversions have been logged, start over
  if(ConversionCount == 9)
  {
     ConversionCount = 0;
  }
  else ConversionCount++;

  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     //Clear ADCINT1 flag reinitialize for next SOC
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

  return;
}
void pwm_setup(){
            GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;//1 is given to act the pin as the pwm
            GpioCtrlRegs.GPADIR.bit.GPIO4 = 1; // 1 is given to act the pin as the output

            //intialization of pwm
            EPwm3Regs.TBPRD = 800; // Period = 601 TBCLK counts
            //EPwm3Regs.CMPA.half.CMPA = 100; // Compare A = 350 TBCLK counts
            EPwm3Regs.CMPB = 200; // Compare B = 200 TBCLK counts
            //EPwm1Regs. = 0; // Set Phase register to zero
            EPwm3Regs.TBCTR = 0;
            EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
            EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;
            EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
            EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
            EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLK
            EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
            EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
            EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
            EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
            EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero
            EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;
            EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
            EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;
            EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;
}
//void motordriver_setup(){
           // GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
           // GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
           // GpioDataRegs.GPASET.bit.GPIO2 = 1;
           // GpioDataRegs.GPASET.bit.GPIO3 = 1;

//}

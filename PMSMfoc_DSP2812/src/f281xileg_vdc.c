/* ==================================================================================
File name:       F281XILEG_VDC.C
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: This file contains source for the F281X Two leg current measurement 
and DC-bus measurement driver.

Target: TMS320F281x family

=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#include "DSP281x_Device.h"
#include "f281xileg_vdc.h"

#define CPU_CLOCK_SPEED      6.6667L   // for a 150MHz CPU clock speed
#define ADC_usDELAY 5000L
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)

extern void DSP28x_usDelay(unsigned long Count);

void F281X_ileg2_dcbus_drv_init(ILEG2DCBUSMEAS *p)
{

    DELAY_US(ADC_usDELAY);						//��ʱ5us	
  
    AdcRegs.ADCTRL1.all = ADC_RESET_FLAG; 		//��λADCģ�� 
	asm(" NOP ");
	asm(" NOP ");    

    AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;		// Power up bandgap/reference circuitry
	DELAY_US(ADC_usDELAY);			    		// Delay before powering up rest of ADC 
    
    AdcRegs.ADCTRL3.bit.ADCPWDN = 1;	   		// Power up rest of ADC
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 6;     		//ADCLK=12.5MHZ 
	DELAY_US(ADC_usDELAY);	

    AdcRegs.ADCTRL1.all |= ADCTRL1_INIT_STATE;	//������𱻺��ԣ��ɼ�����Ϊ2��ADCLK��������ADʱ�Ӳ��ٽ��ж���Ƶ��;����ģʽ 
    AdcRegs.ADCTRL2.all |= ADCTRL2_INIT_STATE; 	//����EVA��������ģʽ��SEQ
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1|= 0x0003; //��4��ת��ͨ��
    AdcRegs.ADCCHSELSEQ1.all = p->ChSelect;     //Ia:ͨ��0��Ib:ͨ��1��Ic��ͨ��2��vsense:ͨ��3

	EvaRegs.GPTCONA.bit.T1TOADC = 1;      		//��T1�������ж�����ADC
}  

void F281X_ileg2_dcbus_drv_read(ILEG2DCBUSMEAS *p)
{
       int16 DatQ15;
       int32 Tmp;

        // Wait until ADC conversion is completed
        while (AdcRegs.ADCST.bit.SEQ1_BSY == 1)
        {};
		
		//Ia,����,����������,-1��+1����25A��25A
        DatQ15 = AdcRegs.ADCRESULT0^0x8000;       // ^��λ���,��ת�����ת��ΪQ15��ʽ��˫��������
        Tmp = (int32)p->ImeasAGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        p->ImeasA = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->ImeasA += p->ImeasAOffset;             // Add offset
        p->ImeasA *= -1;                   	  // ����,����������,-1��+1����25A��25A
		
		//Ib,����,����������,-1��+1����25A��25A
        DatQ15 = AdcRegs.ADCRESULT1^0x8000;   	  // Convert raw result to Q15 (bipolar signal)
        Tmp = (int32)p->ImeasBGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        p->ImeasB = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->ImeasB += p->ImeasBOffset;             // Add offset
        p->ImeasB *= -1;                   		  // Positive direction, current flows to motor
 		
		//Ic,����,����������,-1��+1����25A��25A,�Ǽ�������ģ�������ADCRESULT3������
		p->ImeasC = -(p->ImeasA + p->ImeasB);      // Compute phase-c current
		
		//VSENSE,0����0V,1����5V
        DatQ15 = (AdcRegs.ADCRESULT3>>1)&0x7FFF;   // ת��ΪQ15�����ԣ���ΧΪ[0,1)
        Tmp = (int32)p->VdcMeasGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        if (Tmp > 0x0FFFFFFF)                      // Limit Tmp to 1.0 in Q28
           Tmp = 0x0FFFFFFF;
        p->VdcMeas = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->VdcMeas += p->VdcMeasOffset;            // Add offset

        

        AdcRegs.ADCTRL2.all |= 0x4040;             // Reset the sequence

}            
          

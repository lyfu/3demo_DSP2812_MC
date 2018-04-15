/* =================================================================================
File name:        BUILD.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Incremental Build Level control file.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
=================================================================================  */

#ifndef BUILDLEVEL 

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// SVGEN_DQ and FC_PWM_DRV tests 
#define LEVEL2  2           // Currents measurement test 
#define LEVEL3  3           // Two current PI regulator tests 
#define LEVEL4  4           // QEP test and angle calibration 
#define LEVEL5  5           // Speed closed loop using measured speed based QEP

/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL4


#ifndef BUILDLEVEL    
#error  Critical: BUILDLEVEL must be defined !!
#endif  // BUILDLEVEL

#endif  // BUILDLEVEL

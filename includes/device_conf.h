//---------------------------------------------------------------

#ifndef DEVICE_CONF_H__
#define DEVICE_CONF_H__

#define Output1          0x00000000
#define Output2          0x01000000
#define Output3          0x02000000
#define Output4          0x03000000

#define Input1           0x00
#define Input2           0x01
#define Input3           0x02
#define Input4           0x03
#define Input5           0x04
#define Input6           0x05

#define Input7           0x06
#define Input8           0x07
#define Input9           0x08
#define Input10          0x09
#define Input11          0x10
#define Input12          0x11
#define Input13          0x12
#define Input14          0x13

//--------------------------------------------------------------

#define KM1 	        Output3
#define KM2		        Output4
#define LB_OUT1         Output2
//#define LB_OUT2       OutBoard_Output6

#define DO_STATE_HIGH           (CTRL_GPIO|0x00010000U)    /* ioctl操作的种类，置位DO相应端口*/
#define DO_STATE_LOW            (CTRL_GPIO|0x00000000U)    /* ioctl操作的种类，将DO相应端口置0*/

//----------------------------------------------------------------

#define Grid_Contacter          (Input3)  //Input3
#define MGrid_Contacter         (Input4)  //Input4

//标准机器
//#define Grid_Exist               (DIN(5)==1) //电网有电
//#define Grid_Missed              (DIN(5)==0) //电网没电

//带VSG
//#define Grid_Exist               GRID_VOLTAGE_OK(System_Run_Info.PCS_AC_Voltage)//(GRID_VOLTAGE_OK((VSG_Info.Grid_voltage_Vab+VSG_Info.Grid_voltage_Vbc)/2)) //电网有电
//#define Grid_Missed              ZERO_VOLTAGE(System_Run_Info.PCS_AC_Voltage)//(ZERO_VOLTAGE((VSG_Info.Grid_voltage_Vab+VSG_Info.Grid_voltage_Vbc)/2)) //电网没电

#define Grid_Exist               (GRID_VOLTAGE_OK((Meter_Info[0].VAB+Meter_Info[0].VBC+Meter_Info[0].VCA)/3)) //电网有电
#define Grid_Missed              (ZERO_VOLTAGE((Meter_Info[0].VAB+Meter_Info[0].VBC+Meter_Info[0].VCA)/3)) //电网没电

#endif


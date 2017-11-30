#ifndef _EEPROM_ADDRESS_H_
#define _EEPROM_ADDRESS_H_

//eeprom地址存储map
#define EEPROM_ADDRESS_BASE	                   				(uint32_t)(100)

//系统参数地址
#define SYS_PARAM_ADDRESS                     			 	(uint32_t)(EEPROM_ADDRESS_BASE  + 0)

//时间存储
#define YEAR_LAST_ADDRESS					  				(uint32_t)(SYS_PARAM_ADDRESS 	+ 0)//1B
#define MONTH_LAST_ADDRESS					   				(uint32_t)(SYS_PARAM_ADDRESS 	+ 1)//1B
#define DATE_LAST_ADDRESS					   				(uint32_t)(SYS_PARAM_ADDRESS 	+ 2)//1B
#define HOUR_LAST_ADDRESS   		            			(uint32_t)(SYS_PARAM_ADDRESS 	+ 3)//1B
#define MINUTE_LAST_ADDRESS  		            			(uint32_t)(SYS_PARAM_ADDRESS 	+ 4)//1B
#define SECOND_LAST_ADDRESS  		           			 	(uint32_t)(SYS_PARAM_ADDRESS 	+ 5)//1B
#define MODBUS_ADDRESS_ADDRESS  		       				(uint32_t)(SYS_PARAM_ADDRESS	+ 7)//1B
#define BAUD_RATE_ADDRESS  		               				(uint32_t)(SYS_PARAM_ADDRESS 	+ 8)//1B
#define TOUCH_HARDWARE_EDITION_ADDRESS         				(uint32_t)(SYS_PARAM_ADDRESS 	+ 9)//1B
#define TOUCH_SOFTWARE_EDITION_ADDRESS         				(uint32_t)(SYS_PARAM_ADDRESS 	+ 10)//1B
#define WEBSERVER_LANGUAGE_ADDRESS             				(uint32_t)(SYS_PARAM_ADDRESS 	+ 11)//1B

#define FIRST_START_ADDRESS                    				(uint32_t)(SYS_PARAM_ADDRESS 	+ 30)//1B,初始上电标志
#define NET_IP_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 40)//1B
#define NET_IP_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 41)//1B
#define NET_IP_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 42)//1B
#define NET_IP_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 43)//1B
#define NET_SUBMASK_0_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 44)//1B
#define NET_SUBMASK_1_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 45)//1B
#define NET_SUBMASK_2_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 46)//1B
#define NET_SUBMASK_3_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 47)//1B
#define NET_GATEWAY_0_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 48)//1B
#define NET_GATEWAY_1_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 49)//1B
#define NET_GATEWAY_2_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 50)//1B
#define NET_GATEWAY_3_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 51)//1B
#define NET_DNS1_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 52)//1B
#define NET_DNS1_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 53)//1B
#define NET_DNS1_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 54)//1B
#define NET_DNS1_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 55)//1B
#define NET_DNS2_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 56)//1B
#define NET_DNS2_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 57)//1B
#define NET_DNS2_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 58)//1B
#define NET_DNS2_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 59)//1B
#define NET_DHCP_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 60)//1B
#define NET_PORT_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 61)//2B
#define NET2_IP_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 63)//1B
#define NET2_IP_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 64)//1B
#define NET2_IP_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 65)//1B
#define NET2_IP_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 66)//1B
#define NET2_SUBMASK_0_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 67)//1B
#define NET2_SUBMASK_1_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 68)//1B
#define NET2_SUBMASK_2_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 69)//1B
#define NET2_SUBMASK_3_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 70)//1B
#define NET2_GATEWAY_0_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 71)//1B
#define NET2_GATEWAY_1_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 72)//1B
#define NET2_GATEWAY_2_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 73)//1B
#define NET2_GATEWAY_3_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 74)//1B
#define NET2_DNS1_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 75)//1B
#define NET2_DNS1_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 76)//1B
#define NET2_DNS1_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 77)//1B
#define NET2_DNS1_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 78)//1B
#define NET2_DNS2_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 79)//1B
#define NET2_DNS2_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 80)//1B
#define NET2_DNS2_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 81)//1B
#define NET2_DNS2_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 82)//1B
#define NET2_DHCP_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 83)//1B
#define IEC104_ADDRESS_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS 	+ 90)//2B
#define IEC104_T1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 92)//2B
#define IEC104_T3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 94)//2B
#define IEC104_K_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 96)//2B
#define IEC104_W_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS 	+ 98)//2B
#define BMS_ADDRESS_ADDRESS  		           				(uint32_t)(SYS_PARAM_ADDRESS 	+ 100)//1B
#define BMS_TYPE_ADDRESS              		    			(uint32_t)(SYS_PARAM_ADDRESS 	+ 101)//1B
#define PCS_COMM_ADDRESS              		    			(uint32_t)(SYS_PARAM_ADDRESS 	+ 102)//1B
#define PCS_GRID_RUN_MODE_ADDRESS               			(uint32_t)(SYS_PARAM_ADDRESS 	+ 103)//1B
#define POWER_DISTRIBUTION_ENABLE_ADDRESS       			(uint32_t)(SYS_PARAM_ADDRESS 	+ 104)//1B
#define DCDC_TYPE_ADDRESS                       			(uint32_t)(SYS_PARAM_ADDRESS 	+ 105)//1B
#define METER_COMM_ENABLE_ADDRESS               			(uint32_t)(SYS_PARAM_ADDRESS 	+ 106)//1B
#define KP_ADDRESS                              			(uint32_t)(SYS_PARAM_ADDRESS 	+ 110)//2B
#define KI_ADDRESS                              			(uint32_t)(SYS_PARAM_ADDRESS	+ 112)//2B
#define BALANCING_KP_ADDRESS                    			(uint32_t)(SYS_PARAM_ADDRESS 	+ 114)//2B
#define BALANCING_ENABLE_ADDRESS                			(uint32_t)(SYS_PARAM_ADDRESS 	+ 116)//1B
#define PCS_ISLAND_CTRL_MODE_ADDRESS            			(uint32_t)(SYS_PARAM_ADDRESS 	+ 117)//1B
#define KP_PVLIMIT_ADDRESS                      			(uint32_t)(SYS_PARAM_ADDRESS 	+ 118)//2B
#define KI_PVLIMIT_ADDRESS                      			(uint32_t)(SYS_PARAM_ADDRESS 	+ 120)//2B
#define PV_COMM_ADDRESS              		    			(uint32_t)(SYS_PARAM_ADDRESS 	+ 122)//1B
#define Parameter_recovery_kp_ADDRESS          				(uint32_t)(SYS_PARAM_ADDRESS 	+ 123)//2B
#define Parameter_recovery_dalay_time_ADDRESS   			(uint32_t)(SYS_PARAM_ADDRESS 	+ 125)//2B

//运行参数地址
#define TOTAL_RUNTIME_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 150)//4B
#define TOTAL_MG_RUNTIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 154)//4B
#define TOTAL_GRID_RUNTIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 158)//4B
#define TOTAL_CHARGE_TIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 162)//4B
#define TOTAL_DISCHARGE_TIME_ADDRESS						(uint32_t)(SYS_PARAM_ADDRESS	+ 166)//4B
#define TOTAL_PV_RUNTIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 170)//4B
#define TOTAL_LOAD_SUPPLY_TIME_ADDRESS						(uint32_t)(SYS_PARAM_ADDRESS	+ 174)//4B
#define TOTAL_FAULT_TIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 178)//4B

#define DAY_RUNTIME_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 216)//2B
#define DAY_MG_RUNTIME_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 218)//2B
#define DAY_GRID_RUNTIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 220)//2B
#define DAY_CHARGE_TIME_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 222)//2B
#define DAY_DISCHARGE_TIME_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 224)//2B
#define DAY_PV_RUNTIME_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 226)//2B
#define DAY_LOAD_SUPPLY_TIME_ADDRESS						(uint32_t)(SYS_PARAM_ADDRESS	+ 228)//2B
#define DAY_FAULT_TIME_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 230)//2B

#define RUNMIN_ADDRESS						    			(uint32_t)(SYS_PARAM_ADDRESS	+ 236)//4B
#define MG_RUNMIN_ADDRESS					    			(uint32_t)(SYS_PARAM_ADDRESS	+ 240)//4B
#define GRID_RUNMIN_ADDRESS					    			(uint32_t)(SYS_PARAM_ADDRESS	+ 244)//4B
#define CHARGE_MIN_ADDRESS					    			(uint32_t)(SYS_PARAM_ADDRESS	+ 248)//4B
#define DISCHARGE_MIN_ADDRESS				    			(uint32_t)(SYS_PARAM_ADDRESS	+ 252)//4B
#define PV_RUNMIN_ADDRESS					    			(uint32_t)(SYS_PARAM_ADDRESS	+ 256)//4B
#define LOAD_SUPPLY_MIN_ADDRESS				    			(uint32_t)(SYS_PARAM_ADDRESS	+ 260)//4B
#define FAULT_MIN_ADDRESS					    			(uint32_t)(SYS_PARAM_ADDRESS	+ 264)//4B

#define TOTAL_CHARGE_ENERGY_ADDRESS			    			(uint32_t)(SYS_PARAM_ADDRESS	+ 268)//4B
#define TOTAL_DISCHARGE_ENERGY_ADDRESS          			(uint32_t)(SYS_PARAM_ADDRESS	+ 272)//4B
#define TOTAL_PV_ENERGY_ADDRESS				    			(uint32_t)(SYS_PARAM_ADDRESS	+ 276)//4B
#define TOTAL_LOAD_ENERGY_ADDRESS			    			(uint32_t)(SYS_PARAM_ADDRESS	+ 280)//4B

#define CHARGE_ENERGY_ACC_ADDRESS			    			(uint32_t)(SYS_PARAM_ADDRESS	+ 300)//8B
#define DISCHARGE_ENERGY_ACC_ADDRESS		    			(uint32_t)(SYS_PARAM_ADDRESS	+ 308)//8B
#define PV_ENERGY_ACC_ADDRESS				    			(uint32_t)(SYS_PARAM_ADDRESS	+ 316)//8B
#define LOAD_ENERGY_ACC_ADDRESS				    			(uint32_t)(SYS_PARAM_ADDRESS	+ 324)//8B

#define DAY_LOAD_ENERGY_ACC_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 348)//8B
#define E_LOAD_ADJUST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 356)//2B

#define DAY_CHARGE_ENERGY_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 370)//2B
#define DAY_DISCHARGE_ENERGY_ADDRESS						(uint32_t)(SYS_PARAM_ADDRESS	+ 372)//2B
#define DAY_PV_ENERGY_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 374)//2B
#define DAY_LOAD_ENERGY_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 376)//2B

//定义web界面显示状态
#define WEB_PAGE_PCS_ST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 380)//1B
#define WEB_PAGE_PV_ST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 381)//1B
#define WEB_PAGE_DCDC_ST_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 382)//1B
#define WEB_PAGE_PVS_ST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 383)//1B
#define WEB_PAGE_BMS_ST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 384)//1B
#define WEB_PAGE_AMMETER_ST_ADDRESS							(uint32_t)(SYS_PARAM_ADDRESS	+ 385)//1B
#define WEB_PAGE_REALTIMEMONITOR_ST_ADDRESS					(uint32_t)(SYS_PARAM_ADDRESS	+ 386)//1B
#define WEB_PAGE_BATTERYCONTROL_ST_ADDRESS					(uint32_t)(SYS_PARAM_ADDRESS	+ 387)//1B
#define WEB_PAGE_ENV_ST_ADDRESS								(uint32_t)(SYS_PARAM_ADDRESS	+ 388)//1B

#define NTP_IP_0_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 400)//1B
#define NTP_IP_1_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 401)//1B
#define NTP_IP_2_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 402)//1B
#define NTP_IP_3_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 403)//1B
#define NTP_PORT_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 404)//2B
#define NTP_ENABLE_ADDRESS									(uint32_t)(SYS_PARAM_ADDRESS	+ 406)//1B

#define OPERATOR_PASSWORD_ADDRESS             				(uint32_t)(SYS_PARAM_ADDRESS 	+ 460)//8B
#define ADMINISTRATOR_PASSWORD_ADDRESS            			(uint32_t)(SYS_PARAM_ADDRESS 	+ 470)//8B
#define MAINTRAINER_PASSWORD_ADDRESS            			(uint32_t)(SYS_PARAM_ADDRESS 	+ 480)//8B

#define PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS			(uint32_t)(SYS_PARAM_ADDRESS	+ 500)//2B
#define PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS			(uint32_t)(SYS_PARAM_ADDRESS	+ 502)//2B
#define PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS					(uint32_t)(SYS_PARAM_ADDRESS	+ 504)//2B

#define RAMP_RATE_CTRL_PV_RATED_POWER_ADDRESS               (uint32_t)(SYS_PARAM_ADDRESS	+ 510) //4B
#define RAMP_RATE_CTRL_RAMP_RATIO_ADDRESS                   (uint32_t)(SYS_PARAM_ADDRESS	+ 514) //2B
#define RAMP_RATE_CTRL_RAMP_TIME_ADDRESS                    (uint32_t)(SYS_PARAM_ADDRESS	+ 516) //2B
#define RAMP_RATE_CTRL_SOC_UPLIMIT_ADDRESS                  (uint32_t)(SYS_PARAM_ADDRESS	+ 518) //2B
#define RAMP_RATE_CTRL_SOC_LOWLIMIT_ADDRESS                 (uint32_t)(SYS_PARAM_ADDRESS	+ 520) //2B
#define RAMP_RATE_CTRL_VBAT_UPLIMIT_ADDRESS                 (uint32_t)(SYS_PARAM_ADDRESS	+ 522) //2B
#define RAMP_RATE_CTRL_VBAT_LOWLIMIT_ADDRESS                (uint32_t)(SYS_PARAM_ADDRESS	+ 524) //2B
#define PCS_NET_SETPARA_DELAY_ADDRESS                       (uint32_t)(SYS_PARAM_ADDRESS	+ 526) //2B
#define FM_ANALOG_DIGITAL_ADDRESS                           (uint32_t)(SYS_PARAM_ADDRESS	+ 528) //2B
#define FM_ANALOG_VMAX_ADDRESS                              (uint32_t)(SYS_PARAM_ADDRESS	+ 530) //2B
#define FM_ANALOG_VMIN_ADDRESS                              (uint32_t)(SYS_PARAM_ADDRESS	+ 532) //2B
#define RAMP_RATE_CTRL_PCS_AUTOCHARGE_POWER                 (uint32_t)(SYS_PARAM_ADDRESS	+ 534) //2B
#define PCS_NET_SET_CYCLE_ADDRESS                       	(uint32_t)(SYS_PARAM_ADDRESS	+ 536) //2B

//AGC参数地址
#define AGC_SYSPARA_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE	+ 600)//700

//AGC系统参数地址
#define MG_VOLTAGE_ADDRESS									(uint32_t)(AGC_SYSPARA_ADDRESS	+ 0)//2B
#define MG_FREQUENCY_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 2)//2B
#define TOTAL_PV_NUM_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 4)//2B
#define TOTAL_PCS_NUM_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 6)//2B
#define CONST_V_MODE_V_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 8)//2B
#define CONST_V_MODE_I_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 10)//2B
#define PCC_ACTIVE_POWER_GIVING_ADDRESS        			 	(uint32_t)(AGC_SYSPARA_ADDRESS	+ 12)//2B
#define PCC_REACTIVE_POWER_GIVING_ADDRESS       			(uint32_t)(AGC_SYSPARA_ADDRESS	+ 14)//2B
#define TOTAL_BMS_NUM_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 16)//2B
#define TOTAL_DCDC_NUM_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS 	+ 18)//2B

#define EMS_RL_MODE_ADDRESS									(uint32_t)(AGC_SYSPARA_ADDRESS	+ 20)//1B
#define EMS_RUN_MODE_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 21)//1B
#define EMS_START_TYPE_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 22)//1B
#define PCS_PROTOCOL_TYPE_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 23)//1B
#define BUS_TYPE_ADDRESS									(uint32_t)(AGC_SYSPARA_ADDRESS	+ 24)//1B
#define GRID_CONNECT_TYPE_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 25)//1B
#define DC_BUS_CHARGE_PERMIT_ADDRESS						(uint32_t)(AGC_SYSPARA_ADDRESS	+ 26)//1B
#define SYS_POWER_SET_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 28)//2B
#define SYS_PCS_POWER_TYPE_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 30)//1B
#define AMMETER_PROTOCOL_TYPE_ADDRESS						(uint32_t)(AGC_SYSPARA_ADDRESS	+ 31)//1B

#define CONST_V_MODE_V_MIN_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 55)// 2B
#define CONST_V_MODE_V_MAX_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 57)// 2B
#define EMS_RUN_POWER_SET_VALUE_ADDRESS						(uint32_t)(AGC_SYSPARA_ADDRESS	+ 59)// 2B
#define EMS_RUN_Q_ADJ_MODE_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 61)// 2B
#define EMS_RUN_Q_SET_VALUE_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 63)// 2B
#define EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS					(uint32_t)(AGC_SYSPARA_ADDRESS	+ 65)// 2B
#define ZERO_V_MAX_ADDRESS                      			(uint32_t)(AGC_SYSPARA_ADDRESS	+ 67)// 2B

#define Max_anti_reflux_power_ADDRESS		     			(uint32_t)(AGC_SYSPARA_ADDRESS	+ 82)//2B
#define Send_Data_To_PV_Broadcast_Enable_ADDRESS 			(uint32_t)(AGC_SYSPARA_ADDRESS 	+ 84)//2B

#define TOTAL_AMMETER_NUM_ADDRESS							(uint32_t)(AGC_SYSPARA_ADDRESS	+ 94)//2B
#define TOTAL_PVS_NUM_ADDRESS								(uint32_t)(AGC_SYSPARA_ADDRESS	+ 96)//2B

//SOC参数地址
#define SOC_SYSPARA_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE	+ 700) //800

//SOC运行参数地址
#define SOC_CONTROL_ENABLE_ADDRESS							(uint32_t)(SOC_SYSPARA_ADDRESS	+ 0)//2B
#define SOC_UPLIMIT2_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 2)//2B
#define SOC_UPLIMIT1_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 4)//2B
#define SOC_LOWLIMIT1_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 6)//2B
#define SOC_LOWLIMIT2_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 8)//2B
#define SOC_LOWLIMIT1_RECOVER_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 10)//2B
#define SOC_LOWLIMIT2_RECOVER_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 12)//2B
#define SOC_FLOAT_HYSTERESIS_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 14)//2B
#define SOC_CONTROL_DELAY_ADDRESS							(uint32_t)(SOC_SYSPARA_ADDRESS	+ 16)//2B
#define SOC_CONTROL_RECOVER_DELAY_ADDRESS					(uint32_t)(SOC_SYSPARA_ADDRESS	+ 18)//2B

#define VBAT_CONTROL_ENABLE_ADDRESS							(uint32_t)(SOC_SYSPARA_ADDRESS	+ 20)//2B
#define VBAT_UPLIMIT2_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 22)//2B
#define VBAT_UPLIMIT1_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 24)//2B
#define VBAT_LOWLIMIT1_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 26)//2B
#define VBAT_LOWLIMIT2_ADDRESS								(uint32_t)(SOC_SYSPARA_ADDRESS	+ 28)//2B
#define VBAT_LOWLIMIT1_RECOVER_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 30)//2B
#define VBAT_LOWLIMIT2_RECOVER_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 32)//2B
#define VBAT_FLOAT_HYSTERESIS_ADDRESS						(uint32_t)(SOC_SYSPARA_ADDRESS	+ 34)//2B
#define VBAT_CONTROL_DELAY_ADDRESS							(uint32_t)(SOC_SYSPARA_ADDRESS 	+ 36)//2B
#define VBAT_CONTROL_RECOVER_DELAY_ADDRESS					(uint32_t)(SOC_SYSPARA_ADDRESS	+ 38)//2B
#define Low_Battery_Output1_Mode_ADDRESS            		(uint32_t)(SOC_SYSPARA_ADDRESS	+ 40)//1B
#define Low_Battery_Output2_Mode_ADDRESS            		(uint32_t)(SOC_SYSPARA_ADDRESS	+ 41)//1B

//分时电价
//TIME_DIV0 ~ TIME_DIV9	 -- TIME_DIV_ADDRESS + 0 ~ TIME_DIV_ADDRESS 	+ 84
#define TIME_DIV_ADDRESS							        (uint32_t)(EEPROM_ADDRESS_BASE  + 800)//900~1000

#define TIME_DIV_START_TIME_HOUR_0_ADDRESS					(uint32_t)(TIME_DIV_ADDRESS 	+ 0)//1B
#define TIME_DIV_START_TIME_MIN_0_ADDRESS					(uint32_t)(TIME_DIV_ADDRESS 	+ 1)//1B
#define TIME_DIV_END_TIME_HOUR_0_ADDRESS					(uint32_t)(TIME_DIV_ADDRESS 	+ 2)//1B
#define TIME_DIV_END_TIME_MIN_0_ADDRESS						(uint32_t)(TIME_DIV_ADDRESS 	+ 3)//1B
#define TIME_DIV_RUN_MODE_0_ADDRESS						    (uint32_t)(TIME_DIV_ADDRESS 	+ 4)//1B
#define TIME_DIV_POWER_CTRL_MODE_0_ADDRESS					(uint32_t)(TIME_DIV_ADDRESS 	+ 5)//1B

//TIME_DIV_RUN_P_SET_0 ~ TIME_DIV_RUN_P_SET_9  -- TIME_DIV_ADDRESS 	+ 100 ~ TIME_DIV_ADDRESS + 118
#define TIME_DIV_RUN_P_SET_0_ADDRESS						(uint32_t)(TIME_DIV_ADDRESS 	+ 100)//2B 1000~1100

//TIME_DIV_WEEK_0 ~ TIME_DIV_WEEK_9 -- TIME_DIV_ADDRESS 	+ 120 ~ TIME_DIV_ADDRESS 	+ 126
#define TIME_DIV_WEEK_0_ADDRESS					            (uint32_t)(TIME_DIV_ADDRESS 	+ 120)//1B
//130~148
#define TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS                  (uint32_t)(TIME_DIV_ADDRESS 	+ 130)//2B

/////////////////////////////////////////////
#define TIME_DIV_CAL_ADDRESS								(uint32_t)(EEPROM_ADDRESS_BASE	+ 1500)	//1500 分时电价收益数据存储首地址

#define TIME_DIV_CHARGE_ENERGY_ACC_ADDRESS					(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 0)//8B
#define TIME_DIV_DISCHARGE_ENERGY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 8)//8B
#define TIME_DIV_DAY_CHARGE_ENERGY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 16)//8B
#define TIME_DIV_DAY_DISCHARGE_ENERGY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 24)//8B
#define TIME_DIV_MON_CHARGE_ENERGY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 32)//8B
#define TIME_DIV_MON_DISCHARGE_ENERGY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 40)//8B
#define TIME_DIV_YEAR_CHARGE_ENERGY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 48)//8B
#define TIME_DIV_YEAR_DISCHARGE_ENERGY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 56)//8B

#define TIME_DIV_CHARGE_MONEY_ACC_ADDRESS					(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 64)//8B
#define TIME_DIV_DISCHARGE_MONEY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 72)//8B
#define TIME_DIV_DAY_CHARGE_MONEY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 80)//8B
#define TIME_DIV_DAY_DISCHARGE_MONEY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 88)//8B
#define TIME_DIV_MON_CHARGE_MONEY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 96)//8B
#define TIME_DIV_MON_DISCHARGE_MONEY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 104)//8B
#define TIME_DIV_YEAR_CHARGE_MONEY_ACC_ADDRESS				(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 112)//8B
#define TIME_DIV_YEAR_DISCHARGE_MONEY_ACC_ADDRESS			(uint32_t)(TIME_DIV_CAL_ADDRESS	+ 120)//8B

#define TIME_DIVIDE_FUNCTION_ADDRESS                        (uint32_t)(TIME_DIV_CAL_ADDRESS	+ 128)//2B

//EMS设备地址
#define EMS_EUIP_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE 	+ 2000) //2100

//PCS0_ADDRESS ~ PCS59_ADDRESS   --   EMS_EUIP_ADDRESS+1~ EMS_EUIP_ADDRESS+61
#define PCS0_ADDRESS										(uint32_t)(EMS_EUIP_ADDRESS		+ 1)//1B

//PV0_ADDRESS ~ PV59_ADDRESS   --   EMS_EUIP_ADDRESS+101 ~ EMS_EUIP_ADDRESS+161
#define PV0_ADDRESS											(uint32_t)(EMS_EUIP_ADDRESS		+ 101)//1B

//BMS0_ADDRESS ~ BMS59_ADDRESS   --   EMS_EUIP_ADDRESS+201 ~ EMS_EUIP_ADDRESS+261
#define BMS0_ADDRESS										(uint32_t)(EMS_EUIP_ADDRESS     + 201)//1B

//DCDC0_ADDRESS ~ DCDC49_ADDRESS   --   EMS_EUIP_ADDRESS+301 ~ EMS_EUIP_ADDRESS+351
#define DCDC0_ADDRESS										(uint32_t)(EMS_EUIP_ADDRESS     + 301)//1B

//AMMETER0_ADDRESS ~ AMMETER12_ADDRESS   --   EMS_EUIP_ADDRESS+401 ~ EMS_EUIP_ADDRESS+413
#define AMMETER0_ADDRESS					    			(uint32_t)(EMS_EUIP_ADDRESS     + 401)//1B

//PVS0_ADDRESS ~ PVS49_ADDRESS   --   EMS_EUIP_ADDRESS+451 ~ EMS_EUIP_ADDRESS+501
#define PVS0_ADDRESS										(uint32_t)(EMS_EUIP_ADDRESS     + 501)//1B

//600~608
#define InputPin_Function_ADDRESS0                          (uint32_t)(EMS_EUIP_ADDRESS     + 600)//1B

//610~618
#define InputPin_NC_NO_ADDRESS0                             (uint32_t)(EMS_EUIP_ADDRESS     + 610)//1B

//620~625
#define Meter_Position_Enable_ADDRESS0                      (uint32_t)(EMS_EUIP_ADDRESS     + 620)//1B

//700~700+ENV_NUM
#define ENV0_ADDRESS										(uint32_t)(EMS_EUIP_ADDRESS     + 700)//1B


//串口参数地址
#define COM_BASS_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE  + 3000)
#define COM1_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 0) //2B
#define COM1_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 2) //2B
#define COM1_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 4) //1B
#define COM1_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 5) //1B
#define COM1_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 6) //1B

#define COM2_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 7) //2B
#define COM2_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 9) //2B
#define COM2_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 11) //1B
#define COM2_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 12) //1B
#define COM2_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 13) //1B

#define COM3_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 14) //2B
#define COM3_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 16) //2B
#define COM3_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 18) //1B
#define COM3_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 19) //1B
#define COM3_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 20) //1B

#define COM4_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 21) //2B
#define COM4_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 23) //2B
#define COM4_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 25) //1B
#define COM4_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 26) //1B
#define COM4_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 27) //1B

#define COM5_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 28) //2B
#define COM5_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 30) //2B
#define COM5_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 32) //1B
#define COM5_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 33) //1B
#define COM5_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 34) //1B

#define COM6_PARAM_BAUD_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 35) //2B
#define COM6_PARAM_VMIN_ADDRESS								(uint32_t)(COM_BASS_ADDRESS  	+ 37) //2B
#define COM6_PARAM_STRAT_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 39) //1B
#define COM6_PARAM_END_ID_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 40) //1B
#define COM6_PARAM_PROTOCOL_ADDRESS							(uint32_t)(COM_BASS_ADDRESS  	+ 41) //1B


//PCS\PV服务器参数
#define SERVER_EUIP_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE  + 4000)
// SERVER_0 ~ SERVER_59  --  SERVER_EUIP_ADDRESS + 0 ~ SERVER_EUIP_ADDRESS + 240
#define SERVER_0_IP_0_ADDRESS								(uint32_t)(SERVER_EUIP_ADDRESS  + 0)//1B
#define SERVER_0_IP_1_ADDRESS								(uint32_t)(SERVER_EUIP_ADDRESS  + 1)//1B
#define SERVER_0_IP_2_ADDRESS								(uint32_t)(SERVER_EUIP_ADDRESS  + 2)//1B
#define SERVER_0_IP_3_ADDRESS								(uint32_t)(SERVER_EUIP_ADDRESS  + 3)//1B

//SERVER_0_PORT_ADDRESS ~ SERVER_59_PORT_ADDRESS    -- SERVER_EUIP_ADDRESS + 300 ~ SERVER_EUIP_ADDRESS + 420
#define SERVER_0_PORT_ADDRESS								(uint32_t)(SERVER_EUIP_ADDRESS  + 300)//2B

//SERVER_PV_0 ~ SERVER_PV_59  --  SERVER_EUIP_ADDRESS + 500 ~ SERVER_EUIP_ADDRESS + 740
#define SERVER_PV_0_IP_0_ADDRESS							(uint32_t)(SERVER_EUIP_ADDRESS  + 500)//1B
#define SERVER_PV_0_IP_1_ADDRESS							(uint32_t)(SERVER_EUIP_ADDRESS  + 501)//1B
#define SERVER_PV_0_IP_2_ADDRESS							(uint32_t)(SERVER_EUIP_ADDRESS  + 502)//1B
#define SERVER_PV_0_IP_3_ADDRESS							(uint32_t)(SERVER_EUIP_ADDRESS  + 503)//1B

//SERVER_PV_0_PORT_ADDRESS ~ SERVER_PV_59_PORT_ADDRESS    -- SERVER_EUIP_ADDRESS + 800 ~ SERVER_EUIP_ADDRESS + 920
#define SERVER_PV_0_PORT_ADDRESS							(uint32_t)(SERVER_EUIP_ADDRESS  + 800)//2B

///////////////////////////////////////IAT协议////////////////////////////////////////////////
#define IAT_PROTOCAL_ADDRESS								(uint32_t)(EEPROM_ADDRESS_BASE  + 5000)//5100
#define IAT_SN_NUMBER_ADDRESS								(uint32_t)(IAT_PROTOCAL_ADDRESS + 18)//1B*20
#define IAT_MAC_ADDRESS										(uint32_t)(IAT_PROTOCAL_ADDRESS + 38)//6B    eth0(ETH2)对外网口
#define IAT_MAC2_ADDRESS									(uint32_t)(IAT_PROTOCAL_ADDRESS + 44)//6B    eth1(ETH1)对内网口
#define IAT_MAC_LOCK_ADDRESS								(uint32_t)(IAT_PROTOCAL_ADDRESS + 50)//2B
#define IAT_SN_ENABLE_ADDRESS								(uint32_t)(IAT_PROTOCAL_ADDRESS + 86)//2B


//////////////////////////////////////当天数据曲线////////////////////////////////////////////////
#define CURVE_ARRAY_BASE_ADDRESS							(uint32_t)(EEPROM_ADDRESS_BASE  + 6000) //6100+1440x4Bx3 = 23380

//////////////////////////////////////////////////////////////////////////////////////////////
//历史数据的属性数据，包括运行信息/消峰填谷/交流限发/, 历史数据存放在单独的文件中
#define LOG_ATTR_ADDRESS									(uint32_t)(EEPROM_ADDRESS_BASE  + 30000) //预留 10000

//////////////////////////////////////升级属性///////////////////////////////////////////////////
//#define IAP_ATTR_BASE         							(EEPROM_ADDRESS_BASE 	+ 40000)  // EEPROM_ADDRESS_BASE + 40000 + 62 + 12000*4

////////////////////////////////////复位/告警/故障/事件记录/////////////////////////////////////////
///////////////////////////////设备复位记录  16x100 = 1600 字节///////////////////////////////////
#define HISRESETRECORD_ADDRESS_BASE    	   					(uint32_t)(IAP_ATTR_BASE+62+PKG_CNT_BUFLEN*4) // 88064
#define HISRESETRECORD_PTR_ADDRESS            				(uint32_t)(HISRESETRECORD_ADDRESS_BASE+0)//记录指针
#define HISRESETRECORD_NB_ADDRESS             				(uint32_t)(HISRESETRECORD_ADDRESS_BASE+2)//记录总数
#define HISRESETRECORD_START_ADDRESS          				(uint32_t)(HISRESETRECORD_ADDRESS_BASE+10)//记录的开始地址
#define HISRESETRECORD_END_ADDRESS							(uint32_t)(HISRESETRECORD_START_ADDRESS+2000)

//存储EEPROM中 历史事件的参数变量地址  记录部分 16×10000＝160000字节
#define HISEVENTRECORD_ADDRESS_BASE    	   					(uint32_t)(HISRESETRECORD_END_ADDRESS)
#define HISEVENTRECORD_PTR_ADDRESS            				(uint32_t)(HISEVENTRECORD_ADDRESS_BASE+0)//记录指针
#define HISEVENTRECORD_NB_ADDRESS             				(uint32_t)(HISEVENTRECORD_ADDRESS_BASE+2)//记录总数
#define HISEVENTRECORD_START_ADDRESS          				(uint32_t)(HISEVENTRECORD_ADDRESS_BASE+10)//记录的开始地址

//存储EEPROM中 历史故障的参数变量地址  记录部分16*10000=160000字节
#define HISFAULTRECORD_ADDRESS_BASE    	   					(uint32_t)(HISEVENTRECORD_START_ADDRESS+160000)
#define HISFAULTRECORD_PTR_ADDRESS            				(uint32_t)(HISFAULTRECORD_ADDRESS_BASE+0)//历史故障指针
#define HISFAULTRECORD_NB_ADDRESS             				(uint32_t)(HISFAULTRECORD_ADDRESS_BASE+2)//历史故障总数
#define HISFAULTRECORD_START_ADDRESS          				(uint32_t)(HISFAULTRECORD_ADDRESS_BASE+16)//历史故障开始地址

//存储EEPROM中 告警的参数变量地址  记录部分16*10000=160000字节
#define HISALARMRECORD_ADDRESS_BASE    	   					(uint32_t)(HISFAULTRECORD_START_ADDRESS+160000)
#define HISALARMRECORD_PTR_ADDRESS            				(uint32_t)(HISALARMRECORD_ADDRESS_BASE+0)//历史告警指针
#define HISALARMRECORD_NB_ADDRESS             				(uint32_t)(HISALARMRECORD_ADDRESS_BASE+2)//历史告警总数
#define HISALARMRECORD_START_ADDRESS          				(uint32_t)(HISALARMRECORD_ADDRESS_BASE+16)//历史告警开始地址

//注意 eeprom使用中出现问题， 其驱动会影响本程序中的线程调度，故而不再使用 -- 2017-8-30 L.K
//////////////////////////eeprom:FM32W256 最大 32KB////////////////////////////

//使用文件保存参数, 大小 560k

#endif



/*
 * adc_thread.cpp
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#include "includes.h"
#include "adc_thread.h"

AdcThread::AdcThread()
{
	m_bStart = true;
	rLen = 0;
}

AdcThread::~AdcThread()
{

}

int AdcThread::SetDevicePath(const char* path)
{
	memset(Path, 0x00, sizeof(Path));
	memcpy(Path, path, strlen(path));
	return 0;
}

void AdcThread::run()
{
	uint16_t ad_value;
	float temp_pt[2];
	uint16_t crcresult;
	int fd;
	fd = open (Path, O_RDONLY|O_NOCTTY|O_NONBLOCK);
	if(fd < 0) m_bStart = false;
	close(fd);
	fd = -1;

	while(1)
	{
		if(m_bStart)
		{
			if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode == FM && Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital == 1)
			{
				rLen = drv_read_adc(Path, rBuf, 40);
				if(rLen > 0)
				{
					//for(int i = 0; i < rLen; i++)printf("%02x ", rBuf[i]); printf("\n");
					if((rBuf[0] == 0x68) && (rBuf[1] == 0x68) && (rBuf[36] == 0x69))
					{
						crcresult=embf_get_crc16(rBuf,34);
						if(rBuf[34]== LOBYTE(crcresult) && rBuf[35]== HIBYTE(crcresult))
						{
							mutex_for_adc.lock();

							//AI1,AI2 -- 0~10V/4~20mA
							for(int i = 0; i < 2; i++)
							{
								ad_value = rBuf[2 * i + 26] * 256 + rBuf[2 * i + 27];
								adc.I_adc[i] = 2500 * ((float)(ad_value * 189) / (4096 * 39 * 499));
								adc.V_adc[i] = 2.5 * ((float)(ad_value * 189) / (4096 * 39));
							}
							//printf("[AI1] V = %.1f, A = %.1f ", adc.V_adc[0], adc.I_adc[0]);
							//printf("[AI2] V = %.1f, A = %.1f ", adc.V_adc[1], adc.I_adc[1]);

							//AI3,AI4 -- 0~10V
							ad_value = rBuf[30] * 256 + rBuf[31];
							adc.V_adc[2] = 2500 * ((float)(ad_value ) / (4096 * 101));
							ad_value = rBuf[4] * 256 + rBuf[5];
							adc.V_adc[3] = 2500 * ((float)(ad_value ) / (4096 * 101));

							//printf("[AI3] V = %.1f ", adc.V_adc[2]);
							//printf("[AI4] V = %.1f\n", adc.V_adc[3]);

							temp_pt[0] = 2500 * ((float)(rBuf[2] * 256 + rBuf[3]) / (4096  * 15 ));
							temp_pt[1] = 2500 * ((float)(rBuf[24]  * 256 + rBuf[25])  / (4096 *15));
							adc.PT[0] = (temp_pt[0] / 100 - 1) / PT_FACTOR;
							adc.PT[1] = (temp_pt[1] / 100 - 1) / PT_FACTOR;

							//printf("[PT1] temp = %.1f ",adc.PT[0]);
							//printf("[PT2] temp = %.1f\n",adc.PT[1]);

							mutex_for_adc.unlock();
						}
					}
				}
				usleep(50);
			}
			else
			{
				sleep(3);
			}
		}
		else
		{
			sleep(3);
		}
	}
}

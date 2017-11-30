/*
 * utils.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "utils.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "pthread.h"

pthread_mutex_t mutex_for_eeporm;
pthread_mutex_t mutex_for_eeporm_file;
pthread_mutex_t mutex_for_running_info;
pthread_mutex_t mutex_for_time_of_use;
pthread_mutex_t mutex_for_peak_valley;
pthread_mutex_t mutex_for_ac_limiting;
pthread_mutex_t mutex_for_output_smooth;
pthread_mutex_t mutex_for_fm;
pthread_mutex_t mutex_for_mgrid;
pthread_mutex_t mutex_for_update;

uint32_t CRC32_table[256] =
{
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
  0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
  0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
  0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
  0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
  0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
  0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
  0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
  0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
  0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
  0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
  0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
  0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
  0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
  0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
  0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
  0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
  0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
  0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
  0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
  0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
  0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
  0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
  0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
  0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
  0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
  0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
  0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
  0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
  0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
  0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
  0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
  0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
  0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
  0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
  0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
  0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
  0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
  0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
  0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
  0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
  0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
  0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

#define MAX_EEPROM_SIZE 	(560*1024)  //573440
static volatile uint8_t eeprom[MAX_EEPROM_SIZE];
volatile bool save_eeprom_flag = false;

int super_system(const char * cmd)
{
	FILE * fp;

	if (cmd == NULL) return -1;

	if ((fp = popen(cmd, "w") ) == NULL){   // "w" "r"
		printf("timer thread: POPEN error: %s\n", strerror(errno));
		return -1;
	}else{
		pclose(fp);
	}
	return 0;
}

long get_file_size_time (const char *filename, long *len, long *sec)
{
	struct stat statbuf;

	if (stat (filename, &statbuf) == -1){
		printf ("Get stat on %s Error：%s\n", filename, strerror (errno));
		return (-1);
    }
	if (S_ISDIR (statbuf.st_mode))
		return (1);
	if (S_ISREG (statbuf.st_mode))
		//printf ("%s size：%ld bytes\tmodified at %s\n", filename, statbuf.st_size, ctime (&statbuf.st_mtime));
		printf ("%s size：%ld bytes\tmodified at %ld\n", filename, statbuf.st_size, (long)statbuf.st_mtime);

	*len = statbuf.st_size;
	*sec = statbuf.st_mtime;

	return (0);
}

//main中初始化
void init_eeprom()
{
	long eeprom_len = 0, eeprom_sec = 0;
	long eeprom_bak_len = 0, eeprom_bak_sec = 0;

	pthread_mutex_init(&mutex_for_eeporm, NULL);
	pthread_mutex_init(&mutex_for_running_info, NULL);
	pthread_mutex_init(&mutex_for_time_of_use, NULL);
	pthread_mutex_init(&mutex_for_peak_valley, NULL);
	pthread_mutex_init(&mutex_for_ac_limiting, NULL);
	pthread_mutex_init(&mutex_for_output_smooth, NULL);
	pthread_mutex_init(&mutex_for_fm, NULL);
	pthread_mutex_init(&mutex_for_update, NULL);
	memset((char*)eeprom, 0x00, MAX_EEPROM_SIZE);

	FILE *fp = NULL;
	if((access("eeprom",F_OK))!=-1)
	{
		get_file_size_time("eeprom", &eeprom_len, &eeprom_sec);
		fp = fopen("eeprom", "rb");
		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_SET);
			fread((char*)eeprom, 1, 131, fp);
			fclose(fp);
		}
		fp = NULL;
		//判断参数是否有效,判断初始上电标志
		if(eeprom[130] == 0x00) // 无效
		{
			printf("0x%x eeprom BAD!\n", eeprom[130]);
			if((access("eeprom_bak",F_OK))!=-1)
			{
				printf("1:COPY new!\n");
				super_system("cp -f /opt/eeprom_bak /opt/eeprom 2>null");
				sleep(1);
			}
		}
		else
		{
			printf("0x%x eeprom OK!\n", eeprom[130]);
			//判断参数新旧
			if((access("eeprom_bak",F_OK))!=-1)
			{
				get_file_size_time("eeprom_bak", &eeprom_bak_len, &eeprom_bak_sec);
				if(eeprom_len == 573440 && eeprom_bak_len == 573440 && eeprom_bak_sec > eeprom_sec)  //备份文件较新
				{
					fp = fopen("eeprom_bak", "rb");
					if(fp != NULL)
					{
						fseek(fp, 0, SEEK_SET);
						fread((char*)eeprom, 1, 131, fp);
						fclose(fp);
						if(eeprom[130] == 0xA1)  //判断参数是否有效,判断初始上电标志有效才拷贝
						{
							printf("2:COPY new!\n");
							super_system("cp -f /opt/eeprom_bak /opt/eeprom 2>null");
						}
						sleep(1);
					}
					fp = NULL;
				}
			}
		}
	}

	//eeprom
	fp = fopen("eeprom", "rb");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fread((char*)eeprom, 1, MAX_EEPROM_SIZE, fp);
		fclose(fp);
	}
	fp = NULL;
}

//定时调用，比如1min, 文件储存方式
int save_eeprom()
{
	int bak = 0;
	if(save_eeprom_flag)
	{
		save_eeprom_flag = false;
		pthread_mutex_lock(&mutex_for_eeporm_file);
		//文件存储
		FILE *fp = fopen("eeprom", "wb");
		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_SET);
			fwrite((char*)eeprom, 1, MAX_EEPROM_SIZE, fp);
			fflush(fp);
			fclose(fp);
			bak = 1;
		}
		pthread_mutex_unlock(&mutex_for_eeporm_file);
	}
	return bak;
}
//定时调用，比如1min, 文件储存方式
void bak_eeprom()
{
	pthread_mutex_lock(&mutex_for_eeporm_file);
	super_system("cp -f /opt/eeprom /opt/eeprom_bak 2>null");
	pthread_mutex_unlock(&mutex_for_eeporm_file);
}

inline int32_t write_eeprom(uint32_t addr, const void* buf, uint32_t count)
{
	if(addr > MAX_EEPROM_SIZE-1) return 0;
	pthread_mutex_lock(&mutex_for_eeporm_file);
	int i=0;
	char *p = (char*)&eeprom[addr];
	char *q = (char*)buf;
	for(i=0; i<count; i++) p[i] = q[i];
	save_eeprom_flag = true;
	pthread_mutex_unlock(&mutex_for_eeporm_file);
	return count;
}

inline int32_t read_eeprom(uint32_t addr, void* buf, uint32_t count)
{
	if(addr > MAX_EEPROM_SIZE-1) return 0;
	pthread_mutex_lock(&mutex_for_eeporm_file);
	/*
	//文件存储
	FILE *fp = fopen("eeprom_file", "rb");  //读文件
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	*/
	memcpy((uint8_t*)buf, (uint8_t*)&eeprom[addr], count);
	pthread_mutex_unlock(&mutex_for_eeporm_file);
	return count;
}

int32_t write_running_info(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_running_info);
	FILE *fp = fopen("running_info", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_running_info);
	return count;
}

int32_t read_running_info(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_running_info);
	FILE *fp = fopen("running_info", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_running_info);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_running_info()
{
	pthread_mutex_lock(&mutex_for_running_info);
	FILE *fp = fopen("running_info", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_running_info);
	return 0;
}

int32_t write_time_of_use(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_time_of_use);
	FILE *fp = fopen("time_of_use", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_time_of_use);
	return count;
}

int32_t read_time_of_use(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_time_of_use);
	FILE *fp = fopen("time_of_use", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_time_of_use);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_time_of_use()
{
	pthread_mutex_lock(&mutex_for_time_of_use);
	FILE *fp = fopen("time_of_use", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_time_of_use);
	return 0;
}

int32_t write_peak_valley(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_peak_valley);
	FILE *fp = fopen("peak_valley", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_peak_valley);
	return count;
}

int32_t read_peak_valley(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_peak_valley);
	FILE *fp = fopen("peak_valley", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_peak_valley);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_peak_valley()
{
	pthread_mutex_lock(&mutex_for_peak_valley);
	FILE *fp = fopen("peak_valley", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_peak_valley);
	return 0;
}

int32_t write_ac_limiting(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_ac_limiting);
	FILE *fp = fopen("ac_limiting", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_ac_limiting);
	return count;
}

int32_t read_ac_limiting(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_ac_limiting);
	FILE *fp = fopen("ac_limiting", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_ac_limiting);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_ac_limiting()
{
	pthread_mutex_lock(&mutex_for_ac_limiting);
	FILE *fp = fopen("ac_limiting", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_ac_limiting);
	return 0;
}

int32_t write_output_smooth(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_output_smooth);
	FILE *fp = fopen("output_smooth", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_output_smooth);
	return count;
}

int32_t read_output_smooth(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_output_smooth);
	FILE *fp = fopen("output_smooth", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_output_smooth);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_output_smooth()
{
	pthread_mutex_lock(&mutex_for_output_smooth);
	FILE *fp = fopen("output_smooth", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_output_smooth);
	return 0;
}

int32_t write_fm(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_fm);
	FILE *fp = fopen("fm", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_fm);
	return count;
}

int32_t read_fm(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_fm);
	FILE *fp = fopen("fm", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_fm);
	return count;
}

//对于追加的文件只要使用w方式打开即可清除原来的数据
int32_t clear_fm()
{
	pthread_mutex_lock(&mutex_for_fm);
	FILE *fp = fopen("fm", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_fm);
	return 0;
}

int32_t write_mgrid(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_mgrid);
	FILE *fp = fopen("mgrid", "ab+"); //二进制追加
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp);
		fflush(fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_mgrid);
	return count;
}

int32_t read_mgrid(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_mgrid);
	FILE *fp = fopen("mgrid", "rb"); //二进制读取
	if(fp != NULL)
	{
		fseek(fp, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_mgrid);
	return count;
}

int32_t clear_mgrid()
{
	pthread_mutex_lock(&mutex_for_mgrid);
	FILE *fp = fopen("mgrid", "w");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_mgrid);
	return 0;
}

//////////////////////
FILE *fp_update=NULL;

void open_update_file(char *filename, char *mode)
{
	char path[128];

	pthread_mutex_lock(&mutex_for_update);
	memset(path, 0x00, 128);
	sprintf(path, "update_%s", filename);
	fp_update = fopen(path, mode); //二进制写："wb" 二进制读："rb"
	if(fp_update != NULL)
	{
		//printf("open %s mode %s ok\n", path, mode);
	}
	pthread_mutex_unlock(&mutex_for_update);
}

int32_t write_update_file(uint32_t addr, const void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_update);
	if(fp_update != NULL)
	{
		fseek(fp_update, addr, SEEK_SET);
		fwrite((char*)buf, 1, count, fp_update);
		fflush(fp_update);
	}
	pthread_mutex_unlock(&mutex_for_update);
	return count;
}

int32_t read_update_file(uint32_t addr, void* buf, uint32_t count)
{
	pthread_mutex_lock(&mutex_for_update);
	if(fp_update != NULL)
	{
		fseek(fp_update, addr, SEEK_SET);
		fread((char*)buf, 1, count, fp_update);
	}
	pthread_mutex_unlock(&mutex_for_update);
	return count;
}

void close_update_file()
{
	pthread_mutex_lock(&mutex_for_update);
	if(fp_update != NULL)
	{
		fclose(fp_update);
		//printf("close update file ok\n");
	}
	FILE * fp;
	if ((fp = popen("chmod 777 update_*", "w") ) != NULL){
		pclose(fp);
	}
	pthread_mutex_unlock(&mutex_for_update);
}

int32_t clear_update_file()
{
	FILE * fp;
	if ((fp = popen("rm -rf update_*", "w") ) == NULL){
		return -1;
	}else{
		pclose(fp);
	}
	return 0;
}

uint8_t embf_bcd_to_dec(uint8_t bcd)
{
    return ((((bcd >> 4) & 0x0F) * 10) + (bcd & 0x0F));
}

uint8_t embf_dec_to_bcd(uint8_t dec)
{
    return (((dec / 10) << 4) | (dec % 10));
}

uint8_t embf_get_crc8(uint8_t *id, uint8_t len)
{
	uint8_t i = 0;
	uint8_t crc = 0;
	while (len-- != 0)
	{
	for (i = 1; i != 0; i *= 2)
	{
	  if ((crc &1) != 0)
	  {
		crc /= 2;
		crc ^= 0x8c;
	  }
	  else
	  {
		crc /= 2;
	  }
	  if ((*id &i) != 0)
	  {
		crc ^= 0x8c;
	  }
	}
	id++;
	}

	return crc;
}

uint16_t embf_get_crc16(volatile uint8_t *ptr, uint16_t len)
{
    uint8_t i;
    uint16_t crc=0xFFFF;
    if(len==0)
    {
        len=1;
    }
    while (len--)
    {
        crc ^= *ptr;
        for (i=0; i<8; i++)
        {
            if (crc&1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return (crc);
}
uint32_t embf_get_crc32(volatile uint8_t *ptr, uint16_t len)
{
	uint32_t CRC32_data = 0xFFFFFFFF;
	uint32_t i=0;

	for (i = 0; i != len; ++i)
	{
		uint32_t t = (CRC32_data ^ ptr[i]) & 0xFF;
		CRC32_data = ((CRC32_data >> 8) & 0xFFFFFF) ^ CRC32_table[t];
	}
	return ~CRC32_data;
}

uint32_t CRC32(uint32_t* InputData, int len)
{
	uint32_t dwPolynomial = 0x04c11db7;
	uint32_t xbit;
	uint32_t data;
	uint32_t crc_cnt = 0xFFFFFFFF; // init
	int i=0, bits=0;

	for (i = 0; i < len; i++)
	{
		// xbit = 1 << 31;
		xbit = 0x80000000;
		data = InputData[i];
		for (bits = 0; bits < 32; bits++)
		{
			if ((crc_cnt & 0x80000000) > 0)
			{
				crc_cnt <<= 1;
				crc_cnt ^= dwPolynomial;
			}
			else
				crc_cnt <<= 1;
			if ((data & xbit) > 0)
				crc_cnt ^= dwPolynomial;
			xbit >>= 1;
		}
	}
	return crc_cnt;
}

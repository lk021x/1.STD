/*
 * net_server_thread.cpp
 *
 *  Created on: 2017-5-19
 *      Author: root
 */
#include "includes.h"
#include <sys/vfs.h>
#include "net_server_thread.h"
extern NetEventThread net_event_srv;

#if 0
#include <linux/proc_fs.h>	/* for PROC_SUPER_MAGIC */
#else
#define PROC_SUPER_MAGIC 0x9fa0
#endif

#define PROCFS "/proc"

NetServerThread::NetServerThread()
{
	m_bStart = true;
	Time_100ms_cur = 0;
	Time_100ms_last = 0;
	Time_1s_cur = 0;
	Time_1s_last = 0;
	Time_2s_cur = 0;
	Time_2s_last = 0;
	Time_5s_cur = 0;
	Time_5s_last = 0;
	procstatenames[0] = (char*)"";
	procstatenames[1] = (char*)" running, ";
	procstatenames[2] = (char*)" sleeping, ";
	procstatenames[3] = (char*)" uninterruptable, ";
	procstatenames[4] = (char*)" zombie, ";
	procstatenames[5] = (char*)" stopped, ";
	procstatenames[6] = (char*)" swapping, ";
	procstatenames[7] = NULL;

	cpustatenames[0] = (char*)"user";
	cpustatenames[1] = (char*)"nice";
	cpustatenames[2] = (char*)"system";
	cpustatenames[3] = (char*)"idle";
	cpustatenames[4] = NULL;

	memorynames[0] = (char*)"K used, ";
	memorynames[1] = (char*)"K free, ";
	memorynames[2] = (char*)"K shd, ";
	memorynames[3] = (char*)"K buf  Swap: ";
	memorynames[4] = (char*)"K used, ";
	memorynames[5] = (char*)"K free";
	memorynames[6] = NULL;
}

NetServerThread::~NetServerThread()
{

}

char* NetServerThread::skip_token(const char *p)
{
	if(p==NULL)return NULL;
    while (isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    return (char *)p;
}

int NetServerThread::machine_init(void)
{
    /* make sure the proc filesystem is mounted */
	struct statfs sb;
	if (statfs(PROCFS, &sb) < 0 || sb.f_type != PROC_SUPER_MAGIC)
	{
		fprintf(stderr, " proc filesystem not mounted on " PROCFS "\n");
		return -1;
	}

    /* chdir to the proc filesystem to make things easier */
    //不能cd到/proc否则会影响参数的保存
    //chdir(PROCFS);

	stat.procstate_names = procstatenames;
	stat.cpustate_names = cpustatenames;
	stat.memory_names = memorynames;

    return 0;
}

long NetServerThread::percentages(int cnt, int *out, long *_new, long *old, long *diffs)
{
    int i;
    long change;
    long total_change;
    long *dp;
    long half_total;

    /* initialization */
    total_change = 0;
    dp = diffs;

    /* calculate changes for each state and the overall change */
    for (i = 0; i < cnt; i++)
    {
		if ((change = *_new - *old) < 0)
		{
			/* this only happens when the counter wraps */
			change = (int)
			((unsigned long)*_new-(unsigned long)*old);
		}
		total_change += (*dp++ = change);
		*old++ = *_new++;
    }

    /* avoid divide by zero potential */
    if (total_change == 0)
    {
    	total_change = 1;
    }

    /* calculate percentages based on overall change, rounding up */
    half_total = total_change / 2l;
    for (i = 0; i < cnt; i++)
    {
    	*out++ = (int)((*diffs++ * 1000 + half_total) / total_change);
    }

    /* return the total in case the caller wants to use it */
    return(total_change);
}

void NetServerThread::get_system_info(void)
{
    char buffer[4096+1];
    int len;
    char *p=NULL;
    FILE *fp=NULL;
    int i;

    /* get the cpu time info */
	fp = fopen("/proc/stat", "rb");
	if(fp != NULL){
		fseek(fp, 0, SEEK_SET);
		len = fread((char*)buffer, 1, sizeof(buffer)-1, fp);
		fclose(fp);
		fp=NULL;
	}else{
		return;
	}
	buffer[len] = '\0';
	p = skip_token(buffer);			/* "cpu" */
	cp_time[0] = strtoul(p, &p, 0);
	cp_time[1] = strtoul(p, &p, 0);
	cp_time[2] = strtoul(p, &p, 0);
	cp_time[3] = strtoul(p, &p, 0);

	/* convert cp_time counts to percentages */
	percentages(4, cpu_states, cp_time, cp_old, cp_diff);

    /* get system wide memory usage */
	fp = fopen("/proc/meminfo", "rb");
	if(fp != NULL){
		fseek(fp, 0, SEEK_SET);
		len = fread((char*)buffer, 1, sizeof(buffer)-1, fp);
		fclose(fp);
		fp=NULL;
	}else{
		return;
	}
	buffer[len] = '\0';
	/* be prepared for extra columns to appear be seeking to ends of lines */
	p = buffer;
	p = skip_token(p);
	memory_stats[0] = strtoul(p, &p, 10); /* total memory */
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[1] = strtoul(p, &p, 10); /* free memory */
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[2] = strtoul(p, &p, 10); /* buffer memory */
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[3] = strtoul(p, &p, 10); /* cached memory */
	for(i = 0; i< 8 ;i++) {
		p++;
		p = strchr(p, '\n');
	}
	p = skip_token(p);
	memory_stats[4] = strtoul(p, &p, 10); /* total swap */
	p = strchr(p, '\n');
	p = skip_token(p);
	memory_stats[5] = strtoul(p, &p, 10); /* free swap */

    /* set arrays and strings */
	//info.cpustates = cpu_states;
	//info.memory = memory_stats;
}

inline int NetServerThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	cur = (int64_t)tv.tv_sec*1000+(int64_t)tv.tv_usec/1000; //毫秒

	if(last==0 || cur<last){last=cur;return 0;}
	if(cur > last && (cur-last) >= ms){ //定时时间到
		last = cur;
		return 1;
	}else{
		return 0;
	}
}

float NetServerThread::Get_Used_CPU()
{
	float cpu=0.0;
	cs.lock();
	cpu = (float)cpu_states[0]/10.0;
	cs.unlock();
	return cpu;
}

int32_t NetServerThread::Get_Free_MEM()
{
	int32_t mem=0;
	cs.lock();
	mem = memory_stats[1];
	cs.unlock();
	return mem;
}

void NetServerThread::run()
{
	machine_init();
	while(m_bStart)
	{
		if (CheckTimeout(100, Time_100ms_cur, Time_100ms_last))
		{
			IEC104_linkSent();
		}

		if (CheckTimeout(1000, Time_1s_cur, Time_1s_last))
		{
			IEC104_APDU_TimeOutProc();
		}

		if (CheckTimeout(2000, Time_2s_cur, Time_2s_last))
		{
			//web服务器异常处理
			if(failed_accept_times >= 3)
			{
				net_event_srv.stop_web();
				server_close();
				failed_accept_times = 0;
			}

			doChangeMeasureUpload();//遥测变化上传
			doChangeSignalUpload();//遥信变化上传  ccg 2013.11.30

			//生效网卡参数
			if(Com_Data.Touch_Set_Net_Flag)
			{
				Com_Data.Touch_Set_Net_Flag = false;
				SetNetParam(System_Info.Net, (char*)"eth0");
			}

			if(Com_Data.Touch_Set_Net2_Flag)
			{
				Com_Data.Touch_Set_Net2_Flag = false;
				SetNetParam(System_Info.Net2, (char*)"eth1");
			}
		}

		if (CheckTimeout(5000, Time_5s_cur, Time_5s_last))
		{
			Update_User_Login_Status();
			cs.lock();
			get_system_info();
			cs.unlock();
			/*
			printf("Used CPU:%.1f%%\n",(float)info.cpustates[0]/10);
			printf("Nice CPU:%.1f%%\n",(float)info.cpustates[1]/10);
			printf("System CPU:%.1f%%\n",(float)info.cpustates[2]/10);
			printf("Idle CPU:%.1f%%\n",(float)info.cpustates[3]/10);
			printf("total memroy:%d\n", info.memory[0]);
			printf("free memroy:%d\n", info.memory[1]);
			printf("buffers:%d\n", info.memory[2]);
			printf("cached:%d\n", info.memory[3]);
			printf("total swap:%d\n", info.memory[4]);
			printf("free swap:%d\n", info.memory[5]);
			printf("..................................\n");
			*/
		}
		usleep(20);
	}
}


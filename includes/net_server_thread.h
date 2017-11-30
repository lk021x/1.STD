/*
 * net_server_thread.h
 *
 *  Created on: 2017-5-19
 *      Author: root
 */

#ifndef NET_SERVER_THREAD_H_
#define NET_SERVER_THREAD_H_

#include "includes.h"
#include "MThread.h"

struct statics
{
    char **procstate_names;
    char **cpustate_names;
    char **memory_names;
#ifdef ORDER
    char **order_names;
#endif
};

struct system_info
{
    int    last_pid;
    double load_avg[3];
    int    p_total;
    int    p_active;     /* number of procs considered "active" */
    int    *procstates;
    int    *cpustates;
    int    *memory;
};

#define NPROCSTATES 7
#define NCPUSTATES 4
#define NMEMSTATS 6

class NetServerThread: public MThread
{
public:
	NetServerThread();
	virtual ~NetServerThread();

	float Get_Used_CPU();
	int32_t Get_Free_MEM();

	void run();
private:
	bool m_bStart;
	int64_t Time_100ms_cur,Time_100ms_last;
    int64_t Time_1s_cur,Time_1s_last;
    int64_t Time_2s_cur,Time_2s_last;
    int64_t Time_5s_cur,Time_5s_last;

    char *procstatenames[NPROCSTATES+1];
    char *cpustatenames[NCPUSTATES+1];
    char *memorynames[NMEMSTATS+1];

    long cp_time[NCPUSTATES];
    long cp_old[NCPUSTATES];
    long cp_diff[NCPUSTATES];
    int cpu_states[NCPUSTATES];
    int memory_stats[NMEMSTATS];

	struct statics stat;
	struct system_info info;
	CCriticalSection cs;

	char *skip_token(const char *p);
	long percentages(int cnt, int *out, long *_new, long *old, long *diffs);
	int machine_init(void);
	void get_system_info(void);
	inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
};


#endif /* NET_SERVER_THREAD_H_ */

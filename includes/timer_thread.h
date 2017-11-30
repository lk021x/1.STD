/*
 * timer_thread.h
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

#ifndef TIMER_THREAD_H_
#define TIMER_THREAD_H_

#include "MThread.h"

class TimerThread : public MThread
{
public:
	TimerThread();
	virtual ~TimerThread();

	void pro_10ms(void);        //100ms定时器处理函数
	void pro_20ms(void);         //20ms定时器处理函数
	void pro_300ms(void);       //300ms定时器处理函数
	void pro_500ms(void);       //500ms定时器的处理函数
	void pro_1s(void);          //1s定时器的处理函数
	void pro_2s(void);          //2s定时器的处理函数
	void pro_1min(void);        //1min定时器的处理函数
	//void pro_delay_nms(void);   //不定延时处理函数

	void run();
private:
	bool m_bStart;
    int64_t Time_10ms_cur,Time_10ms_last;
    int64_t Time_20ms_cur,Time_20ms_last;
    //int64_t Time_delayms_cur,Time_delayms_last;
    int64_t Time_300ms_cur,Time_300ms_last;
    int64_t Time_500ms_cur,Time_500ms_last;
    int64_t Time_1s_cur,Time_1s_last;
    int64_t Plus_2s_cur,Plus_2s_last;
    int64_t Plus_3s_cur,Plus_3s_last;
    int64_t Plus_5s_cur,Plus_5s_last;
    int64_t Plus_10s_cur,Plus_10s_last;
    int64_t Plus_1min_cur,Plus_1min_last;
    inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
};


#endif /* TIMER_THREAD_H_ */

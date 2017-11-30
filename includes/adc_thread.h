/*
 * can_thread.h
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#ifndef  _ADC_THREAD_H_
#define _ADC_THREAD_H_

#include "MThread.h"

#define PT_FACTOR 0.00392

class AdcThread : public MThread
{
public:
	AdcThread();
	virtual ~AdcThread();

	int SetDevicePath(const char* path);
	void run();
private:
	bool m_bStart;
	char Path[128];
	int rLen;

	uint8_t rBuf[255];
};


#endif /* _ADC_THREAD_H_ */

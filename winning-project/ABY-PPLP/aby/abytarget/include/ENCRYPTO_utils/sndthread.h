/**
 \file 		sndthread.h
 \author 	michael.zohner@ec-spride.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 ENCRYPTO Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Receiver Thread Implementation
 */


#ifndef SND_THREAD_H_
#define SND_THREAD_H_

#include "thread.h"
#include <memory>
#include <queue>

class CSocket;


class SndThread: public CThread {
public:
	SndThread(CSocket* sock, CLock *glock);

	void stop();

	~SndThread();

	CLock* getlock() const;

    void setlock(CLock *glock);

	void add_snd_task_start_len(uint8_t channelid, uint64_t sndbytes, uint8_t* sndbuf, uint64_t startid, uint64_t len);

	void add_event_snd_task_start_len(CEvent* eventcaller, uint8_t channelid, uint64_t sndbytes, uint8_t* sndbuf, uint64_t startid, uint64_t len);

	void add_snd_task(uint8_t channelid, uint64_t sndbytes, uint8_t* sndbuf);

	void add_event_snd_task(CEvent* eventcaller, uint8_t channelid, uint64_t sndbytes, uint8_t* sndbuf);

	void signal_end(uint8_t channelid);

	void kill_task();

	void ThreadMain();

private:
	struct snd_task {
		uint8_t channelid;
		std::vector<uint8_t> snd_buf;
		CEvent* eventcaller;
	};

	void push_task(std::unique_ptr<snd_task> task);

	CSocket* mysock;
	CLock* sndlock;
	std::unique_ptr<CEvent> send;
	std::queue<std::unique_ptr<snd_task>> send_tasks;
};



#endif /* SND_THREAD_H_ */

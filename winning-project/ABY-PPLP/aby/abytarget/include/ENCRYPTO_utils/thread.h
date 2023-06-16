/**
 \file 		thread.h
 \author 	Seung Geol Choi
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

#ifndef __THREAD_H__BY_SGCHOI
#define __THREAD_H__BY_SGCHOI

#include <condition_variable>
#include <mutex>
#include <thread>

class CThread {
public:
	CThread();
	virtual ~CThread();

	bool Start();
	bool Wait();
	bool IsRunning() const;

protected:
	virtual void ThreadMain() = 0;

	bool m_bRunning;
	std::thread thread_;
};

class CLock {
public:
	CLock() = default;
	~CLock() = default;

	void Lock();
	void Unlock();

	// make CLock `BasicLockable`
	void lock();
	void unlock();

private:
	std::mutex mutex_;
};

class CEvent {
public:
	CEvent(bool bManualReset=false, bool bInitialSet=false);
	~CEvent() = default;

	bool Set();
	bool Wait();
	bool IsSet() const;
	bool Reset();

private:
	std::condition_variable cv_;
	mutable std::mutex mutex_;
	bool m_bManual;
	bool m_bSet;
};

#endif //__THREAD_H__BY_SGCHOI

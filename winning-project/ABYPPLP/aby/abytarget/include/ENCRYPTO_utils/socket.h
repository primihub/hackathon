/**
 \file 		socket.h
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
 \brief		Socket Implementation
 */

#ifndef __SOCKET_H__BY_SGCHOI
#define __SOCKET_H__BY_SGCHOI

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>


class CSocket {
public:
	CSocket(bool verbose=false);
	~CSocket();

	uint64_t getSndCnt() const;
	uint64_t getRcvCnt() const;
	void ResetSndCnt();
	void ResetRcvCnt();

	bool Socket();

	void Close();

	std::string GetIP() const;

	uint16_t GetPort() const;

	bool Bind(const std::string& address = "", uint16_t port = 0);

	bool Listen(int nQLen = 5);

	std::unique_ptr<CSocket> Accept();

	bool Connect(const std::string& host, uint16_t port);

	size_t Receive(void* buf, size_t bytes);

	size_t Send(const void* buf, size_t bytes);

private:
	struct CSocketImpl;
	std::unique_ptr<CSocketImpl> impl_;
	uint64_t send_count_, recv_count_;
	mutable std::mutex send_count_mutex_;
	mutable std::mutex recv_count_mutex_;
	bool verbose_;
};

#endif //SOCKET_H__BY_SGCHOI


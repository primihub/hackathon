/**
 \file 		channel.h
 \author 	michael.zohner@ec-spride.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 Engineering Cryptographic Protocols Group, TU Darmstadt
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
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>

class RcvThread;
class SndThread;
struct rcv_ctx;
class CEvent;
class CLock;

class channel {
public:
	channel(uint8_t channelid, RcvThread* rcver, SndThread* snder);

	~channel();

	void send(uint8_t* buf, uint64_t nbytes);

	void blocking_send(CEvent* eventcaller, uint8_t* buf, uint64_t nbytes);

	void send_id_len(uint8_t* buf, uint64_t nbytes, uint64_t id, uint64_t len);

	void blocking_send_id_len(CEvent* eventcaller, uint8_t* buf, uint64_t nbytes, uint64_t id, uint64_t len);

	//buf needs to be freed, data contains the payload
	uint8_t* blocking_receive_id_len(uint8_t** data, uint64_t* id, uint64_t* len);

    bool queue_empty() const;

	uint8_t* blocking_receive();

	void blocking_receive(uint8_t* rcvbuf, uint64_t rcvsize);

	bool is_alive();

	bool data_available();

	void signal_end();

	void wait_for_fin();

	void synchronize_end();

private:
	uint8_t m_bChannelID;
	RcvThread* m_cRcver;
	SndThread* m_cSnder;
	std::unique_ptr<CEvent> m_eRcved;
	std::unique_ptr<CEvent> m_eFin;
	bool m_bSndAlive;
	bool m_bRcvAlive;
	std::queue<rcv_ctx*>* m_qRcvedBlocks;
	std::mutex& m_qRcvedBlocks_mutex_;
};


#endif /* CHANNEL_H_ */

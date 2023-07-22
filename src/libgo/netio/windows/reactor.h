#ifndef LIBGO_WINDOWS_REACTOR_H
#define LIBGO_WINDOWS_REACTOR_H
#include "../../common/config.h"
#include "../../scheduler/processer.h"
#include <WinSock2.h>
#include <Windows.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include "win_vc_hook.h"

namespace co {

class Reactor
{
public:
	static Reactor& getInstance()
	{
		static Reactor obj;
    	return obj;
	}

public:
	Reactor(){}
    
    struct Entry {
        Processer::SuspendEntry suspendEntry_;
        std::shared_ptr<short int> revents_;
        int idx_;

        Entry() {}
        Entry(Processer::SuspendEntry const& suspendEntry,
            std::shared_ptr<short int> const& revents,
            int idx)
            : suspendEntry_(suspendEntry), revents_(revents), idx_(idx)
        {}

        friend bool operator==(Entry const& lhs, Entry const& rhs) {
            return lhs.idx_ == rhs.idx_ &&
                lhs.revents_ == rhs.revents_ &&
                lhs.suspendEntry_ == rhs.suspendEntry_;
        }
    };

    void Watch(SOCKET sock, short int pollEvent, Entry const& entry)
	{
		SelectorPtr selector;

		{
			std::unique_lock<std::mutex> lock(mtx_);
			size_t idx = sock / (1024 - 1);
			assert(idx < 2048);  // protect
			for (size_t i = selectors_.size(); i <= idx; ++i) {
				selectors_.push_back(std::make_shared<Selector>());
			}
			selector = selectors_[idx];
		}
		
		selector->Watch(sock, pollEvent, entry);
	}
	
private:
	class Interrupter
	{
		SOCKET writer_;
		SOCKET reader_;
		SOCKET accepter_;

	public:
		Interrupter()
		{
			SOCKET accepter_ = ::socket(AF_INET, SOCK_STREAM, 0);
			assert(accepter_ != INVALID_SOCKET);
			
			sockaddr_in addr;
			int addrLen = sizeof(addr);
			addr.sin_family = AF_INET;
			addr.sin_port = 0;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			int res = ::bind(accepter_, (const sockaddr*)&addr, sizeof(addr));
			(void)res;
			assert(res == 0);

			res = getsockname(accepter_, (sockaddr*)&addr, &addrLen);
			assert(res == 0);

			if (addr.sin_addr.s_addr == htonl(INADDR_ANY))
				addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

			::listen(accepter_, 1);

			reader_ = ::socket(AF_INET, SOCK_STREAM, 0);
			assert(reader_ != INVALID_SOCKET);

			res = native_connect(reader_, (const sockaddr*)&addr, addrLen);
			assert(res == 0);

			writer_ = native_accept(accepter_, (sockaddr*)&addr, &addrLen);
			assert(writer_ != INVALID_SOCKET);

			bool bRet = setNonblocking(writer_, true);
			assert(bRet);
			bRet = setNonblocking(reader_, true);
			assert(bRet);

			::closesocket(accepter_);
			accepter_ = INVALID_SOCKET;
		}
		~Interrupter()
		{
			::closesocket(accepter_);
			::closesocket(writer_);
			::closesocket(reader_);
		}

		void interrupter()
		{
			::send(writer_, "A", 1, 0);
		}

		void reset()
		{
			char buf[4096];
			while (::recv(reader_, buf, sizeof(buf), 0) < sizeof(buf));
		}

		SOCKET socket()
		{
			return reader_;
		}
	};

	class Selector
	{
		typedef std::unordered_map<SOCKET, std::list<Entry>> Sockets;
		Sockets readers_;
		Sockets writers_;
		Sockets excepters_;
		std::mutex mtx_;
		Interrupter interrupter_;
        std::thread thread_;
        volatile bool exit_;

	public:
        Selector(): exit_(false)
		{
			std::thread([this] { this->ThreadRun(); }).swap(thread_);
		}
        ~Selector()
		{
			exit_ = true;
			interrupter_.interrupter();
			if (thread_.joinable())
				thread_.join();
		}

		void Watch(SOCKET sock, short int pollEvent, Entry const& entry)
		{
			std::unique_lock<std::mutex> lock(mtx_);
			if (pollEvent & POLLIN) {
				readers_[sock].push_back(entry);
			}
			if (pollEvent & POLLOUT) {
				writers_[sock].push_back(entry);
			}
			excepters_[sock].push_back(entry);
			lock.unlock();

			interrupter_.interrupter();
		}

        void FdSet(fd_set& set, Sockets & sockets)
		{
			auto iter = sockets.begin();
			while (iter != sockets.end()) {
				auto & list = iter->second;
				while (!list.empty()) {
					if (!list.front().suspendEntry_.IsExpire()) {
						if (list.size() > 1) {
							auto entry = list.front();
							list.pop_front();
							list.push_back(entry);
						}
						break;
					}

					list.pop_front();
				}
				if (list.empty()) {
					iter = sockets.erase(iter);
					continue;
				}
				FD_SET(iter->first, &set);
				++iter;
			}
		}

		void Perform(fd_set& set, short int pollEvent, Sockets & sockets, std::set<Processer::SuspendEntry> & suspendEntries)
		{
			for (u_int i = 0; i < set.fd_count; ++i) {
				SOCKET sock = set.fd_array[i];
				auto it = sockets.find(sock);
				if (sockets.end() == it)
					continue;

				for (auto & entry : it->second) {
					entry.revents_.get()[entry.idx_] |= pollEvent;
					suspendEntries.insert(entry.suspendEntry_);
				}

				sockets.erase(it);

				if (&sockets == &excepters_) {
					readers_.erase(sock);
					writers_.erase(sock);
				}
			}
		}

		void ThreadRun()
		{
			for (;;) {
				fd_set readfds, writefds, exceptfds;
				FD_ZERO(&readfds);
				FD_ZERO(&writefds);
				FD_ZERO(&exceptfds);
				FD_SET(interrupter_.socket(), &readfds);

				{
					std::unique_lock<std::mutex> lock(mtx_);
					FdSet(readfds, readers_);
					FdSet(writefds, writers_);
					FdSet(exceptfds, excepters_);
				}
				
				int ignored = 0;
				timeval* blocking = nullptr;
				int n = native_select(ignored, &readfds, &writefds, &exceptfds, blocking);
				if (n > 0 && FD_ISSET(interrupter_.socket(), &readfds)) {
					interrupter_.reset();
					FD_CLR(interrupter_.socket(), &readfds);
					--n;

					if (exit_) return;
				}

				if (!n)
					continue;

				std::set<Processer::SuspendEntry> wakeups;

				{
					std::unique_lock<std::mutex> lock(mtx_);
					Perform(readfds, POLLIN, readers_, wakeups);
					Perform(writefds, POLLOUT, writers_, wakeups);
					Perform(exceptfds, POLLERR, excepters_, wakeups);
				}

				for (auto & suspendEntry : wakeups) {
					Processer::Wakeup(suspendEntry);
				}
			}
		}
	};

	typedef std::shared_ptr<Selector> SelectorPtr;
	std::vector<SelectorPtr> selectors_;
	std::mutex mtx_;    
};

} // namespace co
#endif // LIBGO_WINDOWS_REACTOR_H

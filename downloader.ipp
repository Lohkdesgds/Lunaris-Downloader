#pragma once
#include "downloader.h"

namespace Lunaris {

	inline bool downloader::run_get_with_func(const std::string& full, std::function<void(const char*, const size_t)> f)
	{
		if (!f) return false;
		constexpr size_t max_block_size = 512;

#ifdef LUNARIS_VERBOSE_BUILD
		PRINT_DEBUG("Downloader %p trying to download '%s'", this, full.c_str());
#endif

#ifdef LUNARIS_USE_WINDOWS_LIB

#ifdef UNICODE
		HINTERNET connect = InternetOpen(L"Lunaris Downloader V6.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
#else
		HINTERNET connect = InternetOpen("Lunaris Downloader V6.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
#endif
		if (!connect) return false;
		HINTERNET OpenAddress = InternetOpenUrlA(connect, full.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
		if (!OpenAddress) return false;


		char DataReceived[max_block_size];
		DWORD NumberOfBytesRead = 0;
		TotalBytesRead = 0;

		while (InternetReadFile(OpenAddress, DataReceived, max_block_size, &NumberOfBytesRead)) {
			if (NumberOfBytesRead <= 0) break;
			f(DataReceived, NumberOfBytesRead);
			TotalBytesRead += NumberOfBytesRead;
		}
		if (NumberOfBytesRead > 0) f(DataReceived, NumberOfBytesRead);

		InternetCloseHandle(OpenAddress);
		InternetCloseHandle(connect);

#else
		size_t p = 0;
		for (size_t j = 0; j != 3 && p != std::string::npos; ++j) p = full.find('/', p + 1); // http...// lala / < this one, the third
		if (p == std::string::npos) return false;

		std::string url = full.substr(0, p);
		std::string get = full.substr(p);
		if (get.empty()) get = "/";


		const std::string urls[] = { "https://" , "http://" };
		const char delimiter[] = "\r\n\r\n";

		std::string nohttp = url;
		for (const auto& each : urls) {
			if (nohttp.find(each) == 0 && nohttp.size() > each.size()) {
				nohttp = nohttp.substr(each.size());
			}
		}

		TCP_client client;

		if (!client.setup(socket_config().set_ip_address(nohttp).set_port(80)) &&
			!client.setup(socket_config().set_ip_address(nohttp).set_port(443)) &&
			!client.setup(socket_config().set_ip_address(nohttp).set_port(8080)))
			return {};


		const std::string formatt = "GET " + get + " HTTP/1.1\r\nHost: " + nohttp + "\r\nConnection: close\r\n\r\n";

		if (!client.send(formatt.data(), formatt.size())) return {};

		std::string _filter;
		bool found_rnrn = false, got_smth = false;

		size_t emptys = 0;
		while (client.has_socket() && client.valid())
		{
			auto _tmp = client.recv(max_block_size, false);
			got_smth |= (_tmp.size() != 0);

			if (_tmp.empty()) {
				if (!got_smth && ++emptys >= 500) break;
				else if (++emptys >= 50) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
			else if (found_rnrn)
			{
				TotalBytesRead += _tmp.size();
				f(_tmp.data(), _tmp.size());
			}
			else {
				emptys = 0;
				_filter.insert(_filter.end(), _tmp.begin(), _tmp.end());
				size_t p = _filter.find(delimiter);
				if (p != std::string::npos) {
					_filter = _filter.substr(p + 4); // offset
					found_rnrn = true;
					if (_filter.size()) {
						f(_filter.data(), _filter.size());
						TotalBytesRead += _filter.size();
					}
				}
			}
		}

		if (!got_smth) return false;
#endif

#ifdef LUNARIS_VERBOSE_BUILD
		PRINT_DEBUG("Downloader %p downloaded '%s'", this, full.c_str());
#endif
		return true;
	}

	inline bool downloader::get(const std::string& url)
	{
		buf.clear();
		return run_get_with_func(url, [&](const char* feed, const size_t feed_siz) { buf.insert(buf.end(), feed, feed + feed_siz); });
	}

	inline bool downloader::get_store(const std::string& url, std::function<void(const char*, const size_t)> f)
	{
		buf.clear();
		return run_get_with_func(url, f);
	}

	inline size_t downloader::bytes_read() const
	{
		return TotalBytesRead;
	}

	inline const std::vector<char>& downloader::read() const
	{
		return buf;
	}

	inline void downloader::clear_buf()
	{
		buf.clear();
	}

	inline bool downloader::valid() const
	{
		return buf.size() != 0;
	}

	inline bool downloader::empty() const
	{
		return buf.empty();
	}

}
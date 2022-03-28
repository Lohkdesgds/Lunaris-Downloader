#pragma once

//#include <Lunaris/__macro/macros.h>

#ifdef LUNARIS_USE_WINDOWS_LIB
#include <windows.h>
#include <wininet.h>
#include <ShlObj.h>
#include <string>
#include <vector>
#include <thread>
#include <functional>

#pragma comment (lib, "wininet.lib")

#else

#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <thread>
#include <vector>
#include <functional>

#if defined __has_include
	#if __has_include(<Lunaris-Socket/socket.h>) // default path expected, like /libs/* < your git clones/submodules here
		#include <Lunaris-Socket/socket.h>
	#elif __has_include(<Lunaris/Utility/socket.h>) // expected on huge Lunaris combo path
		#include <Lunaris/Utility/socket.h>
	#elif __has_include("../Lunaris-Socket/socket.h") // maybe relative path works for you
		#include "../Lunaris-Socket/socket.h"
	#elif __has_include("../Socket/socket.h") // maybe you called it Socket
		#include "../Socket/socket.h"
	#elif defined(LUNARIS_SOCKET_PATH) __has_include(LUNARIS_SOCKET_PATH) // Maybe you're just crazy and use your own system. Set this path related to this file and you're fine. Example: '#define LUNARIS_SOCKET_PATH <Lunaris-Socket/socket.h>'
		#include LUNARIS_SOCKET_PATH
	#elif __has_include("socket.h") // maybe you just don't like using folders
		#include "socket.h"
	#else // Sorry, but I can't help you if you do this
		#error "Install Lunaris Socket and add to include path or install alongside the Lunaris-Socket folder"
	#endif
#else
	#error "Your environment doesn't support __has_include. Try using C++17 or newer."
#endif

#endif

namespace Lunaris {

	/// <summary>
	/// <para>downloader is a tool to download files from the internet. It seems to work on most standard URLS, like discord's ones and others without many 'steps'</para>
	/// <para>It is able to store in a buffer internally or directly flush to somewhere using a function (called one or more times).</para>
	/// <para>If you're on Windows you may want to define LUNARIS_USE_WINDOWS_LIB, because WinInet generally works better. The default is manual Sockets way.</para>
	/// <para>NOTE: You NEED Lunaris-Socket if you're not on Windows with LUNARIS_USE_WINDOWS_LIB defined.</para>
	/// </summary>
	class downloader {
		std::vector<char> buf;
		size_t TotalBytesRead = 0;

		bool run_get_with_func(const std::string&, std::function<void(const char*, const size_t)>);
	public:
		/// <summary>
		/// <para>Download content from this URL and stores in a internal Buffer.</para>
		/// </summary>
		/// <param name="{std::string}">The URL.</param>
		/// <returns>{bool} True if succeess.</returns>
		bool get(const std::string&);

		/// <summary>
		/// <para>Download content from this URL and stores directly using the function.</para>
		/// </summary>
		/// <param name="{std::string}">The URL.</param>
		/// <param name="{std::function}">The function receiving the download feed (in chunks).</param>
		/// <returns>{bool} True if succeess.</returns>
		bool get_store(const std::string&, std::function<void(const char*, const size_t)>);

		/// <summary>
		/// <para>Total bytes in memory right now (might not be 100% accurate).</para>
		/// </summary>
		/// <returns>{size_t} Total bytes in memory.</returns>
		size_t bytes_read() const;

		/// <summary>
		/// <para>Reads data from Buffer.</para>
		/// </summary>
		/// <returns>{Buffer} Constant reference to Buffer.</returns>
		const std::vector<char>& read() const;

		/// <summary>
		/// <para>Clears internal buffer.</para>
		/// </summary>
		void clear_buf();

		/// <summary>
		/// <para>If it has something saved internallly, it's considered VALID (to read()).</para>
		/// <para>This doesn't work if you use get_store().</para>
		/// </summary>
		/// <returns>{bool} True if has content to read.</returns>
		bool valid() const;

		/// <summary>
		/// <para>If there's nothing stored for read(), it's considered empty.</para>
		/// <para>Empty doesn't mean fail (unless you tried get() and it didn't get any byte).</para>
		/// </summary>
		/// <returns>{bool} True if buffer is empty.</returns>
		bool empty() const;
	};

}

#include "downloader.ipp"
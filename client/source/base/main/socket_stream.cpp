#include "pch.h"
#include "socket_stream.h"



#ifndef WIN32
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
# ifdef SO_NOSIGPIPE
#  define USE_SO_NOSIGPIPE
# else
#  error "Cannot block SIGPIPE!"
# endif
#endif

#endif





SocketStream::SocketStream()
#ifdef WIN32
    : _socket((SOCKET)-1)
#else
    : _socket(-1)
#endif
    , _connectionIsClosed(false)
    , _totalBytes(0)
{
}

SocketStream::~SocketStream()
{
    close();
}

SocketStream * SocketStream::create(const char * ipAddress, uint16_t port, bool blocking)
{
#ifdef WIN32

    static wchar_t errorBuf[1024];

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
    {
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, result, 0, errorBuf, 1024, NULL);
        GP_WARN("Can't create SocketStream %s:%d %d", ipAddress, port, result);
        return NULL;
    }

    SOCKET socket;

    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == -1)
    {
        int error = WSAGetLastError();
        WSACleanup();
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorBuf, 1024, NULL);
        GP_WARN("Can't create SocketStream %s:%d %d", ipAddress, port, result);
        return NULL;
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ipAddress);
    clientService.sin_port = htons(port);

    result = ::connect(socket, (SOCKADDR *)& clientService, sizeof(clientService));
    GP_LOG("Connecting to %s:%d...", ipAddress,port);
    if (result == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        closesocket(socket);
        WSACleanup();
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorBuf, 1024, NULL);
        GP_WARN("Can't create SocketStream %s:%d %d", ipAddress, port, result);
        return NULL;
    }

#else

    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == -1)
    {
        GP_WARN("Can't create SocketStream %s:%d %d", ipAddress, port, 0);
        return NULL;
    }

    int set = 1;
#ifdef USE_SO_NOSIGPIPE
    setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set));
#endif

    set = 1;
    setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &set, sizeof(set));
#ifdef __EMSCRIPTEN__
    set = 1;
    setsockopt(socket, IPPROTO_TCP, TCP_QUICKACK, &set, sizeof(set));
#endif

    struct sockaddr_in clientService;
    memset(&clientService, 0, sizeof(clientService));
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ipAddress);
    clientService.sin_port = htons(port);

    int res = ::connect(socket, (sockaddr*)&clientService, sizeof(clientService));
    GP_LOG("Connecting to %s:%d...", ipAddress, port);
    if (res < 0)
    {
        int error = (int)errno;
        close(socket);
        socket = -1;
        GP_WARN("Can't create SocketStream %s:%d %d", ipAddress, port, error);
        return NULL;
    }

#endif

    SocketStream * res = new SocketStream();
    res->_socket = socket;

    return res;
}

void SocketStream::close()
{
    _connectionIsClosed = true;
    if (_socket == -1)
        return;

#ifdef WIN32
    closesocket(_socket);
    WSACleanup();
    _socket = (SOCKET)-1;
#else
    close(_socket);
    _socket = -1;
#endif
}

size_t SocketStream::read(void* ptr, size_t size, size_t count)
{
    GP_ASSERT(!_connectionIsClosed);
    if (size == 0 || count == 0)
        return 0;

    size_t sizeInBytes = size * count;
    size_t bytesRemaining = sizeInBytes;
    char * buffer = (char*)ptr;

    auto result = recv(_socket, buffer, bytesRemaining, 0);
    if (result > 0)
        bytesRemaining -= result;

    _connectionIsClosed |= result <= 0;
    _totalBytes += sizeInBytes - bytesRemaining;

    return (sizeInBytes - bytesRemaining) / size;
}

char* SocketStream::readLine(char* str, int num)
{
    if (num <= 0)
        return NULL;

    char ch;
    char * strSave = str;

    size_t maxReadBytes = num;
    while (maxReadBytes > 0)
    {
        if (read(&ch, 1, 1) != 1)
            return NULL;

        if (ch == '\r' || ch == '\n')
        {
            *str++ = ch;
            if (str - strSave < num)
                *str++ = '\0';
            return strSave;
        }

        *str++ = ch;
        maxReadBytes--;
    }

    if (str - strSave < num)
        *str++ = '\0';
    return strSave;
}

size_t SocketStream::write(const void* ptr, size_t size, size_t count)
{
    GP_ASSERT(!_connectionIsClosed);

#ifdef WIN32
    int result = send(_socket, reinterpret_cast<const char *>(ptr), size * count, 0);
#else
    int result = send(_socket, ptr, size * count, 0);
#endif

    if (result < 0)
        return 0;

    _totalBytes += result;
    return result / size;
}

bool SocketStream::canRead()
{
    unsigned long bytes = 0;
#ifdef WIN32
    ioctlsocket(_socket, FIONREAD, &bytes);
#else
    ioctl(_socket, FIONREAD, &bytes);
#endif

    return bytes != 0;
}
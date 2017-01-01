#pragma once


#ifndef __DFG_SOCKET_STREAM_H__
#define __DFG_SOCKET_STREAM_H__

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif




/**
 * SocketStream handles data transmission over the network 
 * using stream-like objects.
 *
 * You can use SocketStream only with TCP sockets, either
 * blocking or non-blocking.
 */
class SocketStream : public gameplay::Stream, Noncopyable
{
public:
    virtual ~SocketStream();

    /**
     * Create SocketServer for a given IPv4 address and port number.
     * Socket connection is always happen in blocking mode.
     *
     * If socket is non-blocking, read and write operations may fail if 
     * no data is ready in the pipe. You need to check EOF to make sure
     * socket is closed and retry reading or writing again if it's not.
     *
     * Non-blocking sockets are not supported at the moment.
     *
     * @param ipAddress IPv4 address.
     * @param port Port number.
     * @param blocking Use blocking read-write operations.
     * @return Newly created SocketStream.
     */
    static SocketStream * create(const char * ipAddress, uint16_t port, bool blocking = true);

    /**
     * Returns true if this stream can perform read operations.
     * Actually checks whether any bytes are available to make socket read operation
     *
     * @return True if the stream can read, false otherwise.
     */
    virtual bool canRead() override;

    /**
     * Returns true if this stream can perform write operations.
     *
     * @return True if the stream can write, false otherwise.
     */
    virtual bool canWrite() override { return true; };

    /**
     * Returns true if this stream can seek.
     *
     * @return True if the stream can seek, false otherwise.
     */
    virtual bool canSeek() override { return false; };

    /**
     * Closes this stream.
     */
    virtual void close() override;

    /**
     * Reads an array of <code>count</code> elements, each of size <code>size</code>.
     *
     * \code
     * int numbers[3];
     * if (stream->read(numbers, sizeof(int), 3) != 3)
     *     print("Error reading from file");
     * \endcode
     *
     * @param ptr   The pointer to the memory to copy into.
     *              The available size should be at least (<code>size * count</code>) bytes.
     * @param size  The size of each element to be read, in bytes.
      * @param count The number of elements to read.
     *
     * @return The number of elements read.
     *
     * @see canRead()
     */
    virtual size_t read(void* ptr, size_t size, size_t count) override;

    /**
     * Reads a line from the stream.
     *
     * A new line is denoted by by either "\n", "\r" or "\r\n".
     * The line break character is included in the string.
     * The terminating null character is added to the end of the string.
     *
     * @param str The array of chars to copy the string to.
     * @param num The maximum number of characters to be copied.
     *
     * @return On success, str is returned. On error, NULL is returned.
     *
     * @see canRead()
     */
    virtual char* readLine(char* str, int num) override;

    /**
     * Writes an array of <code>count</code> elements, each of size <code>size</code>.
     *
     * \code
     * int numbers[] = {1, 2, 3};
     * if (stream->write(numbers, sizeof(int), 3) != 3)
     *     print("Error writing to file");
     * \endcode
     *
     * @param ptr   The pointer to the array of elements to be written.
     * @param size  The size of each element to be written, in bytes.
     * @param count The number of elements to write.
     *
     * @return The number of elements written.
     *
     * @see canWrite()
     */
    virtual size_t write(const void* ptr, size_t size, size_t count) override;

    /**
     * Returns true if the end of the stream has been reached.
     *
     * @return True if end of stream reached, false otherwise.
     */
    virtual bool eof() override { return _connectionIsClosed; };

    /**
     * Returns the length of the stream in bytes.
     *
     * Zero is returned if the length of the stream is unknown and/or it cannot be seeked.
     *
     * Example: The length of a network stream is unknown and cannot be seeked.
     *
     * @return The length of the stream in bytes.
     */
    virtual size_t length() override { return 0; };

    /**
     * Returns the position of the file pointer. Zero is the start of the stream.
     * For network stream total number of bytes send and received is returned.
     *
     * @return The file indicator offset in bytes.
     */
     virtual long int position() override { return _totalBytes; };

    /**
     * Sets the position of the file pointer.
     *
     * Use canSeek() to determine if this method is supported.
     *
     * @param offset The number of bytes to offset from origin.
     * @param origin The position used as a reference for offset.
     *               The supported values are the same as fseek().
     *                - <code>SEEK_SET</code> relative to the beginning of the file.
     *                - <code>SEEK_CUR</code> relative to the current position of the file pointer.
     *                - <code>SEEK_END</code> relative to the end of file.
     *
     * @return True if successful, false otherwise.
     *
     * @see canSeek()
     */
    virtual bool seek(long int offset, int origin) override { return false; };

    /**
     * Moves the file pointer to the start of the file.
     *
     * Use canSeek() to determine if this method is supported.
     *
     * @return True if successful, false otherwise.
     *
     * @see canSeek()
     */
    virtual bool rewind() override { return false; };

protected:
    SocketStream();

private:
#ifdef WIN32
    SOCKET _socket;
#else
    int _socket;
#endif

    bool _connectionIsClosed;
    long int _totalBytes;
};



#endif // __DFG_SOCKET_STREAM_H__
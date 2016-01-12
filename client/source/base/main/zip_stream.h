#pragma once

#ifndef __ZIP_STREAM_H__
#define __ZIP_STREAM_H__

#include "memory_stream.h"




/** ZipStream is an extension to gameplay::Stream to stream
 *  data from zip packages. Uses ZipPackagesCache.
 */
class ZipStream : public gameplay::Stream
{
public:
    virtual ~ZipStream();

    /**
     * Creates ZipStream for given package and fileName.
     * If package is not given (empty or NULL), creates gameplay::Stream for fileName.
     *
     * @param packageName Zip package filename.
     * @param fileName File inside the package.
     */
    static gameplay::Stream * create(const char * packageName, const char * fileName);

    /**
     * Returns true if this stream can perform read operations.
     *
     * @return True if the stream can read, false otherwise.
     */
    virtual bool canRead() { return _underlyingStream && _underlyingStream->canRead(); };

    /**
     * Returns true if this stream can perform write operations.
     *
     * @return True if the stream can write, false otherwise.
     */
    virtual bool canWrite() { return _underlyingStream && _underlyingStream->canWrite(); };

    /**
     * Returns true if this stream can seek.
     *
     * @return True if the stream can seek, false otherwise.
     */
    virtual bool canSeek() { return _underlyingStream && _underlyingStream->canSeek(); };

    /**
     * Closes this stream.
     */
    virtual void close();

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
    virtual size_t read(void* ptr, size_t size, size_t count);

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
    virtual char* readLine(char* str, int num);

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
    virtual size_t write(const void* ptr, size_t size, size_t count);

    /**
     * Returns true if the end of the stream has been reached.
     *
     * @return True if end of stream reached, false otherwise.
     */
    virtual bool eof() { return !_underlyingStream || _underlyingStream->eof(); };

    /**
     * Returns the length of the stream in bytes.
     *
     * Zero is returned if the length of the stream is unknown and/or it cannot be seeked.
     *
     * Example: The length of a network stream is unknown and cannot be seeked.
     *
     * @return The length of the stream in bytes.
     */
    virtual size_t length() { return _underlyingStream ? _underlyingStream->length() : 0; };

    /**
     * Returns the position of the file pointer. Zero is the start of the stream.
     *
     * @return The file indicator offset in bytes.
     */
    virtual long int position() { return _underlyingStream ? _underlyingStream->position() : 0; };

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
    virtual bool seek(long int offset, int origin);

    /**
     * Moves the file pointer to the start of the file.
     *
     * Use canSeek() to determine if this method is supported.
     *
     * @return True if successful, false otherwise.
     *
     * @see canSeek()
     */
    virtual bool rewind();

protected:
    ZipStream();

private:
    std::unique_ptr<MemoryStream> _underlyingStream;
    std::unique_ptr< uint8_t[] > _fileContent;
    static std::mutex _zipReadMutex;
};




#endif
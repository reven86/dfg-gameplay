#pragma once

#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__




class MemoryStream : public gameplay::Stream
{
public:
    virtual ~MemoryStream();

    /**
     * Create read-only MemoryStream stream, passing pointer to buffer of constant data.
     *
     * @param buffer Buffer used to read data from.
     * @param bufferSize Size of the buffer.
     * @return Newly created MemoryStream.
     */
    static MemoryStream * create(const void * buffer, size_t bufferSize);

    /**
     * Create read-write MemoryStream stream, passing pointer to buffer of data.
     *
     * @param buffer Buffer used to read data from and write data to.
     * @param bufferSize Size of the buffer.
     * @return Newly created MemoryStream.
     */
    static MemoryStream * create(void * buffer, size_t bufferSize);

    /**
     * Create read-write MemoryStream stream, passing smart pointer to buffer of data.
     *
     * @param buffer Buffer used to read data from. Ownership of memory allocated is transferred to MemoryStream.
     * @param bufferSize Size of the buffer.
     * @return Newly created MemoryStream.
     */
    static MemoryStream * create(std::unique_ptr<uint8_t[]>& buffer, size_t bufferSize);

    /**
     * Create read-write MemoryStream stream, that automatically reallocates memory when trying to write beyond buffer's end.
     * This type of stream is useful when you need to serialize some structure in memory but
     * don't know how much space it would need. You can call write methods on this type of
     * stream and then get the pointer to result buffer when you finish all writing operations.
     *
     * @return Newly created MemoryStream.
     */
    static MemoryStream * create();

    /**
     * Returns true if this stream can perform read operations.
     *
     * @return True if the stream can read, false otherwise.
     */
    virtual bool canRead() { return true; };

    /**
     * Returns true if this stream can perform write operations.
     *
     * @return True if the stream can write, false otherwise.
     */
    virtual bool canWrite() { return _writeBuffer != nullptr || _canAllocate; };

    /**
     * Returns true if this stream can seek.
     *
     * @return True if the stream can seek, false otherwise.
     */
    virtual bool canSeek() { return true; };

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
    virtual bool eof() { return _cursor >= _bufferSize; };

    /**
     * Returns the length of the stream in bytes.
     *
     * Zero is returned if the length of the stream is unknown and/or it cannot be seeked.
     *
     * Example: The length of a network stream is unknown and cannot be seeked.
     *
     * @return The length of the stream in bytes.
     */
    virtual size_t length() { return _bufferSize; };

    /**
     * Returns the position of the file pointer. Zero is the start of the stream.
     *
     * @return The file indicator offset in bytes.
     */
    virtual long int position() { return static_cast<long int>(_cursor); };

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
    virtual bool rewind() { _cursor = 0; return true; };

    /**
     * Get underlying memory buffer.
     */
    const uint8_t * getBuffer() const { return _readBuffer; };

protected:
    MemoryStream();

private:
    const uint8_t * _readBuffer;
    uint8_t * _writeBuffer;
    size_t _cursor;
    size_t _bufferSize;
    std::unique_ptr<uint8_t[]> _ownedBuffer;

    bool _canAllocate;
    std::vector<uint8_t> _autoBuffer;
};




#endif
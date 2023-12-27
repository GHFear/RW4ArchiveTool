#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <cassert>

#include "mspack/mspack.h"
#include "mspack/lzx.h"

struct mspack_file
{
	byte* buf;
	int			bufSize;
	int			pos;
	int			rest;
};

static int mspack_read(mspack_file* file, void* buffer, int bytes)
{

	if (!file->rest)
	{
		// read block header
		if (file->buf[file->pos] == 0xFF)
		{
			// [0]   = FF
			// [1,2] = uncompressed block size
			// [3,4] = compressed block size
			file->rest = (file->buf[file->pos + 3] << 8) | file->buf[file->pos + 4];
			file->pos += 5;
		}
		else
		{
			// [0,1] = compressed size
			file->rest = (file->buf[file->pos + 0] << 8) | file->buf[file->pos + 1];
			file->pos += 2;
		}
		if (file->rest > file->bufSize - file->pos)
			file->rest = file->bufSize - file->pos;
	}
	if (bytes > file->rest) bytes = file->rest;
	if (bytes <= 0) return 0;

	// copy block data
	memcpy(buffer, file->buf + file->pos, bytes);
	file->pos += bytes;
	file->rest -= bytes;

	return bytes;
}

static int mspack_write(mspack_file* file, void* buffer, int bytes)
{
	assert(file->pos + bytes <= file->bufSize);
	memcpy(file->buf + file->pos, buffer, bytes);
	file->pos += bytes;
	return bytes;
}

static void* mspack_alloc(mspack_system* self, size_t bytes)
{
	return malloc(bytes);
}

static void mspack_free(void* ptr)
{
	free(ptr);
}

static void mspack_copy(void* src, void* dst, size_t bytes)
{
	memcpy(dst, src, bytes);
}

static struct mspack_system lzxSys =
{
	NULL,				// open
	NULL,				// close
	mspack_read,
	mspack_write,
	NULL,				// seek
	NULL,				// tell
	NULL,				// message
	mspack_alloc,
	mspack_free,
	mspack_copy
};

static bool appDecompressLZX(byte* CompressedBuffer, int CompressedSize, byte* UncompressedBuffer, int UncompressedSize)
{

	// setup streams
	mspack_file src, dst;
	src.buf = CompressedBuffer;
	src.bufSize = CompressedSize;
	src.pos = 0;
	src.rest = 0;
	dst.buf = UncompressedBuffer;
	dst.bufSize = UncompressedSize;
	dst.pos = 0;
	// prepare decompressor
	lzxd_stream* lzxd = lzxd_init(&lzxSys, &src, &dst, 17, 0, 256 * 1024, UncompressedSize);
	assert(lzxd);
	// decompress
	int r = lzxd_decompress(lzxd, UncompressedSize);

	// free resources
	lzxd_free(lzxd);

	if (r != MSPACK_ERR_OK)
	{
		return false;
	}
	
	return true;
}
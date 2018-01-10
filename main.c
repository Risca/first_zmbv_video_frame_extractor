#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 64

struct Buffer {
	union {
		char array[BUFFER_SIZE];
		struct {
			char first[BUFFER_SIZE/2];
			char second[BUFFER_SIZE/2];
		};
	};
};

struct ZMBV_intraframe {
	char major;
	char minor;
	char compression;
	char format;
	char width;
	char height;
};

int ReadFromStdIn(struct Buffer* buf)
{
	memmove(buf->first, buf->second, BUFFER_SIZE/2);
	return read(STDIN_FILENO, buf->second, BUFFER_SIZE/2);
}

int main(int argc, char* argv[])
{
	struct Buffer buf;
	int rv = read(STDIN_FILENO, buf.array, BUFFER_SIZE);
	while (rv > 0) {
		const char* n = memmem(buf.array, BUFFER_SIZE, "movi00dc", 8);
		if (n) {
			n += 8;
			if (n >= buf.array + BUFFER_SIZE) {
				rv = ReadFromStdIn(&buf);
				n -= BUFFER_SIZE/2;
			}
			if (rv > 0) {
				uint32_t len = *(uint32_t*)n;
				n += 4;
				char* frame_buf = malloc(len);
				if (frame_buf) {
					ssize_t bytes_read = buf.array + BUFFER_SIZE - n;
					memcpy(frame_buf, n, bytes_read);
					rv = read(STDIN_FILENO, frame_buf + bytes_read, len - bytes_read);
					if (rv > 0) {
						rv = -1;
						if (*frame_buf & 0x01 == 0x01) {
							struct ZMBV_intraframe* frame = (struct ZMBV_intraframe*)(frame_buf+1);
							if (frame->major == 0 && frame->minor == 1 &&
							    frame->compression == 1 && frame->format == 4)
							{
								rv = write(STDOUT_FILENO, frame_buf + 7, len - 7);
							}
						}
					}
					free(frame_buf);
					return rv > 0 ? 0 : -1;
				}
			}
			return -1;
		}
		rv = ReadFromStdIn(&buf);
	}
	return 0;
}


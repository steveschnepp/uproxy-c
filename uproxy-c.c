/*
 * micro HTTP caching proxy
 * (c) 2016 - MIT - Steve Schnepp <steve.schnepp@pwkf.org>
 *
 * It uses only POSIX components modules, and is aimed for low perf hardware,
 * such as embedded NAS, and when installing deps is cumbersome.
 *
 * It should also work with various lighter libc, along with a full static mode.
 */

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "lib/mkpath.h"

/* Size constants */
const size_t SIZE_X = 4;
const size_t SIZE_S = 256;
const size_t SIZE_M = 4 * 1024; // 4kiB
const size_t SIZE_L = 64 * 1024; // 64kiB
const size_t SIZE_H = 1024 * 1024; // 1MiB

const int BUFFER_SIZE = 64 * 1024; // 64k buffer
static char CACHEDIR[] = "cache"; // default is $(pwd)/cache
static int PORT = 3128;
static int debug = 1;

struct header_s {
	char key[SIZE_S];
	char value[SIZE_M];
};

struct request_s {
	char method[SIZE_S];
	char url[SIZE_M];
	char version[SIZE_X];
};

struct url_s {
	char scheme[SIZE_S];
	char host[SIZE_S];
	int port;

};

struct request_s parse_request();
struct header_s* parse_headers();
struct url_s parse_url();
void _connect(char*);

int main(int argc, char** argv)
{
	// Make & go to CACHEDIR
	mkpath(CACHEDIR);
	chdir(CACHEDIR);

	// Do not die when child disconnects
	signal(SIGPIPE, SIG_IGN);

	// We assume we are started from inetd for now
	struct request_s request = parse_request();

	// method CONNECT bypasses everything
	if (strcmp("CONNECT", request.method) == 0) {
		_connect(request.url);
		return 0;
	}

	// Support only GET
	if (strcmp("GET", request.method) != 0) {
		return 0;
	}

	// Fetch the request headers
	struct header_s* headers = parse_headers();

	// Parse the URL
	struct url_s url = parse_url(request.url);

	// Support only simple HTTP
	if (strcmp("http", url.scheme) != 0) {
		return 0;
	}


}

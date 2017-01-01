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

#include <string.h>
#include <sys/stat.h>

#include "lib/mkpath.h"

const int BUFFER_SIZE = 64 * 1024; // 64k buffer
static char CACHEDIR[] = "cache"; // default is $(pwd)/cache
static int PORT = 3128;
static int debug = 1;

struct header_s {
	char key[];
	char value[];
}

struct request_s {
	char method[];
	char url[];
	char version[];
}

struct url_s {
	char scheme[];
	char host[];
	int port;

}

static void main(int argc, char** argv)
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
		return;
	}

	// Support only GET
	if (strcmp("GET", request.method) != 0) {
		return;
	}

	// Fetch the request headers
	struct header_s headers[] = parse_headers();

	// Parse the URL
	struct url_s url = parse_url(request.url);

	// Support only simple HTTP
	if (strcmp("http", url.scheme) != 0) {
		return;
	}


}

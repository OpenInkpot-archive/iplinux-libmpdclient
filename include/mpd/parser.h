/* libmpdclient
   (c) 2003-2009 The Music Player Daemon Project
   This project's homepage is: http://www.musicpd.org

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Music Player Daemon nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LIBMPDCLIENT_PARSER_H
#define LIBMPDCLIENT_PARSER_H

#include <mpd/protocol.h>

#include <stdbool.h>

enum mpd_parser_result {
	/**
	 * Response line was not understood.
	 */
	MPD_PARSER_MALFORMED,

	/**
	 * MPD has returned "OK" or "list_OK" (check with
	 * mpd_parser_is_partial()).
	 */
	MPD_PARSER_SUCCESS,

	/**
	 * MPD has returned "ACK" with an error code.  Call
	 * mpd_parser_get_reason() to get the error code.
	 */
	MPD_PARSER_ERROR,

	/**
	 * MPD has returned a name-value pair.  Call
	 * mpd_parser_get_name() and mpd_parser_get_value().
	 */
	MPD_PARSER_PAIR,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates a new mpd_parser object.  Returns NULL on error (out of
 * memory).
 */
struct mpd_parser *
mpd_parser_new(void);

/**
 * Frees a mpd_parser object.
 */
void
mpd_parser_free(struct mpd_parser *parser);

/**
 * Feeds a line (without the trailing newline character) received from
 * MPD / mpd_async_recv_line() into the parser.
 */
enum mpd_parser_result
mpd_parser_feed(struct mpd_parser *parser, char *line);

/**
 * Call this when mpd_parser_feed() has returned #MPD_PARSER_SUCCESS
 * to find out whether this is an "OK" (false) or a "list_OK" (true)
 * response.
 */
bool
mpd_parser_is_partial(const struct mpd_parser *parser);

/**
 * Call this when mpd_parser_feed() has returned #MPD_PARSER_ERROR to
 * obtain the reason for the error.
 */
enum mpd_ack
mpd_parser_get_ack(const struct mpd_parser *parser);

/**
 * On #MPD_PARSER_ERROR, this returns the number of the list command
 * which failed, or -1 if that information is not available.
 */
int
mpd_parser_get_at(const struct mpd_parser *parser);

/**
 * On #MPD_PARSER_ERROR, this returns the human readable error message
 * returned by MPD (UTF-8).
 */
const char *
mpd_parser_get_message(const struct mpd_parser *parser);

/**
 * On #MPD_PARSER_PAIR, this returns the name.
 */
const char *
mpd_parser_get_name(const struct mpd_parser *parser);

/**
 * On #MPD_PARSER_PAIR, this returns the value.
 */
const char *
mpd_parser_get_value(const struct mpd_parser *parser);

#ifdef __cplusplus
}
#endif

#endif
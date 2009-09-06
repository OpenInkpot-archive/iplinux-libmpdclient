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

#ifndef MPD_OUTPUT_H
#define MPD_OUTPUT_H

#include <stdbool.h>

struct mpd_connection;

/**
 * \struct mpd_output
 *
 * This type represents an audio output device on the MPD server.
 */
struct mpd_output;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends the "outputs" command to MPD.  Call mpd_output_get_next() to
 * read the response.
 *
 * @param connection A valid and connected mpd_connection.
 * @return true on success
 */
bool
mpd_send_outputs(struct mpd_connection *connection);

/**
 * Reads the next mpd_output from the MPD response.  Free the return
 * value with mpd_output_free().
 *
 * @return a mpd_output object on success, NULL on error or
 * end-of-response
 */
struct mpd_output *
mpd_recv_output(struct mpd_connection *connection);

/**
 * Frees a mpd_output object returned from mpd_output_get_next().
 */
void
mpd_output_free(struct mpd_output *output);

/**
 * @return the id of the specified #mpd_output object
 */
unsigned
mpd_output_get_id(const struct mpd_output *output);

/**
 * @return the configured name of the specified #mpd_output object
 */
const char *
mpd_output_get_name(const struct mpd_output *output);

/**
 * @return true if this output is enabled
 */
bool
mpd_output_get_enabled(const struct mpd_output *output);

#ifdef __cplusplus
}
#endif

#endif

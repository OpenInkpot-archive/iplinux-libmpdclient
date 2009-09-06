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

#ifndef MPD_CONNECTION_H
#define MPD_CONNECTION_H

#include <mpd/protocol.h>
#include <mpd/error.h>

#include <stdbool.h>

/**
 * \struct mpd_connection
 *
 * This opaque object represents a connection to a MPD server.  Call
 * mpd_connection_new() to create a new instance.
 *
 * Once an error occurs which invalidates the connection, this object
 * cannot be reused for a new connection attempt.  Dispose it with
 * mpd_connection_free().
 */
struct mpd_connection;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opens a new connection to a MPD server.  Both the name server
 * lookup and the connect() call are done synchronously.  After this
 * function has returned, you should check if the connection was
 * successful with mpd_get_error().
 *
 * @param host the server's host name, IP address or Unix socket path.
 * If the resolver returns more than one IP address for a host name,
 * this functions tries all of them until one accepts the connection.
 * @param port the TCP port to connect to.  If #host is a Unix socket
 * path, this parameter is ignored.
 * @param timeout the timeout in seconds; you may modify it later with
 * mpd_connection_set_timeout()
 * @return a mpd_connection object (which may have failed to connect),
 * or NULL on out-of-memory
 */
struct mpd_connection *
mpd_connection_new(const char *host, int port, float timeout);

/**
 * Close the connection and free all memory.
 */
void mpd_connection_free(struct mpd_connection *connection);

/**
 * Sets the timeout for synchronous operations.  If the MPD server
 * does not send a response during this time span, the operation is
 * aborted by libmpdclient.
 */
void mpd_connection_set_timeout(struct mpd_connection *connection,
			      float timeout);

/**
 * Returns the libmpdclient error code.  MPD_ERROR_SUCCESS means no
 * error occured.
 */
enum mpd_error
mpd_get_error(const struct mpd_connection *connection);

/**
 * Returns the human-readable (English) libmpdclient error message.
 * Calling this function is only valid if an error really occured.
 * Check with mpd_get_error().
 */
const char *
mpd_get_error_message(const struct mpd_connection *connection);

/**
 * Returns the error code returned from the server.  Calling this
 * function is only valid if mpd_get_error() returned MPD_ERROR_ACK.
 */
enum mpd_ack
mpd_get_server_error(const struct mpd_connection *connection);

/**
 * Attempts to recover from an error condition.
 *
 * @return true on success, false if the error is fatal and cannot be
 * recovered
 */
bool
mpd_clear_error(struct mpd_connection *connection);

/**
 * Returns a three-tuple containing the major, minor and patch version
 * of the MPD server.
 */
const unsigned *
mpd_get_server_version(const struct mpd_connection *connection);

/**
 * Compares the MPD server version with the specified triple.
 *
 * @return -1 if the server is older, 1 if it is newer, 0 if it is
 * equal
 */
int
mpd_cmp_server_version(const struct mpd_connection *connection, unsigned major,
		       unsigned minor, unsigned patch);

#ifdef __cplusplus
}
#endif

#endif

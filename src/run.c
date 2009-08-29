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

#include <mpd/run.h>
#include <mpd/command.h>
#include <mpd/response.h>
#include <mpd/status.h>
#include "internal.h"

/**
 * Checks whether it is possible to run a command now.
 */
static bool
run_check(struct mpd_connection *connection)
{
	assert(connection != NULL);

	if (mpd_error_is_defined(&connection->error))
		return false;

	if (connection->sending_command_list) {
		mpd_error_code(&connection->error, MPD_ERROR_STATE);
		mpd_error_message(&connection->error,
				  "Not possible in command list mode");
		return false;
	}

	return true;
}

int
mpd_run_addid(struct mpd_connection *connection, const char *file)
{
	int id;

	if (!run_check(connection))
		return false;

	if (!mpd_send_addid(connection, file))
		return -1;

	id = mpd_recv_song_id(connection);

	if (!mpd_response_finish(connection))
		id = -1;

	return id;
}

struct mpd_status *
mpd_run_status(struct mpd_connection *connection)
{
	struct mpd_status *status;

	if (!run_check(connection) || !mpd_send_status(connection))
		return NULL;

	status = mpd_recv_status(connection);
	if (status == NULL)
		return NULL;

	if (!mpd_response_finish(connection)) {
		mpd_status_free(status);
		return NULL;
	}

	return status;
}

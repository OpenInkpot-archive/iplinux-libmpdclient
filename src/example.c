/* libmpdclient
   (c)2003-2006 by Warren Dukes (warren.dukes@gmail.com)
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

#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static void
print_tag(const struct mpd_song *song, enum mpd_tag_type type,
	  const char *label)
{
	unsigned i = 0;
	const char *value;

	while ((value = mpd_song_get_tag(song, type, i++)) != NULL)
		printf("%s: %s\n", label, value);
}

int main(int argc, char ** argv) {
	struct mpd_connection *conn;
	const char *hostname = getenv("MPD_HOST");
	const char *port = getenv("MPD_PORT");

	if(hostname == NULL)
		hostname = "localhost";
	if(port == NULL)
		port = "6600";

	conn = mpd_connection_new(hostname,atoi(port),10);

	if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
		fprintf(stderr,"%s\n", mpd_get_error_message(conn));
		mpd_connection_free(conn);
		return -1;
	}

	{
		int i;
		for(i=0;i<3;i++) {
			printf("version[%i]: %i\n",i,
			       mpd_get_server_version(conn)[i]);
		}
	}

	if(argc==1) {
		struct mpd_status * status;
		struct mpd_song *song;

		mpd_command_list_begin(conn, true);
		mpd_send_status(conn);
		mpd_send_currentsong(conn);
		mpd_command_list_end(conn);

		status = mpd_recv_status(conn);
		if (status == NULL) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		printf("volume: %i\n", mpd_status_get_volume(status));
		printf("repeat: %i\n", mpd_status_get_repeat(status));
		printf("playlist: %lli\n", mpd_status_get_playlist(status));
		printf("playlistLength: %i\n", mpd_status_get_playlist_length(status));
		if (mpd_status_get_error(status) != NULL)
			printf("error: %s\n", mpd_status_get_error(status));

		if (mpd_status_get_state(status) == MPD_STATE_PLAY ||
		    mpd_status_get_state(status) == MPD_STATE_PAUSE) {
			printf("song: %i\n", mpd_status_get_song(status));
			printf("elaspedTime: %i\n",mpd_status_get_elapsed_time(status));
			printf("totalTime: %i\n", mpd_status_get_total_time(status));
			printf("bitRate: %i\n", mpd_status_get_bit_rate(status));
			printf("sampleRate: %i\n", mpd_status_get_sample_rate(status));
			printf("bits: %i\n", mpd_status_get_bits(status));
			printf("channels: %i\n", mpd_status_get_channels(status));
		}

		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		mpd_response_next(conn);

		while ((song = mpd_recv_song(conn)) != NULL) {
			print_tag(song, MPD_TAG_FILENAME, "file");
			print_tag(song, MPD_TAG_ARTIST, "artist");
			print_tag(song, MPD_TAG_ALBUM, "album");
			print_tag(song, MPD_TAG_TITLE, "title");
			print_tag(song, MPD_TAG_TRACK, "track");
			print_tag(song, MPD_TAG_NAME, "name");
			print_tag(song, MPD_TAG_DATE, "date");

			if (mpd_song_get_time(song) != MPD_SONG_NO_TIME) {
				printf("time: %i\n", mpd_song_get_time(song));
			}
			if (mpd_song_get_pos(song) != MPD_SONG_NO_NUM) {
				printf("pos: %i\n", mpd_song_get_pos(song));
			}

			mpd_song_free(song);
		}

		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		mpd_response_finish(conn);
		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}
	
		mpd_status_free(status);
	}
	else if(argc==3 && strcmp(argv[1],"lsinfo")==0) {
		struct mpd_entity * entity;

		mpd_send_lsinfo(conn,argv[2]);
		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		while ((entity = mpd_recv_entity(conn)) != NULL) {
			const struct mpd_song *song;
			const struct mpd_directory *dir;
			const struct mpd_stored_playlist *pl;

			switch (mpd_entity_get_type(entity)) {
			case MPD_ENTITY_TYPE_UNKNOWN:
				break;

			case MPD_ENTITY_TYPE_SONG:
				song = mpd_entity_get_song(entity);
				print_tag(song, MPD_TAG_FILENAME, "file");
				print_tag(song, MPD_TAG_ARTIST, "artist");
				print_tag(song, MPD_TAG_ALBUM, "album");
				print_tag(song, MPD_TAG_TITLE, "title");
				print_tag(song, MPD_TAG_TRACK, "track");
				break;

			case MPD_ENTITY_TYPE_DIRECTORY:
				dir = mpd_entity_get_directory(entity);
				printf("directory: %s\n", mpd_directory_get_path(dir));
				break;

			case MPD_ENTITY_TYPE_PLAYLISTFILE:
				pl = mpd_entity_get_stored_playlist(entity);
				printf("playlist: %s\n",
				       mpd_stored_playlist_get_path(pl));
				break;
			}

			mpd_entity_free(entity);
		}

		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		mpd_response_finish(conn);
		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}
	}
	else if(argc==2 && strcmp(argv[1],"artists")==0) {
		char * artist;
	
		mpd_search_db_tags(conn, MPD_TAG_ARTIST);
		mpd_search_commit(conn);
		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		while((artist = mpd_get_next_tag(conn, MPD_TAG_ARTIST))) {
			printf("%s\n",artist);
			free(artist);
		}

		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}

		mpd_response_finish(conn);
		if (mpd_get_error(conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"%s\n", mpd_get_error_message(conn));
			mpd_connection_free(conn);
			return -1;
		}
	}

	mpd_connection_free(conn);

	return 0;
}

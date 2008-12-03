/* libmpdclient
   (c) 2003-2008 The Music Player Daemon Project
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

#include <mpd/entity.h>
#include "internal.h"
#include "str_pool.h"

#include <stdlib.h>
#include <string.h>

static void
mpd_initInfoEntity(mpd_InfoEntity * entity) {
	entity->info.directory = NULL;
}

static void
mpd_finishInfoEntity(mpd_InfoEntity * entity) {
	if (entity->info.directory) {
		if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY)
			mpd_freeDirectory(entity->info.directory);
		else if (entity->type == MPD_INFO_ENTITY_TYPE_SONG)
			mpd_freeSong(entity->info.song);
		else if (entity->type == MPD_INFO_ENTITY_TYPE_PLAYLISTFILE)
			mpd_freePlaylistFile(entity->info.playlistFile);
	}
}

mpd_InfoEntity *
mpd_newInfoEntity(void) {
	mpd_InfoEntity * entity = malloc(sizeof(mpd_InfoEntity));

	mpd_initInfoEntity(entity);

	return entity;
}

void
mpd_freeInfoEntity(mpd_InfoEntity * entity) {
	mpd_finishInfoEntity(entity);
	free(entity);
}

mpd_InfoEntity *
mpd_getNextInfoEntity(struct mpd_connection *connection)
{
	mpd_InfoEntity * entity = NULL;

	if (connection->doneProcessing || (connection->listOks &&
	   connection->doneListOk)) {
		return NULL;
	}

	if (!connection->returnElement) mpd_getNextReturnElement(connection);

	if (connection->returnElement) {
		if (strcmp(connection->returnElement->name,"file")==0) {
			entity = mpd_newInfoEntity();
			entity->type = MPD_INFO_ENTITY_TYPE_SONG;
			entity->info.song = mpd_newSong();
			entity->info.song->file =
				str_pool_dup(connection->returnElement->value);
		}
		else if (strcmp(connection->returnElement->name,
					"directory")==0) {
			entity = mpd_newInfoEntity();
			entity->type = MPD_INFO_ENTITY_TYPE_DIRECTORY;
			entity->info.directory = mpd_newDirectory();
			entity->info.directory->path =
				str_pool_dup(connection->returnElement->value);
		}
		else if (strcmp(connection->returnElement->name,"playlist")==0) {
			entity = mpd_newInfoEntity();
			entity->type = MPD_INFO_ENTITY_TYPE_PLAYLISTFILE;
			entity->info.playlistFile = mpd_newPlaylistFile();
			entity->info.playlistFile->path =
				str_pool_dup(connection->returnElement->value);
		}
		else if (strcmp(connection->returnElement->name, "cpos") == 0){
			entity = mpd_newInfoEntity();
			entity->type = MPD_INFO_ENTITY_TYPE_SONG;
			entity->info.song = mpd_newSong();
			entity->info.song->pos = atoi(connection->returnElement->value);
		}
		else {
			connection->error = 1;
			strcpy(connection->errorStr,"problem parsing song info");
			return NULL;
		}
	}
	else return NULL;

	mpd_getNextReturnElement(connection);
	while (connection->returnElement) {
		struct mpd_return_element *re = connection->returnElement;

		if (strcmp(re->name,"file")==0) return entity;
		else if (strcmp(re->name,"directory")==0) return entity;
		else if (strcmp(re->name,"playlist")==0) return entity;
		else if (strcmp(re->name,"cpos")==0) return entity;

		if (entity->type == MPD_INFO_ENTITY_TYPE_SONG &&
				strlen(re->value)) {
			if (!entity->info.song->artist &&
					strcmp(re->name,"Artist")==0) {
				entity->info.song->artist = str_pool_dup(re->value);
			}
			else if (!entity->info.song->album &&
					strcmp(re->name,"Album")==0) {
				entity->info.song->album = str_pool_dup(re->value);
			}
			else if (!entity->info.song->title &&
					strcmp(re->name,"Title")==0) {
				entity->info.song->title = str_pool_dup(re->value);
			}
			else if (!entity->info.song->track &&
					strcmp(re->name,"Track")==0) {
				entity->info.song->track = str_pool_dup(re->value);
			}
			else if (!entity->info.song->name &&
					strcmp(re->name,"Name")==0) {
				entity->info.song->name = str_pool_dup(re->value);
			}
			else if (entity->info.song->time==MPD_SONG_NO_TIME &&
					strcmp(re->name,"Time")==0) {
				entity->info.song->time = atoi(re->value);
			}
			else if (entity->info.song->pos==MPD_SONG_NO_NUM &&
					strcmp(re->name,"Pos")==0) {
				entity->info.song->pos = atoi(re->value);
			}
			else if (entity->info.song->id==MPD_SONG_NO_ID &&
					strcmp(re->name,"Id")==0) {
				entity->info.song->id = atoi(re->value);
			}
			else if (!entity->info.song->date &&
					strcmp(re->name, "Date") == 0) {
				entity->info.song->date = str_pool_dup(re->value);
			}
			else if (!entity->info.song->genre &&
					strcmp(re->name, "Genre") == 0) {
				entity->info.song->genre = str_pool_dup(re->value);
			}
			else if (!entity->info.song->composer &&
					strcmp(re->name, "Composer") == 0) {
				entity->info.song->composer = str_pool_dup(re->value);
			}
			else if (!entity->info.song->performer &&
					strcmp(re->name, "Performer") == 0) {
				entity->info.song->performer = str_pool_dup(re->value);
			}
			else if (!entity->info.song->disc &&
					strcmp(re->name, "Disc") == 0) {
				entity->info.song->disc = str_pool_dup(re->value);
			}
			else if (!entity->info.song->comment &&
					strcmp(re->name, "Comment") == 0) {
				entity->info.song->comment = str_pool_dup(re->value);
			}
		}
		else if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
		}
		else if (entity->type == MPD_INFO_ENTITY_TYPE_PLAYLISTFILE) {
		}

		mpd_getNextReturnElement(connection);
	}

	return entity;
}
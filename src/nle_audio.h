/* nle_audio.h
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef _NLE_AUDIO_H_
#define _NLE_AUDIO_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef void* nle_audio_handle;

nle_audio_handle prepare_audio();
void end_audio( nle_audio_handle h );
void play_audio( nle_audio_handle h );
void stop_audio( nle_audio_handle h );

int get_audio_poll_fd( nle_audio_handle h );
int get_available_frames( nle_audio_handle h );

void write_audio_frames( nle_audio_handle h, float* output, int frames );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NLE_AUDIO_H_ */

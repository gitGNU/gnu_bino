/*
 * This file is part of bino, a 3D video player.
 *
 * Copyright (C) 2010-2011
 * Martin Lambers <marlam@marlam.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MEDIA_INPUT_H
#define MEDIA_INPUT_H

#include <vector>

#include "media_data.h"
#include "media_object.h"


class media_input
{
private:
    std::string _id;                            // ID of this input: URL0[/URL1[/URL2[...]]]
    std::vector<media_object> _media_objects;   // The media objects that are combined into one input
    std::vector<std::string> _tag_names;        // Meta data: tag names
    std::vector<std::string> _tag_values;       // Meta data: tag values

    std::vector<std::string> _video_stream_names;       // Descriptions of available video streams
    std::vector<std::string> _audio_stream_names;       // Descriptions of available audio streams

    bool _supports_stereo_layout_separate;      // Does this input support the stereo layout 'separate_streams'?
    int _active_video_stream;                   // The video stream that is currently active.
    int _active_audio_stream;                   // The audio stream that is currently active.

    int64_t _initial_skip;                      // Initial portion of input to skip, in microseconds.
    int64_t _duration;                          // Total combined duration of input.

    video_frame _video_frame;                   // Video frame template for currently active video stream.
    audio_blob _audio_blob;                     // Audio blob template for currently active audio stream.

    // Find the media object and its stream index for a given video or audio stream number.
    void get_video_stream(int stream, int &media_object, int &media_object_video_stream) const;
    void get_audio_stream(int stream, int &media_object, int &media_object_audio_stream) const;

public:

    /* Constructor, Destructor */

    media_input();
    ~media_input();

    /* Open this input by combining the media objects at the given URLS. */

    void open(const std::vector<std::string> &urls);

    /* Get information */

    // Identifier.
    const std::string &id() const;

    // Metadata
    size_t tags() const;
    const std::string &tag_name(size_t i) const;
    const std::string &tag_value(size_t i) const;
    const std::string &tag_value(const std::string &tag_name) const;

    // Number of video streams in this input.
    int video_streams() const
    {
        return _video_stream_names.size();
    }

    // Number of audio streams in this input.
    int audio_streams() const
    {
        return _audio_stream_names.size();
    }

    // Name of the given video stream.
    const std::string &video_stream_name(int video_stream) const
    {
        return _video_stream_names[video_stream];
    }

    // Name of the given audio stream.
    const std::string &audio_stream_name(int audio_stream) const
    {
        return _audio_stream_names[audio_stream];
    }

    // Initial portion of the input to skip.
    int64_t initial_skip() const
    {
        return _initial_skip;
    }

    // Total combined duration of this input.
    int64_t duration() const
    {
        return _duration;
    }

    // Information about the active video stream, in the form of a video frame
    // that contains all properties but no actual data.
    const video_frame &video_frame_template() const;
    // Video rate information. This is only informal, as videos do not need to have
    // a constant frame rate. Usually, the presentation time of a frame should be used.
    int video_frame_rate_numerator() const;
    int video_frame_rate_denominator() const;
    int64_t video_frame_duration() const;       // derived from frame rate

    // Information about the active audio stream, in the form of an audio blob
    // that contains all properties but no actual data.
    const audio_blob &audio_blob_template() const;

    /*
     * Access video and audio data
     */

    /* Set the active video and audio streams. */
    void select_video_stream(int video_stream);
    void select_audio_stream(int audio_stream);

    /* Set the stereo layout. If this input is not compatible with the requested layout,
     * 'false' is returned and no change is made. */
    bool set_stereo_layout(video_frame::stereo_layout_t layout, bool swap);

    /* Start to read a video frame from the active stream asynchronously
     * (in a separate thread). */
    void start_video_frame_read();
    /* Wait for the video frame reading to finish, and return the frame.
     * An invalid frame means that EOF was reached. */
    video_frame finish_video_frame_read();

    /* Start to read the given amount of audio data from the active stream asynchronously
     * (in a separate thread). */
    void start_audio_blob_read(size_t size);
    /* Wait for the audio data reading to finish, and return the blob.
     * An invalid blob means that EOF was reached. */
    audio_blob finish_audio_blob_read();

    /* Seek to the given position in microseconds. This affects all streams.
     * Make sure that the position is not out of range!
     * The real position after seeking is only revealed after reading the next video frame
     * or audio blob. This position may differ from the requested position for various
     * reasons (seeking is only possible to keyframes, seeking is not supported by the
     * stream, ...) */
    void seek(int64_t pos);

    /*
     * Cleanup
     */

    /* When done, close the input and clean up. */
    void close();
};

#endif
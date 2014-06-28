#ifndef MPEGUTILS_H
#define MPEGUTILS_H


#include <stdint.h>

bool GetMPEGAudioFrameSize(
    uint32_t header, size_t *frame_size,
    int *out_sampling_rate = NULL, int *out_channels = NULL,
    int *out_bitrate = NULL, int *out_num_samples = NULL);

#endif // MPEGUTILS_H

#include "MpegUtils.h"

bool GetMPEGAudioFrameSize(uint32_t header, size_t *frame_size,int *out_sampling_rate, int *out_channels,int *out_bitrate, int *out_num_samples)
{
    if(frame_size)
    {
        *frame_size = 0;
    }

    if (out_sampling_rate)
    {
        *out_sampling_rate = 0;
    }

    if (out_channels)
    {
        *out_channels = 0;
    }

    if (out_bitrate)
    {
        *out_bitrate = 0;
    }

    if (out_num_samples)
    {
        *out_num_samples = 1152;
    }

    if ((header & 0xffe00000) != 0xffe00000)//֡ͬ����־
    {
        return false;
    }

    unsigned version = (header >> 19) & 3;//MPEG �汾

    if (version == 0x01)//δ����汾
    {
        return false;
    }

    unsigned layer = (header >> 17) & 3;//ѹ�����ڼ���

    if (layer == 0x00)//δ�����
    {
        return false;
    }

    //unsigned protection = (header >> 16) & 1;//CRCУ��

    unsigned bitrate_index = (header >> 12) & 0x0f;//λ��

    if (bitrate_index == 0 || bitrate_index == 0x0f)//��֧�ֵ�λ��
    {
        return false;
    }

    unsigned sampling_rate_index = (header >> 10) & 3;//������

    if (sampling_rate_index == 3)//δ����Ĳ�����
    {
        return false;
    }

    static const int kSamplingRateV1[] = { 44100, 48000, 32000 };
    int sampling_rate = kSamplingRateV1[sampling_rate_index];
    if (version == 2 /* V2 */)
    {
        sampling_rate /= 2;//MPEG 2.0 �Ĳ�����
    }
    else if (version == 0 /* V2.5 */)
    {
        sampling_rate /= 4;//MPEG 2.5 �Ĳ�����
    }

    unsigned padding = (header >> 9) & 1;//�Ƿ���Ҫ֡������

    if (layer == 3)// layer I
    {
        static const int kBitrateV1[] =//V1��һ��λ�ٱ�
        {
            32, 64, 96, 128, 160, 192, 224, 256,
            288, 320, 352, 384, 416, 448
        };

        static const int kBitrateV2[] =//V2��һ��λ�ٱ�
        {
            32, 48, 56, 64, 80, 96, 112, 128,
            144, 160, 176, 192, 224, 256
        };

        int bitrate =
            (version == 3 /* V1 */)
            ? kBitrateV1[bitrate_index - 1]
            : kBitrateV2[bitrate_index - 1];

        if (out_bitrate)
        {
            *out_bitrate = bitrate;
        }
        if(frame_size)
        {
            *frame_size = (12000 * bitrate / sampling_rate + padding) * 4;
        }
        if (out_num_samples)
        {
            *out_num_samples = 384;
        }
    }
    else
    {
        // layer II or III
        static const int kBitrateV1L2[] =//V1�ڶ���λ�ٱ�
        {
            32, 48, 56, 64, 80, 96, 112, 128,
            160, 192, 224, 256, 320, 384
        };

        static const int kBitrateV1L3[] =//V2������λ�ٱ�
        {
            32, 40, 48, 56, 64, 80, 96, 112,
            128, 160, 192, 224, 256, 320
        };

        static const int kBitrateV2[] =//V2/2.5�ڶ�����λ�ٱ�
        {
            8, 16, 24, 32, 40, 48, 56, 64,
            80, 96, 112, 128, 144, 160
        };

        int bitrate;
        if (version == 3 /* V1 */)
        {
            bitrate = (layer == 2 /* L2 */)
                      ? kBitrateV1L2[bitrate_index - 1]
                      : kBitrateV1L3[bitrate_index - 1];

            if (out_num_samples)
            {
                *out_num_samples = 1152;
            }
        }
        else
        {
            // V2 (or 2.5)
            bitrate = kBitrateV2[bitrate_index - 1];
            if (out_num_samples)
            {
                *out_num_samples = 576;
            }
        }

        if (out_bitrate)
        {
            *out_bitrate = bitrate;
        }

        if (version == 3 /* V1 */)
        {
            if(frame_size)
            {
                *frame_size = 144000 * bitrate / sampling_rate + padding;
            }
        }
        else
        {
            // V2 or V2.5
            if(frame_size)
            {
                *frame_size = 72000 * bitrate / sampling_rate + padding;
            }
        }
    }

    if (out_sampling_rate)
    {
        *out_sampling_rate = sampling_rate;
    }

    if (out_channels)
    {
        int channel_mode = (header >> 6) & 3;

        *out_channels = (channel_mode == 3) ? 1 : 2;
    }
    return true;
}

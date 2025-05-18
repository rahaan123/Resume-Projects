#include <iostream>
#include <alsa/asoundlib.h>

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    int pcm;
    
    // Open PCM device
    pcm = snd_pcm_open(&pcm_handle, "default", stream, 0);
    if (pcm < 0) {
        std::cerr << "Error opening PCM device." << std::endl;
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(pcm_handle, params, 44100, 0);
    snd_pcm_hw_params_set_channels(pcm_handle, params, 1);
    snd_pcm_hw_params(pcm_handle, params);

    // Set buffer size and prepare
    frames = 32;
    snd_pcm_prepare(pcm_handle);
    
    // Capture audio in a loop
    while (true) {
        char buffer[128];
        pcm = snd_pcm_readi(pcm_handle, buffer, frames);
        if (pcm == -EPIPE) {
            std::cerr << "Overrun occurred!" << std::endl;
            snd_pcm_prepare(pcm_handle);
        }
        if (pcm < 0) {
            std::cerr << "Error reading audio data." << std::endl;
            break;
        }
    }
    
    snd_pcm_close(pcm_handle);
    return 0;
}

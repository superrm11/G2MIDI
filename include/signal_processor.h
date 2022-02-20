#pragma once

#include <vector>
#include <cstdint>
#include <jack/jack.h>
#include <fftw3.h>
#include <cmath>
#include <algorithm>
#include <cstring>

namespace SignalProcessor
{

typedef struct
{
    jack_client_t* client;
    jack_port_t* port;
} jack_components_t;

extern int process_samples(jack_nframes_t nframes, void* arg);

extern std::vector<float> get_common_freqs(std::vector<std::pair<int, float>> fourier_out_mag, int nframes, int sample_rate, float cutoff);

extern std::vector<uint8_t> get_midi_keys(std::vector<float> frequencies);

extern void set_note_supplier(void (*fn_ptr)(std::vector<uint8_t>));

static void (*note_supplier)(std::vector<uint8_t>) = NULL;

static std::vector<uint8_t> midi_keys;

};
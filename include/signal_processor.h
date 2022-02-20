#pragma once

#include <vector>
#include <cstdint>
#include <jack/jack.h>

int process_samples(jack_nframes_t nframes, void* arg);

std::vector<uint8_t> get_midi_keys(std::vector<float> frequencies);


#include "signal_processor.h"

#define FOURIER_CUTOFF 25

using namespace SignalProcessor;

int SignalProcessor::process_samples(jack_nframes_t nframes, void* arg)
{
    jack_components_t *com = (jack_components_t*) arg;
    jack_client_t *client = com->client;
    jack_port_t *port = com->port;

    // Initialize fourier transform functionality with fftw3
    static float* in = (float*) malloc(sizeof(float) * nframes);
    static fftwf_complex *out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * nframes);
    static fftwf_plan plan = fftwf_plan_dft_r2c_1d(nframes, in, out, FFTW_ESTIMATE);

    jack_default_audio_sample_t *audio_in = (jack_default_audio_sample_t*) jack_port_get_buffer(port, nframes);
    std::memcpy(in, audio_in, nframes);

    fftwf_execute(plan);

    // Get the sample id (index of array) and magnitudes (val of array) of the fourier transform
    std::vector< std::pair<int, float> > fourier_out;
    for (int i = 0; i < nframes; i++)
    {
        float mag = sqrt( (out[i][0] * out[i][0]) + (out[i][1] * out[i][1]) );
        fourier_out.push_back(std::pair(i, mag));
    }
    // Get the most common frequencies over a certain magnitude cutoff
    std::vector<float> common_freqs = get_common_freqs(fourier_out, nframes, jack_get_sample_rate(client), FOURIER_CUTOFF);

    // printf("Top Freqs: ");
    // for(int i = 0; i < common_freqs.size(); i++)
    //     printf("%f, ", common_freqs[i]);
    // printf("\n");

    static std::vector<uint8_t> midi_keys;
    midi_keys = get_midi_keys(common_freqs);

    if(note_supplier != NULL)
        note_supplier(midi_keys);

    return 0;
}

std::vector<float> SignalProcessor::get_common_freqs(std::vector<std::pair<int, float>> fourier_out_mag, int nframes, int sample_rate, float cutoff)
{
    // Sort the fourier output to get the frequencies with the highest amplitudes
    std::sort(fourier_out_mag.begin(), fourier_out_mag.end(), [](std::pair<int, float> item1, std::pair<int, float> item2){
        return item1.second > item2.second;
    });

    // Get the frequencies above the cutoff and put them in a vector
    // Nasty hack that limits the output to a max of 1; lots of noise on single notes
    std::vector<float> output;
    if(fourier_out_mag[0].second >= cutoff)
        output.push_back(fourier_out_mag[0].first * ((float)sample_rate / (float)nframes));
    // for(int i = 0; i < fourier_out_mag.size(); i++)
    // {
    //     if(fourier_out_mag[i].second < cutoff)
    //         break;
    //     // Translate the "sample index" into a frequency
    //     output.push_back(fourier_out_mag[i].first * ((float)sample_rate / (float)nframes));
    //     printf("%f, ", fourier_out_mag[i].second);
    // }

    // printf("\n");

    return output;    
}

std::vector<uint8_t> SignalProcessor::get_midi_keys(std::vector<float> frequencies)
{
    std::vector<uint8_t> output;

    // Algorithm (as provided by wikipedia) for frequency to MIDI note is:
    // 12 * log2( freq / 440 ) + 69
    // the + 49 gives the piano number, MIDI is offset by 20
    for(int i = 0; i < frequencies.size(); i++)
        output.push_back(floor(12 * log2(frequencies[i]/440.0) + 49 + 20));

    return output;
}

void SignalProcessor::set_note_supplier(void (*fn_ptr)(std::vector<uint8_t>))
{
    note_supplier = fn_ptr;
}
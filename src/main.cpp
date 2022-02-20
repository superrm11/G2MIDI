#include <cstdio>
#include <rtmidi/RtMidi.h>
#include <unistd.h>
#include <jack/jack.h>
#include <fftw3.h>
#include "midi_message.h"

RtMidiOut *midi_out;

int main()
{ 
    jack_client_t *jack_c = jack_client_open("G2MIDI In", JackNullOption, NULL);

    static jack_port_t *in_port = jack_port_register(jack_c, "G2MIDI In", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    
    static fftwf_plan plan;
    static float *in;
    static fftwf_complex *out;

    in = (float*) malloc(sizeof(float) * 1024);
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftw_complex) * 1024);  
    plan = fftwf_plan_dft_r2c_1d(1024, in, out, FFTW_ESTIMATE);

    jack_set_process_callback(jack_c, [](jack_nframes_t nframes, void *arg){
        
        
        jack_default_audio_sample_t *audio_in;

        audio_in = (jack_default_audio_sample_t*)jack_port_get_buffer(in_port, nframes);

        for(int i = 0; i < nframes; i++)
        {
            in[i]= audio_in[i];        
        }

        fftwf_execute(plan);

        for(int i = 0; i < nframes; i++)
        {
            printf("%d, %f, %f\n",i, out[i][0], out[i][1]);
        }
        // exit(0);

        return 0;
    }, 0);

    printf("Sample rate: %d; Buffer size: %d\n", jack_get_sample_rate(jack_c), jack_get_buffer_size(jack_c));

    jack_activate(jack_c);

    while(true)
    {
        sleep(1);
    }


    return 0;
    try{
    midi_out = new RtMidiOut();
    } catch (RtMidiError e)
    {
        e.printMessage();
    }

    std::cout << midi_out->getPortName() << std::endl;
    
    midi_out->openVirtualPort();

    std::cout << midi_out->getPortName() << std::endl;

    MidiMessage midi_message(*midi_out);

    midi_message.start_note(60, 0, 127);
    usleep(1000 * 1000);
    midi_message.end_note(60, 0);
    midi_message.start_note(68, 0, 127);
    usleep(1000 * 1000);
    midi_message.end_note(68, 0);

    // cleanup
    midi_out->closePort();
    delete midi_out;

    return 0;
}
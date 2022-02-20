#include <cstdio>
#include <rtmidi/RtMidi.h>
#include <unistd.h>
#include <jack/jack.h>
#include <fftw3.h>
#include "midi_message.h"
#include "signal_processor.h"



int main()
{ 
    // Init Jack
    jack_client_t *jack_c = jack_client_open("G2MIDI In", JackNullOption, NULL);
    static jack_port_t *in_port = jack_port_register(jack_c, "G2MIDI In", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    SignalProcessor::jack_components_t com = {
        .client = jack_c,
        .port = in_port
    };

    jack_set_process_callback(jack_c, SignalProcessor::process_samples, &com);

    // Init RtMidi
    static RtMidiOut *midi_out;

    try
    {
        midi_out = new RtMidiOut();
    } catch (RtMidiError e)
    {
        e.printMessage();
    }

    midi_out->openVirtualPort();
    static MidiMessage midi_message(*midi_out);

    SignalProcessor::set_note_supplier([](std::vector<uint8_t> notes){
        midi_message.update_notes(notes);
    });

    jack_activate(jack_c);

    while(true)
    {
        sleep(1);
    }

    // cleanup
    midi_out->closePort();
    // delete midi_out;

    return 0;
    
}
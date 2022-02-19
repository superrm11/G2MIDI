#include <cstdio>
#include <rtmidi/RtMidi.h>
#include <unistd.h>
#include "midi_message.h"

RtMidiOut *midi_out;

int main()
{
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
#pragma once
#include "rtmidi/RtMidi.h"
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unistd.h>

#define PROG_CHG 0b11000000
#define CTRL_CHG 0b10110000
#define NOTE_ON  0b10010000
#define NOTE_OFF 0b10000000

#define VOL_ID  7

class MidiMessage
{
    public:

    MidiMessage(RtMidiOut &midi_dev);

    // C4: 60; each step is 1
    void start_note(uint8_t note, uint8_t channel, uint8_t velocity);

    void end_note(uint8_t note, uint8_t channel);

    void set_volume(uint8_t vol, uint8_t channel);

    void update_notes(std::vector<uint8_t> note_input);

    private:
    
    RtMidiOut &midi_dev;
    std::vector<uint8_t> message;    
    std::thread *midi_bg;

    protected:
    std::mutex midi_mut;
    std::vector<uint8_t> note_input;

};
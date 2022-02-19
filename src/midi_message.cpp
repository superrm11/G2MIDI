#include "midi_message.h"

MidiMessage::MidiMessage(RtMidiOut &midi_dev)
: midi_dev(midi_dev)
{

}

void MidiMessage::start_note(uint8_t note, uint8_t channel, uint8_t velocity)
{
    uint8_t message[] = { (uint8_t) (NOTE_ON | channel), note, velocity};
    midi_dev.sendMessage(message, 3);
}

void MidiMessage::end_note(uint8_t note, uint8_t channel)
{
    uint8_t message[] = { (uint8_t) (NOTE_OFF | channel), note, 0x40};
    midi_dev.sendMessage(message, 3);
}

// min = 0, max = 127
void MidiMessage::set_volume(uint8_t vol, uint8_t channel)
{
    uint8_t message[] = { (uint8_t)(CTRL_CHG | channel), VOL_ID, vol};
    midi_dev.sendMessage(message, 3);
}
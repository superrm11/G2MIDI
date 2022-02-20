#include "midi_message.h"

MidiMessage::MidiMessage(RtMidiOut &midi_dev)
: midi_dev(midi_dev)
{
    // Create a background thread to constantaly monitor the sustained notes, create new notes and remove old notes.
    midi_bg = new std::thread([](void* arg){

        static std::vector<uint8_t> sustaining_notes;

        MidiMessage *msg = (MidiMessage*)arg;
        while(true)
        {            
            msg->midi_mut.lock();
            std::vector<uint8_t> input_instance;
            for(uint8_t b : msg->note_input)
                input_instance.push_back(b);
            msg->midi_mut.unlock();
            std::vector<uint8_t> start_notes, stop_notes;

            // Search the newly input notes for the previously sustaining notes.
            // If it isn't there, the note has stopped, and midi must send a stop code
            for(auto sus_itr = sustaining_notes.begin(); sus_itr != sustaining_notes.end(); sus_itr++)
            {
                if(std::find(input_instance.begin(), input_instance.end(), *sus_itr) == input_instance.end())
                {
                    sustaining_notes.erase(sus_itr);
                    stop_notes.push_back(*sus_itr);
                    sus_itr--;
                }

            }
            
            // Search the sustaining notes for each new input note. If nothing is found, then it is a new note
            // and must be added to the sustaining notes, and send a start code
            for(auto in_itr = input_instance.begin(); in_itr != input_instance.end(); in_itr++)
            {
                if(std::find(sustaining_notes.begin(), sustaining_notes.end(), *in_itr) == sustaining_notes.end())
                {
                    sustaining_notes.push_back(*in_itr);
                    start_notes.push_back(*in_itr);
                }
            }

            for(int i = 0; i < start_notes.size(); i++)
                msg->start_note(start_notes[i], 0, 127);
            
            for(int i = 0; i < stop_notes.size(); i++)
                msg->end_note(stop_notes[i], 0);
            

            usleep(10 * 1000);

        }
    }, this);
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

void MidiMessage::update_notes(std::vector<uint8_t> note_input)
{
    midi_mut.lock();
    this->note_input.clear();
    for(uint8_t b : note_input)
        this->note_input.push_back(b);
    midi_mut.unlock();
}
#ifndef SPEAKER_CONTROLLER_H
#define SPEAKER_CONTROLLER_H

class SpeakerController 
{
public:
    SpeakerController(int pin);
    ~SpeakerController();

    void playVictoryTune();
    void playAskTune();
    void playErrorTune();

private:
    int _pin;

    void playTone(int frequency, int duration);
};

#endif // SPEAKER_CONTROLLER_H
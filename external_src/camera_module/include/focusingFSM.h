#ifndef FOCUSINGMACHINE_H
#define FOCUSINGMACHINE_H
#include "objectivecontroller.h"

enum FocusingState {
    notFocused = 0,
    coarseFocusing,
    fineFocusing,
    holdingFocus,
    refocusing,
};

class FocusingFSM
{
public:
    FocusingFSM(ObjectiveController* objective);
    ~FocusingFSM();

    void focus(double sharpness);
    void stopFocus();

    double getCurrentPosition() const;

private:
    void notFocusingState();
    void coarseFocusingState(double sharpness);
    void fineFocusingState(double sharpness);
    void holdingFocusState(double sharpness);
    void refocusingState(double sharpness);

private:
    ObjectiveController* pObjective;
    FocusingState mState;

    // в процентах
    const double delta = 0.15;

    const int stepCoarse = 500;
    const int stepFine = 100;

    double firstLocalMax = 0;
    double firstSidePosition = 0;

    double secondLocalMax = -1;
    double secondSidePosition = 0;

    double absoluteMax;
    double absoluteMaxPosition;

    double currentPosition;
};

#endif // FOCUSINGMACHINE_H

#include "focusingFSM.h"

FocusingFSM::FocusingFSM(ObjectiveController* objective) : pObjective(objective){
    mState = notFocused;
}

FocusingFSM::~FocusingFSM() {
    pObjective = nullptr;
}

void FocusingFSM::focus(double sharpness) {
    switch (mState) {
    case notFocused:
        notFocusingState();
        break;
    case coarseFocusing:
        coarseFocusingState(sharpness);
        break;
    case fineFocusing:
        fineFocusingState(sharpness);
        break;
    case holdingFocus:
        holdingFocusState(sharpness);
        break;
    case refocusing:
        refocusingState(sharpness);
        break;
    }
}

void FocusingFSM::stopFocus() {
    mState = notFocused;
    firstLocalMax = 0;
    firstSidePosition = 0;

    secondLocalMax = -1;
    secondSidePosition = 0;

    absoluteMax = 0;
    absoluteMaxPosition = 0;
}

void FocusingFSM::notFocusingState() {
    currentPosition = pObjective->getCurrentFocusing();
    if(currentPosition == 0.0)
        mState = coarseFocusing;
    else
        pObjective->setFocusing(0);
}

void FocusingFSM::coarseFocusingState(double sharpness) {
    currentPosition = pObjective->getCurrentFocusing();

    if(sharpness > firstLocalMax) {
        secondLocalMax = firstLocalMax;
        secondSidePosition = firstSidePosition;

        firstLocalMax = sharpness;
        firstSidePosition = currentPosition;
    }
    else if(sharpness < firstLocalMax) {
        if(sharpness > secondLocalMax) {
            secondLocalMax = sharpness;
            secondSidePosition = currentPosition;
        }
    }

    if(currentPosition == 10000.0) {
        mState = fineFocusing;
        if(secondSidePosition < firstSidePosition){
            double tmp = secondSidePosition;
            secondSidePosition = firstSidePosition;
            firstSidePosition = tmp;
        }
        pObjective->setFocusing(firstSidePosition);
    }
    else
        pObjective->setFocusing(currentPosition + stepCoarse);
}

void FocusingFSM::fineFocusingState(double sharpness) {
    currentPosition = pObjective->getCurrentFocusing();

    if(sharpness > absoluteMax) {
        absoluteMax = sharpness;
        absoluteMaxPosition = currentPosition;
    }

    if(currentPosition == secondSidePosition) {
        mState = holdingFocus;
        pObjective->setFocusing(absoluteMaxPosition);
    }
    else
        pObjective->setFocusing(currentPosition + stepFine);
}

void FocusingFSM::holdingFocusState(double sharpness) {
    double left = absoluteMax * (1 - delta);
    double right = absoluteMax * (1 + delta);
    if((left <= sharpness) && (sharpness <= right))
        return;
    else {
        mState = refocusing;
        absoluteMax = 0;
        absoluteMaxPosition = 0;
        pObjective->setFocusing(firstSidePosition);
    }
}

void FocusingFSM::refocusingState(double sharpness) {
    currentPosition = pObjective->getCurrentFocusing();

    if(sharpness > absoluteMax) {
        absoluteMax = sharpness;
        absoluteMaxPosition = currentPosition;
    }

    if(currentPosition == secondSidePosition) {
        mState = holdingFocus;
        pObjective->setFocusing(absoluteMaxPosition);
    }
    else
        pObjective->setFocusing(currentPosition + stepFine);
}

double FocusingFSM::getCurrentPosition() const {
    return currentPosition;
}

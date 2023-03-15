#ifndef OBJECTIVECONTROLLER_H
#define OBJECTIVECONTROLLER_H
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

#include "serialib.h"

using namespace std;

class ObjectiveController {
public:
    explicit ObjectiveController(const char* serialPort);

    ~ObjectiveController();

    bool connectToController(const char* serialPort);

    bool disconnectController();

    void setDiaphragmLevel(const double value);

    void setFocusing(const int value);

    double getCurrentFocusing();

    string currentError() const;

    std::vector<double> getAppertures();

    void setAppertures(std::vector<double> appertures);

    bool isContollerActive();

private:

    bool testControllerActive();
    string getFocusingCmd(const int value);
    string getAppertureCmd(const int value);

    vector <double> mAppertures;
    serialib mObjective;
    string mSerialPort;
    string mError;
};

#endif // OBJECTIVECONTROLLER_H

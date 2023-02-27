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

    void setDiaphragmLevel(const double value);

    void setFocusing(const double value);

    double getCurrentFocusing();

    string currentError() const;

    void setAppertureVal();

    std::vector<double> getAppertures();

    void setAppertures(std::vector<double> appertures);

private:

    bool testControllerActive();
    string getFocusingCmd(const double value);
    string getAppertureCmd(const double value);

    vector <double> mAppertures;
    serialib mObjective;
    string mSerialPort;
    string mError;
};

#endif // OBJECTIVECONTROLLER_H

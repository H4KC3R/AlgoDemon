#include "objectivecontroller.h"

ObjectiveController::ObjectiveController(const char* serialPort) : mSerialPort(serialPort) {
    if(!connectToController(serialPort))
        throw std::logic_error("Could not connect to objective");
    // Устанавливаем значения по умолчанию(Canon EF 70-200mm f/4L USM)
    mAppertures = {4.0, 4.5, 5.0, 5.6, 6.3,
                   7.1, 8, 9, 10, 11, 13, 14, 16,
                   18, 20, 22, 25, 29, 32};

}

ObjectiveController::~ObjectiveController(){
    if(testControllerActive())
        mObjective.closeDevice();
}

bool ObjectiveController::connectToController(const char* serialPort) {
    mError.clear();

    if(mObjective.isDeviceOpen())
        mObjective.closeDevice();

    if( mObjective.openDevice(serialPort, 38400) != 1 ) {
        mError = "Can't open connection on current port";
        return false;
    }
    return true;
}

void ObjectiveController::setDiaphragmLevel(const double value) {
    mError.clear();
    int index;
    auto itr = std::find(mAppertures.begin(), mAppertures.end(), value);
    if (itr != mAppertures.cend())
        index = std::distance(mAppertures.begin(), itr);
    else{
        mError = "Wrong apperture value";
        return;
    }
    if (testControllerActive()) {
        if( mObjective.writeString(getAppertureCmd(index).c_str()) != 1 )
            mError = "Writting error";

    }
}

void ObjectiveController::setFocusing(const double value) {
    mError.clear();
    if (testControllerActive()) {
        if( mObjective.writeString(getFocusingCmd(value).c_str()) != 1 ) {
            mError = "Writting error";
        }
    }
}

double ObjectiveController::getCurrentFocusing() {
    mError.clear();
    char* rxBuff;
    std::string resultStr = "0";
    std::string::size_type sz;

    if (testControllerActive()) {
        mObjective.flushReceiver();
        if(mObjective.writeString("P#") == 1) {
            std::this_thread::sleep_for(1000ms);
            if( mObjective.readString(rxBuff, '#', 10, 1000) != 1 ) {
                std::string s(rxBuff);
                resultStr = s.substr(0, s.length() - 1);
            }
            else
                mError = "Can't get answer";
        }
    }
    double result = 0;
    try{
        result = std::stod(resultStr, &sz);
    }
    catch(...){
        mError = "Can't get answer";
    }
    return result;
}

string ObjectiveController::currentError() const{
    return mError;
}

std::vector<double> ObjectiveController::getAppertures() {
    return mAppertures;
}

void ObjectiveController::setAppertures(std::vector<double> appertures) {
    if(!appertures.empty())
        mAppertures = appertures;
}

bool ObjectiveController::testControllerActive() {
    if(mObjective.isDeviceOpen())
        return true;
    return false;
}

string ObjectiveController::getFocusingCmd(const double value) {
    string val = to_string(value);
    string cmd = "M" + val + "#";
    return cmd;
}

string ObjectiveController::getAppertureCmd(const double value) {
    string str = to_string(value);
    size_t n = 2;

    int precision = n - std::min(n, str.size());
    str.insert(0, precision, '0');

    string cmd = "A" + str + "#";
    return cmd;
}

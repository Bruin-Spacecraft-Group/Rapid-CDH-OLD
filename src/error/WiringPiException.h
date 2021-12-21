#ifndef RAPIDCDH_WIRINGPIEXCEPTION_H
#define RAPIDCDH_WIRINGPIEXCEPTION_H

#include <string>
#include <stdexcept>

using std::string;

class WiringPiException : public std::runtime_error {
public:
    explicit WiringPiException(const string &msg)
        : std::runtime_error(msg) {}
};


#endif //RAPIDCDH_WIRINGPIEXCEPTION_H
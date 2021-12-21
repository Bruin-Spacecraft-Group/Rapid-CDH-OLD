#ifndef RAPIDCDH_UCAMIIIEXCEPTION_H
#define RAPIDCDH_UCAMIIIEXCEPTION_H

#include <string>
#include <stdexcept>

using std::string;

class UCamIIIException : public std::runtime_error {
public:
    explicit UCamIIIException(const string &msg);
    explicit UCamIIIException(int nak_err);

private:
    [[nodiscard]] static string parse_nak_err(int nak_err) ;
};

#endif //RAPIDCDH_UCAMIIIEXCEPTION_H

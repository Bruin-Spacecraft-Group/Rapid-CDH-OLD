#ifndef TMP36
#define TMP36

class ina260;

class TMP36 {
public:
    TMP36(ina260* sensor);
    double getTemperature();
private:
    double voltageAtZero;
    double voltagePerDegree;
    ina260* sensor;
};

#endif
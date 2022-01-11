#ifndef TMP36
#define TMP36

class ADS7828;

class TMP36 {
public:
    TMP36(ADS7828* sensor, int channel);
    double getTemperature();
private:
    double voltageAtZero;
    double voltagePerDegree;
    ADS7828* sensor;
    int channel;
};

#endif
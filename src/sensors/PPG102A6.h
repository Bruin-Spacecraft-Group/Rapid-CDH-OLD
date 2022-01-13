#ifndef PPG102A6
#define PPG102A6

class ina260;

class PPG102A6 {
    public:
        PPG102A6(ADS7828* sensor, int channel, int gpioPin);
        double getTemperature();
    private:
        double resistanceAtZero;
        double ppmPerDegree;
        ADS7828* sensor;
        double dividerResistance;
        int channel;
        int gpioPin;
};

#endif
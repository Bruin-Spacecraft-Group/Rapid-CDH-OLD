#ifndef PPG102A6
#define PPG102A6

class ina260;

class PPG102A6 {
    public:
        PPG102A6(ADS7828* sensor, int channel, int gpioPin);
        // note that there are no error codes related to the gpio pin because
        // gpio failures are silent in the wiringpi library
        [[nodiscard]] Status getTemperature(double& value);
    private:
        double resistanceAtZero;
        double ppmPerDegree;
        ADS7828* sensor;
        double dividerResistance;
        double topVoltage;
        int channel;
        int gpioPin;
};

#endif
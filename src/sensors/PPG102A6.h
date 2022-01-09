#ifndef PPG102A6
#define PPG102A6

class ina260;

class PPG102A6 {
    public:
        PPG102A6(ina260* sensor);
        double getTemperature();
    private:
        double resistanceAtZero;
        double ppmPerDegree;
        ina260* sensor;
};

#endif
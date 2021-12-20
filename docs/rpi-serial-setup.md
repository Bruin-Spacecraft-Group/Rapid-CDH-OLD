# Using the serial port on the Raspberry Pi
(See [this guide](https://di-marco.net/blog/it/2020-06-06-raspberry_pi_3_4_and_0_w_serial_port_usage/))

## Disable Bluetooth
At the end of `/boot/config.txt`, add:
```
dtoverlay=disable-bt
```

## Disable serial console
Remove the following text from `/boot/cmdline.txt`:
```
console=serial0,115200
```

## Reboot and verify
Reboot:
```
sudo reboot
```

To verify that everything worked, run:
```
ls -al /dev | grep serial
```

You should now see another serial port:
```
lrwxrwxrwx  1 root root           7 Dec 20 20:22 serial0 -> ttyAMA0
lrwxrwxrwx  1 root root           5 Dec 20 20:22 serial1 -> ttyS0
```

## Test program
```cpp
#include <wiringPi.h>
#include <wiringSerial.h>

using std::cout;
using std::endl;

int main() {
    if (wiringPiSetup() == -1) {
        cout << "Unable to start WiringPi" << endl;
        return 1;
    }

    int fd; // file descriptor
    unsigned int next_time;

    if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0) {
        cout << "Unable to open serial device" << endl;
        return 1;
    }

    next_time = millis() + 300;

    int i = 0;
    while (i < 256) {
        if (millis() > next_time) {
            cout << "Out: " << i << endl;
            serialPutchar(fd, i);
            next_time += 300;
            i++;
        }

        delay(3);

        while (serialDataAvail(fd) > 0) {
            cout << "In: " << serialGetchar(fd) << endl;
        }
    }

    serialClose(fd);

    return 0;
}
```

Connect GPIO pins 14 (TXD) and 15 (RXD) together, and run the test program. You should see something like:
```
Out: 0
In: 0
Out: 1
In: 1
Out: 2
In: 2
Out: 3
In: 3
Out: 4
In: 4
Out: 5
In: 5
...
```
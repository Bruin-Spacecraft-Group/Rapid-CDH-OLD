# RapidCDH
Rapid C&DH repository

## Getting started

### Install WiringPi
Clone the [WiringPi Mirror](https://github.com/WiringPi/WiringPi) on the Pi (you may need to install `git`):

```
pi@raspberrypi:~ $ git clone https://github.com/WiringPi/WiringPi.git
```

Then install it with their build script:

```
pi@raspberrypi:~/WiringPi $ ./build
```

### Remote development
The easiest way to develop for the Raspberry Pi is remotely via SSH. To set up your IDE for remote development, take
a look at the following:

- [Visual Studio Code](https://code.visualstudio.com/docs/remote/ssh)
- [CLion](https://www.jetbrains.com/help/clion/remote-projects-support.html)

To set up the Pi, install CMake:
```
pi@raspberrypi:~ $ sudo apt install cmake
```
  
Then clone the repository:
```
pi@raspberrypi:~ $ git clone https://github.com/Bruin-Spacecraft-Group/RapidCDH.git
```

Finally, run the setup script (you only need to do this once):
```
pi@raspberrypi:~/RapidCDH $ ./setup.sh
```

Now you can build the project:
```
pi@raspberrypi:~/RapidCDH $ ./build.sh
```

And run the executable:
```
pi@raspberrypi:~/RapidCDH $ ./build/RapidCDH
```

### Cross-compiling
Alternatively, you can set up cross-compiling on your computer. This will build the project on your computer, 
but create an executable for the Raspberry Pi.

Clone the repository on your computer:

```
git clone https://github.com/Bruin-Spacecraft-Group/RapidCDH.git
```

Then run the setup script (you only need to do this once):

```
sudo ./setup-rpi.sh
```

This will install the necessary Raspberry Pi toolchain and library files on your computer, as well as create a
FindWiringPi.cmake file for finding and linking the WiringPi library.

Build the project:

```
./build-rpi.sh
```

Move the executable to the Pi:

```
scp build/RapidCDH pi@<pi-IP>:/home/pi
```

Run the executable on the Pi:

```
pi@raspberrypi ~ $ ./RapidCDH
```

#### Warning!
The generated executable targets ARM, and will not run on your computer! If you try it, you'll
probably get an error like:

```
The file './build/RapidCDH' is marked as an executable but could not be run by the operating system.
```

## Making a new branch
To make a new branch, run:

```
git checkout -b <branch-name>
```

After making your changes, push your new branch to the remote repository:

```
git push origin <branch-name>
```

## WiringPi
- [Repository](https://github.com/WiringPi/WiringPi)
- [Gordon's Website](http://wiringpi.com/)

## More guides

- [Headless setup guide](docs/rpi-headless-setup.md)
- [Cross-compiling guide](docs/rpi-cross-compile.md)

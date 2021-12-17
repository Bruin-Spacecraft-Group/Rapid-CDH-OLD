# RapidCDH
Rapid C&DH repository

## Getting started

Clone the repository:

```
git clone https://github.com/Bruin-Spacecraft-Group/RapidCDH.git
```

Run the setup script (you only need to do this once):

```
sudo ./setup.sh
```

Build the project:

```
./build.sh
```

Move the executable to the Pi:

```
scp build/RapidCDH pi@<pi-IP>:/home/pi
```

Run the program on the Pi:

```
pi@raspberrypi ~ $ ./RapidCDH
```

### Warning!
The generated executable targets ARM, and will not run on your computer! If you try it, you'll
probably get an error like:

```
The file './build/RapidCDH' is marked as an executable but could not be run by the operating system.
```

## More guides

- [Headless setup guide](docs/rpi-headless-setup.md)
- [Cross-compiling guide](docs/rpi-cross-compile.md)

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
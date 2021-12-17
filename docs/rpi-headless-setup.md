# Headless Raspberry Pi Setup

## Make a startup disk

Download a [Raspberry Pi OS](https://www.raspberrypi.com/software/operating-systems/) or another OS of your choice.
To create the startup disk, [use dd](https://linuxiac.com/how-to-create-bootable-usb-drive-using-dd-command/) or a
startup disk creator, like Ubuntu's conveniently named Startup Disk Creator or 
[balenaEtcher](https://www.balena.io/etcher/).

## Configure Networking

Following [this guide](https://www.raspberrypi.com/documentation/computers/configuration.html#setting-up-a-headless-raspberry-pi),
create a file called `wpa_supplicant.conf` in the boot folder of your microSD card with this:

```

ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
country=US
update_config=1

network={
 ssid="<Name of your wireless LAN>"
 psk="<Password for your wireless LAN>"
}
```

Note the newline at the top (not sure if this is necessary, but the Raspberry Pi guide says it might be).

## Enable SSH

Following [this guide](https://linuxize.com/post/how-to-enable-ssh-on-raspberry-pi/), simply create an empty file
named `ssh` in the boot folder of your microSD. That's it!

## Find your PI's IP address
See [this guide](https://www.raspberrypi.com/documentation/computers/remote-access.html).

## SSH!
Now you can simply access your Pi from the convenience of your own computer:

```
ssh pi@<pi-IP>
```

The default Raspberry Pi password is `raspberry`.

## Moving files between the Pi and your computer
This can be done with `scp` or `rsync`.

For example,
```
scp <source>:/path/to/file <dest>:/destination/path
```
or
```
scp -r <source>:/path/to/directory <dest>:/destination/path
```

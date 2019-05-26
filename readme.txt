insmod rickroll.ko
device is /dev/ricky

to observe kernel messages: journalctl -f _TRANSPORT=kernel 
to get rick rolled: cat /dev/ricky

to unload module: rmmod rickroll.ko

Written by Tim Nibert, tested on Arch Linux

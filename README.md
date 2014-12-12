VOLIMITER

This is a Carbon daemon which limits the system's sound volume. It uses CoreAudio API to get notified about output volume changes and, whenever it exceeds a limit, restores it to the said limit.

Requirements:

Mac OS 10.4, PowerPC or Intel.

Installation:

Though you can run volimiter from any directory, I recommend copying the executable to /usr/bin for easier access.

Usage:

You supply the only argument to volimiter, a desired fraction of maximum output volume, for example:

volimiter 0.25

This will limit the default output device's volume to 0.25 of the maximum value. If you run volimiter without an argument, it will use current output volume as the limit. To stop volume limiting simply kill volimiter:

killall volimiter

Copying:

This product is distributed as arsware under the terms of GNU General Public License. If you find it useful, say thank-you to the members of ArsTechnica forums (http://episteme.arstechnica.com/6/ubb.x).
# Push-to-Talk for PulseAudio

This program implements "push-to-talk" functionality for programs that access your microphone using PulseAudio. It was created because [Zoom's](https://zoom.us/) Linux client does not support push-to-talk. (The Windows and macOS clients *do* support push-to-talk.)

## How it works
Once your program of interest has started getting audio from your microphone, launch ptt\_pa. A black window will appear, and the program will stop receiving microphone input. When the space key is held, the window will turn white and the program will receive microphone input again.

## Requirements
 * Xlib, and development headers if building from source. On Ubuntu the library is provided by the package [libx11-6](https://packages.ubuntu.com/search?keywords=libx11-6) and the headers are provided by the package [libx11-dev](https://packages.ubuntu.com/search?keywords=libx11-dev).
 * pacmd. On Ubuntu this program is provided by the package [pulseaudio-utils](https://packages.ubuntu.com/search?keywords=pulseaudio-utils).

## Compiling
Run Make in the root directory of this repository. This will produce an executable ptt\_pa in that same directory.

## Running
 1. Start your program of interest, and start recording audio. (It's not enough to just start the program. For Zoom, this means that you should join a meeting before proceeding.)
 2. Run `pacmd list-source-outputs`. Make note of the index of the output associated with the program you're interested in.
 3. Run `./ptt_pa [THAT INDEX]`.
 4. Press and hold the space key whenever you wish to unmute yourself.

## Known problems and to-dos
 * [ ] Closing the program while muted will leave the output muted.
   * Workaround: hold space to stay unmuted while closing the program.
 * [ ] User is unmuted when program is first started even though they should be muted.
   * Workaround: mute and unmute every time you start the program to ensure that you're in the correct state.
 * Using system commands is pretty gross. I'd like to switch to PulseAudio's C library at some point.


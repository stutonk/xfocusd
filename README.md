# xfocusd

xfocusd is a simple and orthogonal X focus manager designed to be used in 
conjunction with [wmutils](https://github.com/wmutils). It institutes the
policy of 'focus follows mouse; focus new windows; focus top window upon
focused window close' and places new windows centered under the mouse pointer.
Whenever a new window receives focus, its window id is printed to STDOUT in
order to facilitate interoperation with other scripts.

## Usage
xfocusd has no command line options

## Building
#### Dependencies
[libxcb](https://xcb.freedesktop.org/), [libwm](https://github.com/wmutils/libwm)
#### Installation
`make && make install`
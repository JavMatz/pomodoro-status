# pomodoro-status
A simple pomodoro timer for your status bar or panel

# How to 
Run `simple-pomodoro -h` to see the available flags 😜

Run `kill -SIGUSR1 <simple-pomodor pid>` to skip the currently running timer.

Run `simple-pomodoro | cut -f1` to get the name of the currently running timer.

Run `simple-pomodoro | cut -f2` to get the timer itself.

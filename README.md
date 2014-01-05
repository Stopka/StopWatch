TimeTool
========

Stopwatch for Pebble SDK 2 featuring centisecond precision, up to 5 laps always visible, and a 30 lap history accessible by pressing down button. Max time is 99 days 23 hours and 59 minutes, then days overflow to 0 and continues counting. The current display format is shown in labels. Display time is refreshing only once per second due to battery savings.

Stopwatch should be also persistent: the time should continue counting even on closed app. But there is a bug in persist functions of PebbleOS and this funkcionality sometimes doesn't work. ;-)

Press select button: toggles start stop;
Press up button: starts new lap (on running state)/resets (if stopped);
Long press up button: resets;
Press down button: select older lap in list
Long press down button: select newer lap in list

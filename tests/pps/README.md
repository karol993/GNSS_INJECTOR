# PPS regression test

Run from the project root (create `Debug/host-tests` first):

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Itests/pps -ICore/Inc tests/pps/test_pps.c Core/Src/pps_gen.c -o Debug/host-tests/test_pps.exe
./Debug/host-tests/test_pps.exe
```

The test compiles the production PPS driver with a small HAL/PWM model.
It checks 100 ms pulse width, calls immediately before and after automatic
timer rollover, 180 further seconds, STOP/restart, and retry after a failed
HAL start. The original driver fails the before-rollover assertion because
it resets the counter and raises PPS prematurely. The fixed driver passes.

The model assumes a disabled PWM channel is low; it does not prove the
electrical behavior of a disconnected alternate-function output. Confirm
the actual PB4 waveform and downstream PPS acceptance on hardware.

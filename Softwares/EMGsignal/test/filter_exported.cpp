// Runs the EMGsignal.ino processing chain on samples fed over stdin.
//
// Protocol (one sample per line, both directions):
//   in : "<raw> <taskID>\n"
//   out: "<blurred> <taskID>\n"
// The program exits when stdin is closed.
#include <cmath>
#include <cstdio>
#include "../src/emg_filters.h"

EMGFilters<f32> filter(
    false,  //by pass all
    true,   //notch
    true,   //lpf
    true,   //hpf
    false   //blur
);

int main() {
    char line[256];
    while (std::fgets(line, sizeof line, stdin)) {
        f32 raw;
        i32 taskID;
        if (std::sscanf(line, "%f %d", &raw, &taskID) != 2) continue;

        f32 filtered = filter.process(raw);

        f32 envelope = std::fabs(filtered) * amplification_factor;
        if (envelope < 0.0f) envelope = 0.0f;

        i32 blurred = (i32)(filter.gblur().blur(envelope));

        std::printf("%d %d\n", blurred, taskID);
        std::fflush(stdout);    // essential, or Python blocks waiting for the reply
    }
    return 0;
}

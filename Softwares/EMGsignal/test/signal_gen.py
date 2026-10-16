"""Generate a test signal sampled at fixed time steps (in microseconds).

The filter test program is started as a child process; each sample is sent
to its stdin as "<value> <taskID>" and the filtered result is read back from
its stdout as "<blurred> <taskID>":

    python signal_gen.py [path/to/filter_exported.exe]
"""
import math
import time
import threading

# IPC
import subprocess, sys
from pathlib import Path

default_exe = Path(__file__).resolve().parent.parent / "build" / "filter_exported.exe"
exe = sys.argv[1] if len(sys.argv) > 1 else str(default_exe)

p = subprocess.Popen(
    [exe],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    text=True,
    bufsize=1,      # line-buffered
)
assert p.stdin is not None and p.stdout is not None
to_filter = p.stdin
from_filter = p.stdout

# Test constants
duration = 1
rate = 2000
period_us = 1_000_000 / rate
n_samples = round(duration * rate)

filter_program = "../build/filter_exported.exe"

# Sine wave parameters
sine_freq = 10        # Hz
sine_amplitude = 256.0
sine_offset = 512.0     # mid-scale of a 10-bit ADC

# Test data generator
def signal(t_us: int) -> float:
    # Signal versus time
    return sine_offset + sine_amplitude * math.sin(2 * math.pi * sine_freq * t_us / 1_000_000)

def task_id(n):
    # used to identify order of data emitted
    return n

# Filtered collected
result: list[dict] = []

def hear():
    for line in from_filter:    # ends when the filter closes its stdout
        fields = line.split()
        if len(fields) != 2:
            continue
        data, taskID = fields
        result.append({"data": float(data), "id": int(taskID)})
    from_filter.close()

def send():
    start_ns = time.perf_counter_ns()
    n = 0
    try:
        while n < n_samples:
            t_us = round(n * period_us)

            # busy-wait: time.sleep() is too coarse for sub-millisecond periods
            target_ns = start_ns + t_us * 1000
            while time.perf_counter_ns() < target_ns:
                pass

            value = signal(t_us)
            taskID = task_id(n)

            to_filter.write(f"{value} {taskID}\n")
            to_filter.flush()
            n += 1
    except (BrokenPipeError, KeyboardInterrupt):
        pass
    finally:
        # EOF tells the filter to exit, which in turn ends hear()
        try:
            to_filter.close()
        except BrokenPipeError:
            pass

def plot():
    import matplotlib.pyplot as plt

    samples = sorted(result, key=lambda r: r["id"])
    t_s = [r["id"] * period_us / 1_000_000 for r in samples]
    inputs = [signal(round(r["id"] * period_us)) for r in samples]
    outputs = [r["data"] for r in samples]

    fig, (ax_in, ax_out) = plt.subplots(2, 1, sharex=True)
    ax_in.plot(t_s, inputs)
    ax_in.set_ylabel("input")
    ax_out.plot(t_s, outputs)
    ax_out.set_ylabel("filtered")
    ax_out.set_xlabel("time (s)")
    fig.tight_layout()
    plt.show()

# Start threads for each link
th = threading.Thread(target=send)
ts = threading.Thread(target=hear)

ts.start()
th.start()

th.join()
ts.join()
p.wait()

in_order = all(r["id"] == i for i, r in enumerate(result))
print(f"received {len(result)}/{n_samples} samples, in order: {in_order}, exit code: {p.returncode}")
plot()
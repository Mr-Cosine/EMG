import json
import prettier_console as UI
from prettier_console import default_colored_output as coutput
import serial
import time
import os

if os.path.exists("motor_config.json"):
    with open("motor_config.json", "r") as motor_config:
        try:
            config = json.load(motor_config)
        except json.decoder.JSONDecodeError:
            config = {}
else:
    config = {}

pin = config.get("pin", None)
actuation_length = config.get("actuation_length", {}) or {}
interstep_delay = config.get("interstep_delay", {}) or {}
speed_level = config.get("speed_level", None)

port = None
port_name = None

COMMAND_LIST = {
    "forward": "FWD",
    "backward": "BWD",
    "increase_speed": "SPDU",
    "decrease_speed": "SPDD"
}
def send(port, cmd):
    port.reset_input_buffer()
    port.write((COMMAND_LIST[cmd] + "\n").encode())
    return port.readline().decode(errors="replace").strip()

def set_serial():
    UI.clear_screen()

    global port, port_name
    import serial.tools.list_ports
    all_port = serial.tools.list_ports.comports()

    options = [
        {
            "text": f"{port.device} - {port.description}",
            "color": "cyan",
            "id": port.device
        }
        for port in all_port
    ]

    print("Choosing the serial port connected to the Arduino board.")
    print("Notice: the serial port has bluetooth is not the port for Arduino, connecting them will result a fail.")

    port_name_temp = UI.print_selections(f"Select from the following port(check Arduino IDE for board connection):", options)
    try:
        port_temp = serial.Serial(port_name_temp, 9600, timeout=5)
    except Exception:
        coutput.print("Connection failed, please retry.", color="red")
        UI.safe_input("Press enter to return.")
        return

    print("Waiting for board to boot...")
    BOOT_TIMEOUT = 10  # total seconds to wait before giving up
    deadline = time.time() + BOOT_TIMEOUT
    ready = False

    while time.time() < deadline:
        line = port_temp.readline().decode(errors="replace").strip()
        if line == "READY":
            ready = True
            break
        # empty or unrelated line — keep polling until the deadline

    if not ready:
        print(f"No response from {port_name_temp} within {BOOT_TIMEOUT}s.")
        print("Check the cable/port and try again.")
        port_temp.close()
        UI.safe_input("Press enter to return.")
        return

    if port is not None and port.is_open: port.close()
    port_name = port_name_temp
    port = port_temp

    print("Board ready.")
    

def setup_pin():
    UI.clear_screen()

    global pin
    DIGITAL_PINS = list(range(2, 14))
    selected = {}

    for index in range(1, 5):
        name = f"IN{index}"
        # only offer the pins that are not taken by a previous entry
        options = [
            {
                "text": f"D{digital_pin}",
                "color": "cyan",
                "id": str(digital_pin)
            }
            for digital_pin in DIGITAL_PINS if digital_pin not in selected.values()
        ]

        print("Setting up for the pin layout.")
        if selected:
            print("Assigned so far: ")
            coutput.print("\n".join(f"{key}->D{value}" for key, value in selected.items()), color="yellow")
        print()
        selected[name] = int(UI.print_selections(f"Choose the digital pin wired to {name}:", options))
        UI.clear_screen()

    UI.clear_screen()
    print("Setting up for the pin layout.")
    print("New pin layout:")
    for name, digital_pin in selected.items():
        print(f"    {name} -> D{digital_pin}")
    print()

    if UI.print_yesorno("Keep this layout?") == 'y':
        pin = selected
        print("Pin layout updated.")
    else:
        print("Pin layout unchanged.")

    UI.safe_input("Press enter to return.")

def setup_length():
    UI.clear_screen()

    global port
    if port is None:
        print("No serial port connected — set the serial port first.")
        UI.safe_input("Press enter to return.")
        return

    port.reset_input_buffer()
    print("Caliberating actuation length.")
    print("Move the motor to the bottom.")
    print("Select yes to go backward once(10 actuation cycle). Until motor slider is at the starting point.")
    response = 'y'
    while(response == 'y'):
        response = UI.print_yesorno("Continue?")
        if response == 'y':
            send(port, "backward")

    print("Move the motor to the tip.")
    print("Select yes to go forward once(10 actuation cycle). Until motor slider is at the starting point.")
    cycle_counter = 0
    response = 'y'
    while(response == 'y'):
        response = UI.print_yesorno("Continue?")
        if response == 'y':
            send(port, "forward")
            cycle_counter += 1

    actuation_length['forward'] = cycle_counter * 10

    print("Move the motor back to the bottom.")
    print("Select yes to go backward once(10 actuation cycle). Until motor slider is at the starting point.")
    cycle_counter = 0
    response = 'y'
    while(response == 'y'):
        response = UI.print_yesorno("Continue?")
        if response == 'y':
            send(port, "backward")
            cycle_counter += 1

    actuation_length['backward'] = cycle_counter * 10

def setup_delay():
    UI.clear_screen()

    global port
    if port is None:
        print("No serial port connected — set the serial port first.")
        UI.safe_input("Press enter to return.")
        return

    port.reset_input_buffer()
    print("Adjusting inter-step delay.")

def setup_level():
    UI.clear_screen()

    global port, speed_level
    if port is None:
        print("No serial port connected — set the serial port first.")
        UI.safe_input("Press enter to return.")
        return

    print("Adjusting speed levels.")
    print(f"Current number of speed levels: {speed_level if speed_level is not None else 'not set'}")
    print()

    while True:
        entry = UI.safe_input("Enter the number of speed levels (0 - 255): ").strip()
        try:
            value = int(entry)
        except ValueError:
            print(f"'{entry}' is not a whole number, try again.")
            continue
        if not 0 <= value <= 255:
            print(f"{value} is out of the 0 - 255 range, try again.")
            continue
        speed_level = value
        break

    print(f"Speed levels set to {speed_level}.")
    UI.safe_input("Press enter to return.")

def quit(save=True):
    UI.clear_screen()
    if (save==True):
        with open("motor_config.json", "w") as motor_config:
            json.dump({
                "pin": pin,
                "actuation_length": actuation_length,
                "interstep_delay": interstep_delay,
                "speed_level": speed_level
            }, motor_config)
    motor_config.close()
    print("Exiting program.")        
    exit(0)

options_home = [
    {
        "text": "set serial port(required to connect to board)",
        "color": "yellow",
        "id": "serial",
        "func": {
            "body": set_serial,
            "param": []
        }
    },
    {
        "text": "setup for pin layout",
        "color": "green",
        "id": "pin",
        "func": {
            "body": setup_pin,
            "param": []
        }
    },
    {
        "text": "caliberate actuation length",
        "color": "magenta",
        "id": "length",
        "func": {
            "body": setup_length,
            "param": []
        }
    },
    {
        "text": "adjust interstep delays",
        "color": "cyan",
        "id": "delay",
        "func": {
            "body": setup_delay,
            "param": []
        }
    },
    {
        "text": "adjust speed levels",
        "color": "blue",
        "id": "level",
        "func": {
            "body": setup_level,
            "param": []
        }
    },
    {
        "text": "quit and save",
        "color": "red",
        "id": "quit save",
        "func": {
            "body": quit,
            "param": [True]
        }
    }
]
def home_prompt():
    return "\n".join([
        coutput.get_print_string_text("- SERIAL PORT:", color="red"),
        f"    {coutput.get_print_string_text("Connected to Arduino: ", color="white")}{coutput.get_print_string_text(port_name, color="yellow")}",
        coutput.get_print_string_text("- CURRENT CONFIGURATIONS:", color="red"),
        f"    {coutput.get_print_string_text("Pin layout: ", color="white")}{coutput.get_print_string_text(pin, color="yellow")}",
        f"    {coutput.get_print_string_text("Actuation length:", color="white")}",
        f"        {coutput.get_print_string_text("Forward: ", color="white")}{coutput.get_print_string_text(actuation_length.get("forward", None), color="yellow")}",
        f"        {coutput.get_print_string_text("backward: ", color="white")}{coutput.get_print_string_text(actuation_length.get("backward", None), color="yellow")}",
        f"    {coutput.get_print_string_text("Interstep delay:", color="white")}",
        f"        {coutput.get_print_string_text("Min(fastest): ", color="white")}{coutput.get_print_string_text(interstep_delay.get("min", None), color="yellow")}",
        f"        {coutput.get_print_string_text("Max(slowest): ", color="white")}{coutput.get_print_string_text(interstep_delay.get("max", None), color="yellow")}",
        f"    {coutput.get_print_string_text("Number of speed levels: ", color="white")}{coutput.get_print_string_text(speed_level, color="yellow")}",
        f"{coutput.get_print_string_text("====================================", color="white")}"
    ])
UI.home_menu(name="Motor Setup", prompt=home_prompt, options=options_home)
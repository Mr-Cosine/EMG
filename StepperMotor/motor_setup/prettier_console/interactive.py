"""
dedicated tools to create interactive UI in command line window
"""

import tkinter
import tkinter.filedialog 
import subprocess
import os
import sys
import keyboard
from .ascii_art_font import ascii_art_font as ascii_art

#----------------------------------------------------------------------------------------------------------------------------------
class colored_output:
    """
    Colors: black, red, green, yellow, blue, magenta, cyan, white.
    """

    _NORMAL_FG = {
        'BLACK': 30, 'RED': 31, 'GREEN': 32, 'YELLOW': 33,
        'BLUE': 34, 'MAGENTA': 35, 'CYAN': 36, 'WHITE': 37
    }
    _NORMAL_BG = {
        'BLACK': 40, 'RED': 41, 'GREEN': 42, 'YELLOW': 43,
        'BLUE': 44, 'MAGENTA': 45, 'CYAN': 46, 'WHITE': 47
    }

    _BRIGHT_FG = {
        'BLACK': 90, 'RED': 91, 'GREEN': 92, 'YELLOW': 93,
        'BLUE': 94, 'MAGENTA': 95, 'CYAN': 96, 'WHITE': 97
    }
    _BRIGHT_BG = {
        'BLACK': 100, 'RED': 101, 'GREEN': 102, 'YELLOW': 103,
        'BLUE': 104, 'MAGENTA': 105, 'CYAN': 106, 'WHITE': 107
    }

    def __init__(self, bright=False):
        """
        :param bright:      boolean,    True = bright text
        """
        self._bright = bright
        if bright:
            self._fg_map = self._BRIGHT_FG
            self._bg_map = self._BRIGHT_BG
        else:
            self._fg_map = self._NORMAL_FG
            self._bg_map = self._NORMAL_BG

        self._RESET = "\033[0m"

    def _get_escape(self, fg_color=None, bg_color=None):
        """
        return ANSI color code
        """
        codes = []
        if fg_color and fg_color.upper() in self._fg_map:
            codes.append(str(self._fg_map[fg_color.upper()]))
        if bg_color and bg_color.upper() in self._bg_map:
            codes.append(str(self._bg_map[bg_color.upper()]))

        if not codes: return ""
        return f"\033[{';'.join(codes)}m"

    def print(self, *objects, color="white", background=None, sep=' ', end='\n', file=None, flush=False):
        """
        print out colored content

        :param color:       string, text color
        :param objects:     any,        printing objects
        :param background:  string,     background color
        :param sep:         string,     delimintor between printing objects
        :param end:         string,     ending style
        :param file:        any,        output stream
        :param flush:       boolean,    flush buffer zone
        """
        text = sep.join(str(obj) for obj in objects)
        colored_text = f"{self._get_escape(color, background)}{text}{self._RESET}"
        print(colored_text, end=end, file=file, flush=flush)

    def get_print_string_text(self, *objects, sep=' ', color=None, background=None):
        """
        return colored text string for print

        :param color:       string,     text color
        :param objects:     any,        printing objects
        :param background:  string,     background color
        :param sep:         string,     delimintor between printing objects
        """
        text = sep.join(str(obj) for obj in objects)
        escape = self._get_escape(color, background)
        return f"{escape}{text}{self._RESET}"

# shared instance used for every output of this module
default_colored_output = colored_output(bright=False)

#----------------------------------------------------------------------------------------------------------------------------------
def display_width(text):
    """
    get consistent displayed width for latin + chinese characters string

    :param text:            string,     the text for getting length of display
    """
    width = 0
    for ch in str(text):
        if '\u4e00' <= ch <= '\u9fff' or '\u3000' <= ch <= '\u303f' or '\uff00' <= ch <= '\uffef':
            width += 2
        else:
            width += 1
    return width

#---------------------------------------------------------------------------------------------------------------------------------- 
def safe_input(prompt=""):
    """
    input with preventions of previous input buffer overflow

    :param prompt:          string,     text displayed before asking for input
    """
    if prompt: default_colored_output.print(prompt, color='white', end='', flush=True)

    result = []

    while True:
        try:
            event = keyboard.read_event(suppress=True)
            
            if event.event_type == keyboard.KEY_DOWN:
                if event.name == 'enter':
                    print()
                    return ''.join(result)
                elif event.name == 'backspace':
                    if result:
                        result.pop()
                        print('\b \b', end='', flush=True)
                elif event.name == 'space':
                    result.append(' ')
                    default_colored_output.print(' ', color='white', end='', flush=True)
                elif len(event.name) == 1:
                    result.append(event.name)
                    default_colored_output.print(event.name, color='white', end='', flush=True)
        except:
            return input()

#----------------------------------------------------------------------------------------------------------------------------------

def clear_screen():
    """
    flush content on the console
    """
    command = 'cls' if os.name == 'nt' else 'clear'
    subprocess.run(command, shell=True, check=False)

#----------------------------------------------------------------------------------------------------------------------------------

def print_banner(input_string, color='white'):
    """
    print a text rendered in the banner ascii-art style.

    :param input_string:    string,     the text to render, '\\n' splits it into rows
    :param color:           string,     text color (default: white)
    """
    default_colored_output.print(ascii_art.default_banner(input_string), color=color, background=None)

def print_header(input_string, color='white'):
    """
    print a text rendered in the header ascii-art style.

    :param input_string:    string,     the text to render, '\\n' splits it into rows
    :param color:           string,     text color (default: white)
    """

    default_colored_output.print(ascii_art.default_header(input_string), color=color, background=None)

#----------------------------------------------------------------------------------------------------------------------------------

def select_files(file_types):
    """
    opens file selection dialog.

    :param file_types:      [string...],    list of wanted types, eg. ['jpg', 'pdf', 'txt]
    :param 

    :return:                the selected file location, if not selected return None
    """
    root = tkinter.Tk()
    root.withdraw()

    print()

    if isinstance(file_types, str):
        file_types = [file_types]

    filetypes = []
    for ft in file_types:
        ext = ft.lstrip('*.')
        desc = ext.lower() + ' File'
        filetypes.append((desc, ft))
    filetypes.append(('All Files', '*.*'))

    files = tkinter.filedialog.askopenfilenames(
        title='File selection',
        filetypes=filetypes
    )

    root.destroy()
    return files if files else None

#----------------------------------------------------------------------------------------------------------------------------------

def select_folder():
    """
    opens file selection dialog.

    :return:                the selected folder path, if not selected return None
    """
    root = tkinter.Tk()
    root.withdraw()
    folder = tkinter.filedialog.askdirectory(title='Folder selection')
    root.destroy()
    return folder if folder else None;

#----------------------------------------------------------------------------------------------------------------------------------

def print_yesorno(prompt):
    """
    print yes or no choice to let user select.

    :param prompt:          string/func,    the prompt displayed before asking.

    :return:                string,         'y' or 'n'
    """
    options = [
        {
            'text': 'Yes',
            'color': 'green',
            'id': 'y'
        },
        {
            'text': 'No',
            'color': 'red',
            'id': 'n'
        }
    ]
    return print_selections(prompt, options)

def print_selections(prompt, options):

    """
    print menu options could be operated by up and down arrow to select. returns the id of the option.

    :param prompt"          string/func,    the prompt string or prompt builder function(need to return string)
    :param options:         dict,           the options for choose
                            {
                                text:   string,     the option text displayed
                                color:  string,     color of the option displayed(default: white)
                                id: string,         unique identifier for each option
                            }
    """
    def hide_cursor(): print('\033[?25l', end='', flush=True)
    def show_cursor(): print('\033[?25h', end='', flush=True)

    active_index = 0

    # Calculate max length once for lining up entries
    target_w = max(display_width(str(option['text'])) for option in options)
    target_w = 10 if target_w < 10 else target_w

    if not callable(prompt): default_colored_output.print(prompt, color='white')
    else: default_colored_output.print(prompt(), color='white')

    def print_options():
        for idx, option in enumerate(options):
            lead_cursor = ">·" if idx == active_index else " "
            lag_cursor = "·<" if idx == active_index else " "

            text = option['text']
            color = option.get('color') or 'white'

            current_w = display_width(text)
            filler = ('·' if active_index == idx else " ") * (max((target_w - current_w), 0) + 4 - display_width(lead_cursor))

            default_colored_output.print(' '*(4 - display_width(lead_cursor)), color='white', end='', flush=True)
            default_colored_output.print(f"{lead_cursor}", color='white', end='', flush=True)
            default_colored_output.print(f"{idx+1}.{text}", color=color, end='', flush=True)
            default_colored_output.print(f"{filler}{lag_cursor}", color='white', flush=True)

    hide_cursor()
    # Print menu
    print_options()

    # Main loop for arrow key navigation
    while True:
        # Move cursor up to redraw menu
        print(f"\033[{len(options) + 1}A", flush=True)
        print_options()
        
        # Get key press
        key = keyboard.read_event(suppress=True)
        
        if key.event_type == keyboard.KEY_DOWN:
            if key.name == 'up':
                active_index = (active_index - 1) % len(options)
            elif key.name == 'down':
                active_index = (active_index + 1) % len(options)
            elif key.name == 'enter':
                show_cursor()
                return options[active_index]['id']
            else: continue

def menu(name, prompt, options, home=False):
    """
    open up a menu with single selection toward other menu

    :param name:            string,         the name of the panel
    :param prompt:          string/func,    the prompt pr a prompt builder function(need to return string)
    :param options:         [dict],         the options informations
                            [
                                {
                                text:   string, 
                                color:  string, 
                                id:     string, 
                                func: {
                                    body:   function, 
                                    param:  [string...],
                                    }
                                }
                                ...
                            ]
    """
    # gather the necessary information for print_selections(options)
    menu_options = [{'text': option['text'], 'color': option.get('color') or None, 'id': option['id']} for option in options]

    # add back option to allow to return to previous menu
    if home: menu_options.append({'text': 'quit program', 'color': '', 'id': 'quit'})
    else: menu_options.append({'text': 'return to previous', 'color': '', 'id': 'back'})
    # check for any duplicated id to prevent ambiguation in searching the options
    if len([d.get('id') for d in menu_options]) != len(set([d.get('id') for d in menu_options])):
        raise Exception('duplicated id for options')

    while True:
        clear_screen()
        if not home:
            print_header(name)
        else:
            print_banner(name)
        if not callable(prompt): default_colored_output.print(prompt, color='white')
        else: default_colored_output.print(prompt(), color='white')
        print()

        chosen_id = print_selections(
            f'Choose from the following {len(menu_options)} options:', 
            menu_options
        )
    
        if chosen_id in ['back', 'quit']:
            return chosen_id

        selected = None
        for opt in options:
            if opt['id'] == chosen_id:
                selected = opt
                break
            
        if not selected: raise Exception(f'no defined action for an option: {selected}.')

        # If there's a 'func', execute it
        if 'func' in selected:
            func = selected['func']
            if callable(func):
                result = func()
            elif isinstance(func, dict) and 'body' in func:
                body = func['body']
                params = func.get('param', [])
                result = body(*params)
            else: raise Exception(f'action provided for an option: {selected} is not callable.')
            if result == 'quit':
                return result

def home_menu(name, prompt, options):
    """
    open a root menu with single selection toward other menu

    :param name:            string,     the name of the panel
    :param prompt:          string,     the prompt
    :param options:         [dict...],  the options informations
                            [
                                {
                                text:   string, 
                                color:  string, 
                                id:     string, 
                                func: {
                                    body:   function, 
                                    param:  [string...],
                                    }
                                }
                                ...
                            ]
    """
    result = menu(name, prompt, options, home=True)
    if result == 'quit': quit_program(0)

def quit_program(code: int = 0):
    clear_screen()
    default_colored_output.print("Exiting program.", color='white')
    default_colored_output.print('='*30, color='white')
    
    try: keyboard.unhook_all()
    except: pass
    finally: sys.exit(code)
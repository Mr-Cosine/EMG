"""
command line interface for prettier_console
"""

import zipfile
import hashlib
from datetime import datetime
import argparse
import os
import sys

from .ascii_art_font import ascii_art_font


def _cmd_updatefont(args):
    if args.banner: style = 'banner'
    elif args.header: style = 'header'
    else: style = args.style if args.style else os.path.splitext(os.path.basename(args.change_font))[0]

    try:
        font_folder_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ascii_art_font', 'font')
        target_path = os.path.join(font_folder_path, f'{style}.json')

        if os.path.exists(target_path):
            randID = hashlib.md5(f'{datetime.now().timestamp()}'.encode()).hexdigest()
            zip_name = f'legacyfont-{style}-{randID}.zip'
            os.makedirs(os.path.join(font_folder_path, "legacy_fonts"), exist_ok=True)
            zip_path = os.path.join(font_folder_path, "legacy_fonts", zip_name)

            with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                zipf.write(target_path, arcname=f'{style}.json')

        ascii_art_font.set_cset(style, args.change_font)
        print(f'Font updated.')
    except Exception as e:
        print(f'Error: {e}', file=sys.stderr)
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(prog='prettier_console')
    subparsers = parser.add_subparsers(dest='command', required=True)

    updatefont = subparsers.add_parser(
        'updatefont',
        help='add or replace an ascii-art font style from a text file'
    )
    updatefont.add_argument(
        'change_font',
        help="path to a text file with A-Z + space glyphs, one row per line, each character separated by '/'"
    )
    target = updatefont.add_mutually_exclusive_group()
    target.add_argument('--banner', action='store_true', help='save as the banner style')
    target.add_argument('--header', action='store_true', help='save as the header style')
    target.add_argument('--style', help='style name to save as (default: the file name without extension)')
    updatefont.set_defaults(func=_cmd_updatefont)

    args = parser.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()

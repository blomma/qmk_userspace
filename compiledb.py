#!/usr/bin/env python3
"""Generates compile_commands.json for every build target in qmk.json.

`qmk compile --compiledb` writes a database for a single keyboard and keymap,
so clangd guesses the flags for the files of every other target and reports
errors that the real build does not have. This builds the database for each
target in qmk.json and merges them into one.

Each `qmk compile --compiledb` starts by deleting the whole .build folder,
which holds headers the database points at, so all targets are compiled again
at the end to put them back.

Usage: python3 compiledb.py
"""

import json
import subprocess
import sys
from pathlib import Path

USERSPACE = Path(__file__).resolve().parent
DATABASE = USERSPACE / 'compile_commands.json'


def run(command):
    print('+', ' '.join(command), flush=True)
    subprocess.run(command, cwd=USERSPACE, check=True)


def main():
    targets = json.loads((USERSPACE / 'qmk.json').read_text())['build_targets']

    entries = []
    seen = set()
    for keyboard, keymap, *rest in targets:
        env = rest[0] if rest else {}
        command = ['qmk', 'compile', '--compiledb', '-kb', keyboard, '-km', keymap]
        for name, value in env.items():
            command += ['-e', f'{name}={value}']
        run(command)

        # Files shared between targets, such as the community modules, keep
        # the flags of the first target that builds them.
        for entry in json.loads(DATABASE.read_text()):
            file = str(Path(entry['directory'], entry['file']).resolve())
            if file not in seen:
                seen.add(file)
                entries.append(entry)

    run(['qmk', 'userspace-compile'])

    DATABASE.write_text(json.dumps(entries, indent=4) + '\n')
    print(f'Wrote {len(entries)} entries for {len(targets)} targets to {DATABASE.name}')


if __name__ == '__main__':
    try:
        main()
    except subprocess.CalledProcessError as error:
        sys.exit(error.returncode)

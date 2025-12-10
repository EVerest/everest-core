#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#

def snake_case(word: str) -> str:
    """Convert capital case to snake case.
    Only alphanumerical characters are allowed. Only inserts camelcase
    between a consecutive lower and upper alphabetical character and
    lowers first letter.
    """

    # special handling for soc
    word = word.replace('SoC', 'Soc')

    out = ''
    if len(word) == 0:
        return out
    cur_char: str = ''
    for i, character in enumerate(word):
        if i == 0:
            cur_char = character
            if not cur_char.isalnum():
                raise Exception('Non legal character in: ' + word)
            out += cur_char.lower()
            continue
        last_char: str = cur_char
        cur_char = character
        if (last_char.islower() and last_char.isalpha() and
                cur_char.isupper() and cur_char.isalpha):
            out += '_'
        if not cur_char.isalnum():
            out += '_'
        else:
            out += cur_char.lower()

    return out

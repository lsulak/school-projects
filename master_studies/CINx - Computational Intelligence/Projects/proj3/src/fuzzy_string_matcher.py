#!/usr/bin/env python3
# -*- coding: utf-8 -*-
""" 
.. Created on 2017-01-11
.. codeauthor:: Ladislav Šulák (xsulak04@stud.fit.vutbr.cz)
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)

"""
import time
from argparse import ArgumentParser

from prepare_data import DataGenerator


class StringMatcherTemplate(object):

    def __init__(self, pattern, text):

        if not pattern:
            raise ValueError("No pattern provided")
        if len(pattern) > 31:
            raise ValueError("Pattern is too long")
        self._pattern = pattern
        self._pattern_len = len(pattern)

        self._text = text

    def find_first(self, custom_text=None):
        raise NotImplementedError("To be implemented")

    def find_all(self):
        all_occurrences = list()
        text = self._text
        prev_pos = 0

        while 1:
            pos = self.find_first(custom_text=text)
            if pos == -1 or not text:
                return all_occurrences

            pos += prev_pos
            all_occurrences.append(pos)
            prev_pos = pos + self._pattern_len

            text = text[pos+self._pattern_len:]

    def mark(self):
        pos = self.find_all()
        text = self._text

        for i in pos[::-1]:
            text = f"{text[:i]}*{text[i:i+self._pattern_len]}*{text[self._pattern_len+i:]}"

        print(text)


class NaiveStringMatcher(StringMatcherTemplate):
    """
    Inspirated from:
    https://www.geeksforgeeks.org/searching-for-patterns-set-1-naive-pattern-searching/
    """
    def __init__(self, pattern, text):

        StringMatcherTemplate.__init__(self, pattern, text)

    def find_first(self, custom_text=None):
        text = self._text if not custom_text else custom_text

        for i in range(len(text) - self._pattern_len + 1):
            for j in range(self._pattern_len):
                if text[i+j] != self._pattern[j]:
                    break

                if j == self._pattern_len - 1:
                    return i
        return -1


class FuzzyStringMatcher(StringMatcherTemplate):
    """
    Inspirated by:
        https://www.programmingalgorithms.com/algorithm/fuzzy-bitap-algorithm
    """
    ERRORS = 2
    ASCII_RANGE = 127

    def __init__(self, pattern, text):
        StringMatcherTemplate.__init__(self, pattern, text)

    def find_first(self, custom_text=None):
        text = self._text if not custom_text else custom_text

        result = -1

        err_arr = [~1 for _ in range(self.ERRORS+1)]  # bit array
        pattern_mask = [~0 for _ in range(self.ASCII_RANGE + 1)]

        for i in range(self._pattern_len):
            pattern_mask[ord(self._pattern[i])] &= ~(1 << i)

        for i, char in enumerate(text):
            old_err_arr = err_arr[0]  # update bit arrays

            err_arr[0] |= pattern_mask[ord(char)]
            err_arr[0] <<= 1

            for d in range(1, self.ERRORS + 1):
                tmp = err_arr[d]

                # Substitution.
                err_arr[d] = (old_err_arr & (err_arr[d] | pattern_mask[ord(char)])) << 1
                old_err_arr = tmp

            if (err_arr[self.ERRORS] & (1 << self._pattern_len)) == 0:
                result = (i - self._pattern_len) + 1
                break

        return result

    def spell_corrector(self):
        pos = self.find_first()
        print(f"{self._pattern} -> {self._text[pos:pos + self._pattern_len]}")


def get_args():
    parser = ArgumentParser(
        description="Apply naive/fuzzy string matcher from a pattern on a text."
    )
    parser.add_argument(
        "--find_all", help="Find all occurrences of a pattern beginning in an input text.", action="store_true"
    )
    parser.add_argument(
        "--mark_all", help="Mark all occurrences of a pattern beginning in an input text.", action="store_true"
    )
    parser.add_argument(
        "-spell_check",
        help="Perform spell checker of an input pattern. "
             "It finds a (pattern) word in text and returns such (correct) word from text.",
        action="store_true"
    )
    return parser.parse_args()


if __name__ == "__main__":
    noise = FuzzyStringMatcher.ERRORS  # or 0 for turning it off
    data = DataGenerator(pattern_noise_level=noise).extract()

    args = get_args()

    for matcher_cls in (FuzzyStringMatcher, NaiveStringMatcher):
        start = time.time()
        for pattern, text in data:
            matcher = matcher_cls(pattern, text)
            if args.find_all:
                matcher.find_all()

            if args.mark_all:
                print(matcher.mark())

            if matcher_cls is FuzzyStringMatcher and args.spell_check:
                matcher.spell_corrector()

        end = time.time()
        print(f"{matcher_cls.__name__}: {end - start}")

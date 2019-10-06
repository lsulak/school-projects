# !/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-01-11
.. codeauthor:: Ladislav Šulák (xsulak04@stud.fit.vutbr.cz)
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)

"""
import random


class DataGenerator(object):
    DICT = "big.txt"
    DATASET_SIZE = 25_000

    def __init__(self, pattern_noise_level=0):
        self._pattern_noise_level = pattern_noise_level

    def extract(self):
        final_data = list()
        with open(file=self.DICT, mode='r') as in_file:
            remaining_dataset_size = self.DATASET_SIZE
            lines = in_file.readlines()

            while 1:
                final_lines = lines[:remaining_dataset_size]

                for line in final_lines:
                    tmp_line = line.strip()
                    if not tmp_line:
                        continue
                    words = tmp_line.split(' ')
                    if len(words) < 3:
                        continue

                    random_word = random.randint(0, len(words)-1)
                    pattern = words[random_word]
                    len_pattern = len(pattern)
                    if len_pattern < 10 or len_pattern > 31:
                        continue

                    # if len(tmp_line) < 50:
                    #    continue

                    for _ in range(self._pattern_noise_level):
                        noise_letter_pos = random.randint(0, len_pattern - 1)
                        random_letter = 'X'
                        pattern = pattern[:noise_letter_pos] + random_letter + pattern[noise_letter_pos + 1:]

                    curr_data = pattern, tmp_line
                    final_data.append(curr_data)

                len_final_data = len(final_data)
                if len_final_data < self.DATASET_SIZE:
                    remaining_dataset_size = self.DATASET_SIZE - len_final_data
                else:
                    break

        avg_pattern_size = sum(map(lambda x: len(x), list(zip(*final_data))[0])) / len(final_data)
        avg_text_size = sum(map(lambda x: len(x), list(zip(*final_data))[1])) / len(final_data)
        print(f"Dataset has length: {len(final_data)} and noise level: {self._pattern_noise_level}")
        print(f"The average pattern size is: {avg_pattern_size} and average text size is: {avg_text_size}")
        return final_data


if __name__ == "__main__":
    data = DataGenerator().extract()
    for p, t in data:
        print(f"{p}, {t}")


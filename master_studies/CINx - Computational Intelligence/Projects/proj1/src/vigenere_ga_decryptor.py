#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-10-29
.. codeauthor:: Ladislav Šulák (laco.sulak@gmail.com)
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)

    Genetic algorithm implementation is based on:
        * materials from Computational Intelligence course from TEI
        * http://www.obitko.com/tutorials/genetic-algorithms/ga-basic-description.php
        * https://github.com/rodhilton/Geneticrypt

    Markov Model classifier (gibberish_detector) is based on:
        * https://github.com/exp0se/dga_detector

    Other materials:
        * [1996] The Applications of Genetic Algorithms in Cryptanalysis by A. J. Bagnall
        * [2003] Solving Substitution Ciphers with Genetics Algorithm by Joe Gester
        * [2008] Cryptoanalysis using genetic algorithms by P. Bergmann, Karel & Scheidler, Renate & Jacob, Christian
        * [2008] Applying Genetic Algorithms for Searching KeySpace of Polyalphabetic Substitution Ciphers
                    by Ragheb Toemeh and Subbanagounder Arumugam
        * [2011] A cryptanalytic attack on Vigenère cipher using genetic algorithm
                    by Omran, Safaa & Al-Khalid, A.S. & Alsaady, Dalal
"""
import frogress
import logging
import re
import sys
from argparse import ArgumentParser
from collections import OrderedDict
from datetime import datetime
from functools import reduce
from itertools import product
from os.path import abspath, dirname, getsize, isfile, join, pardir
from pycipher import Vigenere
from string import ascii_lowercase
from random import choice, uniform, randint

sys.path.append(abspath(join(dirname(__file__), pardir)))
_ROOT_DIR = abspath(join(dirname(__file__), pardir))
DATA_DIR = join(_ROOT_DIR, "data")

from libs.markov_classifier.gib_detect import GibberishDetector

MAX_GENERATION_AMOUNT = 160
POPULATION_SIZE = 100  # Only number divided by 2 !
THRESHOLD_SCORE_PER_WORD = 0.90


class Population(object):
    """
    Class representing and encapsulating Population used in Genetic Algorithms
    along with its operations and data structures.
    """
    def __init__(self, encrypted_text, force_train_mm, population_size, member_size):
        """
        Entry point of Population class.

        :param int population_size: defines a number of members in population
        :param int member_size: defines a length of each member in population
        """
        self._encrypted_text = encrypted_text.split()

        if population_size % 2 != 0:
            raise ValueError("Population must be divided by 2 for GA operations.")
        self._population_size = population_size

        self._member_size = member_size

        self._population = None
        self._fitness_values = None

        self._gib_detector = GibberishDetector(force_train_mm)

    def __str__(self):
        """
        Method useful for printing Population class (class Population -> string).

        :return: string representation of a current population
        :rtype: str
        """
        population_to_print = str("Printing current population:")
        for cnt, (member, fitness) in enumerate(zip(self._population, self._fitness_values)):
            population_to_print += "\n[{}]: '{}' (score: {})".format(cnt, member, fitness)

        return population_to_print

    @staticmethod
    def _decode_binary_string(bin_str):
        """
        Method useful for decoding string containing zeroes and ones into ascii text.
        Each character must be represented by 8 bits i input text.

        :param: string bin_str: input binary string (values 0 and 1) which is being decoded
        :return: final string containing a-z characters
        :rtype: str
        """
        final_string = str()
        if len(bin_str) % 8 != 0:
            raise ValueError("Binary string has bad format, it must be length divided by 8: {}".format(bin_str))

        for i in range(len(bin_str) // 8):
            ord_value = int(bin_str[i * 8:(i + 1) * 8], 2)

            # if must be withing the range of alphabet (lowercase, we are working with it)
            if ord_value > ord('z'):   # bigger than 122
                offset = ord_value - ord('z')
                if offset > 25:  # 'z' - 'a' = 25
                    offset = offset % 26  # so only offset values in range 0-25

                # for example from 123 it will be 97 - 'a'
                # and from 200 it will be 200-122=78, 78%26=0 and 0+97=97 - 'a
                ord_value = offset + ord('a')

            elif ord_value < ord('a'):  # lower than 97
                offset = ord('a') - ord_value
                if offset > 25:  # 'z' - 'a' = 25
                    offset = offset % 26  # so only offset values in range 0-25

                # for example from 96 it will be 122 - 'z'
                # and from 6 it will be 96-6=90, 90%26=12 and 122-12=110 - 'n'
                ord_value = ord('z') - offset

            final_string += chr(ord_value)
        return final_string

    @staticmethod
    def perform_1p_crossover(array_of_parent_pairs):
        """
        Performs operation of crossover (1 point) on each member of a current population.

        :param: list(tuple) array_of_parent_pairs: list of member pairs to change
        :return: new list of pairs which are changed
        :rtype: [(item, item)]
        """
        new_members_from_parents = list()

        # Len of parent1 and parent2 is same (as every population member)
        for parent1, parent2 in array_of_parent_pairs:
            crossover_point = randint(1, len(parent1) - 1)
            list_parent1 = list(parent1)  # from string to list for better manipulation
            list_parent2 = list(parent2)

            new_parent1 = list_parent1[0:crossover_point] + list_parent2[crossover_point:]
            new_parent2 = list_parent2[0:crossover_point] + list_parent1[crossover_point:]

            new_members_from_parents.append((new_parent1, new_parent2))

        return new_members_from_parents

    @staticmethod
    def perform_2p_crossover(array_of_parent_pairs):
        """
        Performs operation of crossover (Two points) on each member of a current population.

        :param: list(tuple) array_of_parent_pairs: list of member pairs to change
        :return: new list of pairs which are changed
        :rtype: [(item, item)]
        """
        new_members_from_parents = list()

        # Len of parent1 and parent2 is same (as every population member)
        for parent1, parent2 in array_of_parent_pairs:
            while 1:
                crossover_point1 = randint(1, len(parent1) - 1)
                crossover_point2 = randint(1, len(parent1) - 1)
                if crossover_point1 != crossover_point2:
                    break

            list_parent1 = list(parent1)  # from string to list for better manipulation
            list_parent2 = list(parent2)

            if crossover_point1 < crossover_point2:
                new_parent1 = (list_parent1[0:crossover_point1] +
                               list_parent2[crossover_point1:crossover_point2] +
                               list_parent1[crossover_point2:])

                new_parent2 = (list_parent2[0:crossover_point1] +
                               list_parent1[crossover_point1:crossover_point2] +
                               list_parent2[crossover_point2:])
            else:
                new_parent1 = (list_parent2[0:crossover_point2] +
                               list_parent1[crossover_point2:crossover_point1] +
                               list_parent2[crossover_point1:])

                new_parent2 = (list_parent1[0:crossover_point2] +
                               list_parent2[crossover_point2:crossover_point1] +
                               list_parent1[crossover_point1:])

            new_members_from_parents.append((new_parent1, new_parent2))

        return new_members_from_parents

    def _get_random_member(self):
        """
        Pseudo-random generation of 1 member of population.

        :return: population member
        :rtype: string
        """
        return ''.join([choice(ascii_lowercase) for _ in range(self._member_size)])

    def _select_one_in_roulette_wheel(self):
        """
        Let the wheel spun once and select the parent.

        :return: parent for next generation
        """
        self.calculate_fitness()
        sum_of_population = sum(self._fitness_values)  # sum of all scores of each member of a current population
        pick = uniform(0, sum_of_population)
        current = 0
        for member, score in zip(self._population, self._fitness_values):
            current += score
            if current > pick:
                return member

    def calculate_fitness(self):
        """
        Calculate fitness values of all members in a current population.
        """
        self._fitness_values = [self.evaluate_fitness_of_member(member) for member in self._population]

    def evaluate_fitness_of_member(self, member):
        """
        Evaluates member from a current population and returns resulting value (float number as score).

        :param str member: member of current population
        :return: fitness score
        :rtype: float
        """
        fitness_score = list()
        for encrypted_word in self._encrypted_text:
            decrypted_word = Vigenere(member).decipher(encrypted_word)
            fitness_score.append(self._gib_detector.word_language_score_level(decrypted_word))

        return sum(fitness_score)

    def get_fittest_member(self):
        """
        Getting the fittest member of current population also with its fitness score.

        :return: information about fittest member
        :rtype: (string, float)
        """
        population_and_fitness = list(zip(self._population, self._fitness_values))

        # Revert because in case that there are more values which are same, pick the last one.
        member, score = max(population_and_fitness[::-1], key=lambda x: x[1])
        return member, score

    def init_new(self):
        """
        Initializing brand new population, each member has same length.

        :return: information about fittest member
        :rtype: (string, float)
        """
        self._population = [self._get_random_member() for _ in range(self._population_size)]

    def perform_roulette_wheel(self):
        """
        Create tuples from existing keys using roulette wheel method.

        :return: list of tuples represendint list of member pairs
        :rtype: [(item, item)]
        """
        array_of_parent_pairs = list()

        for _ in range(len(self._population) // 2):
            parent1 = self._select_one_in_roulette_wheel()
            parent2 = self._select_one_in_roulette_wheel()

            bin_parent1 = ''.join("{0:08b}".format(ord(x)) for x in parent1)  # GA works with binary strings!
            bin_parent2 = ''.join("{0:08b}".format(ord(x)) for x in parent2)

            array_of_parent_pairs.append((bin_parent1, bin_parent2))

        return array_of_parent_pairs

    def perform_mutation(self, array_of_member_pairs):
        """
        Performs operation of mutation (2 points) on each member of a current population.

        :param: list(tuple) array_of_member_pairs: list of member pairs to change
        :return: new list of pairs which are changed
        :rtype: [(item, item)]
        """
        new_population = list()
        for member1, member2 in array_of_member_pairs:
            fst_index = randint(0, len(member1) - 1)
            snd_index = randint(0, len(member2) - 1)

            list_member1 = list(member1)
            list_member2 = list(member2)

            list_member1[fst_index] = list_member2[snd_index]
            list_member1[snd_index] = list_member2[fst_index]
            list_member2[fst_index] = list_member1[snd_index]
            list_member2[snd_index] = list_member1[fst_index]

            str_member1 = self._decode_binary_string(''.join(list_member1))
            str_member2 = self._decode_binary_string(''.join(list_member2))

            new_population.append(str_member1)  # adding 2 mutated members to new population
            new_population.append(str_member2)

        self._population = new_population


class KeyLength(object):
    """
    Class tries to find all possible key lengths of encrypted text (by Vigenere cipher).
    """

    @staticmethod
    def add_to_count_dict(saved_key_lens, key_len):
        """
        Helper method which adds information about occurence of key length to final dictionary.

        :param dict saved_key_lens: saved key lengths
        :param int key_len: key len which will be added to final dict
        """
        if key_len in range(3):  # do not add short keys (mainly keys of len 1 or 2)
            return

        if key_len not in saved_key_lens.keys():
            saved_key_lens[key_len] = 1
        else:
            saved_key_lens[key_len] += 1

    @staticmethod
    def factor(n):
        """
        Method for computing factor used during calculation.

        :param int n: final fittest member = solution
        :return: helper set
        :rtype: set
        """
        return set(reduce(list.__add__, ([i, n // i] for i in range(1, int(n ** 0.5) + 1) if n % i == 0)))

    @staticmethod
    def find_repeated_substrings(cipher_text, sub_length):
        """
        Method tries to find substrings which are being repeated in input text.

        :param str cipher_text: input encrypted text
        :param int sub_length: length of substring being used during calculation
        :return: helper dict used during calculation
        :rtype: dict
        """
        subs = {}
        for i in range(0, len(cipher_text) - sub_length):
            substring = cipher_text[i:i + sub_length]
            if cipher_text.count(substring) > 1 and not substring in subs.keys():
                subs[substring] = [m.start() for m in re.finditer(substring, cipher_text)]

        return subs

    def get_keylength(self, cipher_text):
        """
        Converts found solution into pretty human-readable format.

        :param str cipher_text: input encrypted text
        :return: all possible key lenghs
        :rtype: dict
        """
        substrings = self.find_repeated_substrings(cipher_text, 3)
        diffs = []
        for substring in substrings.keys():
            for i in range(0, len(substrings[substring]) - 1):
                diffs.append(substrings[substring][i + 1] - substrings[substring][i])
        factor_counts = {}

        for d in diffs:
            factors = self.factor(d)
            for f in factors:
                self.add_to_count_dict(factor_counts, f)

        return factor_counts


class VigenereDecryptor(object):
    """
    Class representing and handling Vigenere cipher decryption with the usage of Genetic Algorithms.
    """

    def __init__(self, input_to_decrypt=None, key_len=None, force_train=False):
        """
        Entry point of this class which initializes text to be decrypted.

        :param str input_to_decrypt: input text or name of input file containing encrypted text.
        :param bool force_train: if train new markov model classifier or use already trained one
        """
        if isfile(input_to_decrypt) and getsize(input_to_decrypt) > 0:
            with open(input_to_decrypt, mode='r') as f:
                self.logger.info("Trying to decryption of text from file '{}'".format(input_to_decrypt))
                self._encrypted_text = f.read()

        elif isinstance(input_to_decrypt, str):
            self._encrypted_text = input_to_decrypt

        else:
            raise ValueError(
                "Parameter 'input_to_decrypt' has bad type. Provide path to file or string containing encrypted text."
            )

        self._key_len = int(key_len) if key_len else None  # mostly for smaller samples
        self._force_train = force_train  # regarding to fitness function (measurement) - Markov Model learning process

    @property
    def logger(self):
        """
        Logger property which for initialization a current logger.

        :return: initialized logger
        :rtype: Logger
        """
        logger = logging.getLogger(self.__class__.__name__)

        curr_date_str = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        log_file = '_'.join((curr_date_str, "output.log"))

        logging.basicConfig(
            level=logging.DEBUG,
            format="%(asctime)s [%(name)s|%(filename)s] %(levelname)s: %(message)s",
            datefmt="%Y-%m-%d %H:%M:%S",
            filename=log_file
        )
        return logger

    @staticmethod
    def _bruteforce_keys_generation(key_len):
        return (''.join(key) for key in product(ascii_lowercase, repeat=key_len))

    @staticmethod
    def _get_printable_solution(solution):
        """
        Converts found solution into pretty human-readable format.

        :param OrderedDict solution: final fittest member = solution
        :return: statistics with fittest member for printing purposes
        :rtype: str
        """
        pprint_final_solution = list()
        for stat in solution.items():
            pprint_final_solution.append(": ".join(str(item) for item in stat))

        return ", ".join(pprint_final_solution)

    def _correct_plaintext(self, final_plaintext):
        """
        Corrects the input string which now has words separated with space and is lowercase.

        :return: corrected string
        :rtype: str
        """
        special_chars_indexes = {match.start(): match[0] for match in re.finditer("[^a-zA-Z]", self._encrypted_text)}
        uppercase_indexes = [match.start() for match in re.finditer("[A-Z]", self._encrypted_text)]

        text_with_spaces = str()
        final_cnt = 0

        for index, _ in enumerate(self._encrypted_text):
            if index in special_chars_indexes.keys():
                text_with_spaces += special_chars_indexes[index]

            else:
                text_with_spaces += final_plaintext[final_cnt]
                final_cnt += 1

        return ''.join([c if i in uppercase_indexes else c.lower() for i, c in enumerate(text_with_spaces)])

    def _get_possible_key_lens(self):
        """
        Computes list of key lengths which could lead to successful decrypting of text with Vigenere cipher.

        :return: list of key lengths
        :rtype: list
        """

        key_lengths = KeyLength().get_keylength(self._encrypted_text)
        sorted_key_lengths = sorted(key_lengths, key=key_lengths.__getitem__, reverse=True)
        self.logger.info("Found {0} candidate key lengths".format(len(sorted_key_lengths)))
        self.logger.info("Key lengths are: {0}".format(sorted_key_lengths))

        return key_lengths

    def _run_evolution(self, key_len):
        """
        Method for running evolution process. It works with 1 set of probable solutions (determined by key_len).

        :param int key_len: a length of key (solution - population member)
        :return: fittest member with its statistics
        :rtype: OrderedDict
        """
        self.logger.info("Evolution started")

        fittest_member_stats = OrderedDict()

        population = Population(self._encrypted_text, self._force_train, POPULATION_SIZE, key_len)
        population.init_new()

        threshold_score = THRESHOLD_SCORE_PER_WORD * len(self._encrypted_text.split())

        for gen_nr in frogress.bar(range(1, MAX_GENERATION_AMOUNT + 1)):
            population.calculate_fitness()

            self.logger.debug("Generation number {}.".format(gen_nr))
            self.logger.debug(population)  # change this line if you want to see each members of populations with score!

            member, score = population.get_fittest_member()

            if gen_nr == MAX_GENERATION_AMOUNT or score >= threshold_score:
                self.logger.info(
                    "Fittest member from all generations is: '{}' (score: {}, from generation: {})".format(
                        member, score, gen_nr)
                )
                fittest_member_stats["member"] = member
                fittest_member_stats["fitness_score"] = score
                fittest_member_stats["generation"] = gen_nr
                fittest_member_stats["key_len"] = key_len
                fittest_member_stats["reached_threshold"] = True if score >= threshold_score else False
                return fittest_member_stats

            # Forming new population - picking pairs of parents, do crossover and do mutation.
            parent_pairs = population.perform_roulette_wheel()

            # crossovered_pairs = population.perform_1p_crossover(parent_pairs)
            crossovered_pairs = population.perform_2p_crossover(parent_pairs)
            population.perform_mutation(crossovered_pairs)

    def brute_force(self):
        """
        Experimental method for running brute force approach for
        decrypting of text encrypted by Vigenere cipher.
        """
        self.logger.info("Trying to decryption of text '{}' by brute forcing".format(self._encrypted_text))

        if self._key_len:
            possible_key_lens = [self._key_len]

        else:
            possible_key_lens = self._get_possible_key_lens()
            if not possible_key_lens:
                self.logger.error("There is no possible key length or very short, application ends")
                return

        # Increase a threshold in comparison to threshold in GAs.
        threshold_score = (THRESHOLD_SCORE_PER_WORD * 1.25) * len(self._encrypted_text.split())
        final_key = None

        for key_len in possible_key_lens:
            space_dim = 26 ** key_len  # 26 is the sice of alphabet
            population = Population(self._encrypted_text, self._force_train, POPULATION_SIZE, key_len)

            for key in frogress.bar(self._bruteforce_keys_generation(key_len), steps=space_dim):
                score = population.evaluate_fitness_of_member(key)
                self.logger.info("Key '{}' (score: {})".format(key, score))
                if score > threshold_score:
                    final_key = key
                    break

        final_plaintext = Vigenere(final_key).decipher(self._encrypted_text)

        self.logger.info("Final key: '{}'".format(final_key))
        self.logger.info("Final plaintext: '{}'".format(self._correct_plaintext(final_plaintext)))

    def run(self):
        """
        Main method for running the whole class handling GA and searching for
        decrypting of text encrypted by Vigenere cipher.
        """
        self.logger.info("Trying to decryption of text '{}' by genetic algorithms".format(self._encrypted_text))

        if self._key_len:
            possible_key_lens = [int(self._key_len)]

        else:
            possible_key_lens = self._get_possible_key_lens()
            if not possible_key_lens:
                self.logger.error("There is no possible key length or very short, application ends")
                return

        fittest_members = list()

        for try_num, key_len in enumerate(possible_key_lens):
            self.logger.info("Trying key length: {}".format(key_len))

            curr_fittest_mem_stats = self._run_evolution(key_len)
            fittest_members.append(curr_fittest_mem_stats)

            if curr_fittest_mem_stats["reached_threshold"]:
                break

        # Revert because in case that there are more values which are same, pick the last one.
        final_solution = max(fittest_members[::-1], key=lambda x: x["fitness_score"])
        self.logger.info("Final key: '{}'.".format(self._get_printable_solution(final_solution)))

        final_plaintext = Vigenere(final_solution["member"]).decipher(self._encrypted_text)

        self.logger.info("Final plaintext: '{}'".format(self._correct_plaintext(final_plaintext)))


def get_args():
    parser = ArgumentParser(
        description="Train and evaluate the performance of the usage of decrypting text with genetic algorithms."
                    "Text is encrypted by Vigenere cipher."
    )
    parser.add_argument("--input", help="Encrypted text or filename containing such text to guess", required=True)
    parser.add_argument("--key_len", help="Key of length, for smaller text samples mostly.")
    parser.add_argument(
        "--force_train", help="Force training Markov Model for determining fitness score.", action="store_true"
    )
    parser.add_argument(
        "--brute_force", help="Trying to decrypt by using brute-force method instead of genetic algorithms.",
        action="store_true"
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = get_args()
    decryptor = VigenereDecryptor(input_to_decrypt=args.input, key_len=args.key_len, force_train=args.force_train)
    decryptor.brute_force() if args.brute_force else decryptor.run()

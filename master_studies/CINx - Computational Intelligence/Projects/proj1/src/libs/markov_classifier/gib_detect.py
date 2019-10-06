#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-10-29
.. codeauthor:: Ladislav Šulák (laco.sulak@gmail.com)
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)

    Original version taken and inspirated from https://github.com/exp0se/dga_detector
"""
import enchant
import pickle
import sys
from os.path import abspath, dirname, getsize, isfile, join, pardir

sys.path.append(abspath(join(dirname(__file__), pardir)))
MARKOV_CLASSIFIER_DIR = dirname(__file__)

from libs.markov_classifier.gib_detect_train import train, avg_transition_prob


class GibberishDetector(object):
    """
    Class for determining the score which shows how the word belongs to English
    language (can be modified to more languages).
    """
    SAVED_GIBBERISH_MODEL = join(MARKOV_CLASSIFIER_DIR, "gib_model.pki")

    def __init__(self, force_training=False):
        """
        Entry point of this class which initializes model.

        :param bool force_training: if train new markov model classifier or use already trained one
        """
        if force_training or not isfile(self.SAVED_GIBBERISH_MODEL) or getsize(self.SAVED_GIBBERISH_MODEL) == 0:
            train()

        with open(self.SAVED_GIBBERISH_MODEL, mode="rb") as f:
            model_data = pickle.load(f)

            self._model_mat = model_data["mat"]

        self._en_dictionary = enchant.Dict("en_US")  # for more languages: enchant.list_languages()

    def word_language_score_level(self, word):
        """
        Method determines a score (probability) that word belongs to English language or not.
        Firstly it searches all substrings of word bigger than 3 characters in English dictionary and then
        it checks score with Markov Model Classifier. It returns always a biggest number.

        :param str word: word to be determined
        :return: final score/probability of word belonging to English language
        :rtype: float
        """
        word_length = len(word)

        if word_length > 3 and self._en_dictionary.check(word):
            return 3.0

        all_subwords = [word[x:y] for x in range(word_length+1) for y in range(word_length+1) if len(word[x:y]) > 3]

        all_lang_subword_scores = [
            len(sub_word) / word_length for sub_word in all_subwords if self._en_dictionary.check(sub_word)
        ]

        best_score = max(all_lang_subword_scores) if all_lang_subword_scores else 0.0

        language_score = avg_transition_prob(word, self._model_mat)
        return language_score if language_score > best_score else best_score  # bigger probability wins

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-12-17
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)
.. codeauthor:: Ladislav Šulák (laco.sulak@gmail.com)
"""
import cv2
import numpy as np

# Dataset - https://free-ebooks.gr/


class TextExtractor(object):
    def __init__(self, input_img_name):
        self._input_img = cv2.imread(input_img_name)
        self._tmp_curr_img = None

    def _to_grayscale(self):
        # self._tmp_curr_img = cv2.pyrUp(self._tmp_curr_img)  # upsample a given image (or pyrDown)
        self._tmp_curr_img = cv2.cvtColor(self._tmp_curr_img, cv2.COLOR_BGR2GRAY)

    def _morph_gradient(self, kernel_size):
        # prepare 3x3 (experimentation) morphological ellipse kernel and
        # perform morphological transphormation (gradient) on a given kernel
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, kernel_size)
        self._tmp_curr_img = cv2.morphologyEx(self._tmp_curr_img, cv2.MORPH_GRADIENT, kernel)

    def _morph_open(self, kernel_size):
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, kernel_size)
        self._tmp_curr_img = cv2.morphologyEx(self._tmp_curr_img, cv2.MORPH_OPEN, kernel)

    def _threshold(self):
        _, self._tmp_curr_img = cv2.threshold(self._tmp_curr_img, 0.0, 255.0, cv2.THRESH_BINARY | cv2.THRESH_OTSU)

    def _morph_close(self, kernel_size):
        # prepare 3x3 (experimentation) morphological rectangle kernel and
        # perform morphological transphormation (close) on a given kernel
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, kernel_size)
        self._tmp_curr_img = cv2.morphologyEx(self._tmp_curr_img, cv2.MORPH_CLOSE, kernel)

    def _find_text_from_img(self, original_img, processed_img, testing=False):
        text_segments = list()
        # using RETR_EXTERNAL instead of RETR_CCOMP
        _, contours, hierarchy = cv2.findContours(processed_img.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

        mask = np.zeros(processed_img.shape, dtype=np.uint8)
        print(f"Find {len(contours)} text items")
        for idx in range(len(contours)):
            x, y, w, h = cv2.boundingRect(contours[idx])
            mask[y:y + h, x:x + w] = 0
            cv2.drawContours(mask, contours, idx, (255, 255, 255), -1)
            r = float(cv2.countNonZero(mask[y:y + h, x:x + w])) / (w * h)

            if r > 0.45 and w > 9 and h > 9:
                if testing:
                    cv2.rectangle(original_img, (x, y), (x + w - 1, y + h - 1), (0, 255, 0), 2)
                else:
                    changed_image = original_img[y: y + h - 1, x:x + w - 1]
                    text_segments.append(changed_image[0:13, 0:250])

        if testing:
            cv2.imshow("final", original_img)
            cv2.waitKey(0)
        else:
            return text_segments

    def _show_curr_img(self):
        cv2.imshow("current_image", self._tmp_curr_img)
        cv2.waitKey(0)

    def _preprocess_text(self, img_to_process):
        self._tmp_curr_img = img_to_process
        self._to_grayscale()

        self._morph_gradient((3, 3))
        self._morph_open((3, 3))
        self._threshold()
        self._morph_close((20, 1))

        return self._tmp_curr_img

    def _preprocess_line(self, img_to_process):
        self._tmp_curr_img = img_to_process
        self._to_grayscale()

        self._morph_gradient((3, 3))
        self._morph_open((3, 3))
        self._threshold()
        self._morph_close((3, 10))

        return self._tmp_curr_img

    def extract(self):
        processed_text = self._preprocess_text(self._input_img)
        # self._find_lines_of_text(self._input_img, processed_text, testing=True)

        for line in self._find_text_from_img(self._input_img, processed_text):
            bigger_line = line
            # params
            inverted_colors = cv2.bitwise_not(bigger_line)
            cv2.imwrite("final.png", inverted_colors)
            return
            # zoom = 2 if len(bigger_line) < 15 else 1
            # for _ in range(zoom):  # todo range!!
            #    bigger_line = cv2.pyrUp(bigger_line)
            # processed_line = self._preprocess_line(bigger_line)
            # self._find_text_from_img(bigger_line, processed_line, testing=True)
            # return


if __name__ == "__main__":
    input_img = "exp1.jpg"
    TextExtractor(input_img).extract()

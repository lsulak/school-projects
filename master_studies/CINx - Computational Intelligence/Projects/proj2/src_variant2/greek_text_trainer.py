#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-12-17
.. codeauthor:: Kristian Benko (kristianbnk@gmail.com)
.. codeauthor:: Ladislav Šulák (laco.sulak@gmail.com)
"""
import os
import numpy as np
import string
import sys
from PIL import Image, ImageFont, ImageDraw
from skimage.io import imread
from sklearn.model_selection import ShuffleSplit
from TFANN.TFANN import ANNC

MAX_CHAR = 64


# TODO good - https://github.com/TJCVRS/CRNN_Tensorflow
def make_img(t, f, fn, offset, s=(100, 100), o=(16, 8)):
    """
    Generate an image of text
    t:      The text to display in the image
    f:      The font to use
    fn:     The file name
    s:      The image size
    o:      The offest of the text in the image
    """
    img = Image.new('RGB', s, "black")
    draw = ImageDraw.Draw(img)
    draw.text(offset, t, (255, 255, 255), font=f)
    img.save(fn)


def generate_dataset():
    # The possible characters to use
    CS = list(string.ascii_letters)
    RTS = list(np.random.randint(10, 64, size=8192)) + [64]
    # The random strings
    S = [''.join(np.random.choice(CS, i)) for i in RTS]
    # Get the font
    font = ImageFont.truetype("font.ttf", 16)
    # The largest size needed
    MS = max(font.getsize(Si) for Si in S)
    # Computed offset
    OFS = ((640 - MS[0]) // 2, (32 - MS[1]) // 2)
    # Image size
    MS = (640, 32)
    Y = []
    for i, Si in enumerate(S):
        make_img(Si, font, str(i) + '.png', OFS, MS, OFS)
        Y.append(str(i) + '.png,' + Si)
    # Write CSV file
    with open('Train.csv', 'w') as F:
        F.write('\n'.join(Y))


def load_data(FP='.'):
    """
    Loads the OCR dataset. A is matrix of images (NIMG, Height, Width, Channel).
    Y is matrix of characters (NIMG, MAX_CHAR)
    FP:     Path to OCR data folder
    return: Data Matrix, Target Matrix, Target Strings
    """
    TFP = os.path.join(FP, 'Train.csv')
    A, Y, T, FN = [], [], [], []
    with open(TFP) as F:
        for Li in F:
            FNi, Yi = Li.strip().split(',')  # filename,string
            T.append(Yi)
            A.append(imread(os.path.join(FP, 'Out', FNi)))
            Y.append(list(Yi) + [' '] * (MAX_CHAR - len(Yi)))  # Pad strings with spaces
            FN.append(FNi)
    return np.stack(A), np.stack(Y), np.stack(T), np.stack(FN)


def train():
    A, Y, T, FN = load_data("dataset")
    CS = list(string.ascii_letters)
    # Architecture of the neural network
    NC = len(string.ascii_letters + string.digits + ' ')
    IS = (32, 640, 3)       #Image size for CNN
    ws = [('C', [4, 4,  3, NC // 2], [1, 2, 2, 1]), ('AF', 'relu'),
          ('C', [4, 4, NC // 2, NC], [1, 2, 1, 1]), ('AF', 'relu'),
          ('C', [8, 5, NC, NC], [1, 8, 5, 1]), ('AF', 'relu'),
          ('R', [-1, 64, NC])]
    # Create the neural network in TensorFlow
    cnnc = ANNC(IS, ws, batchSize = 64, learnRate = 5e-5, maxIter = 32, reg = 1e-5, tol = 1e-2, verbose = True)
    # if not cnnc.RestoreModel('TFModel/', 'ocrnet'):
    # Fit the network
    cnnc.fit(A, Y)
    # The predictions as sequences of character indices
    YH = np.zeros((Y.shape[0], Y.shape[1]), dtype = np.int)
    for i in np.array_split(np.arange(A.shape[0]), 32):
        YH[i] = np.argmax(cnnc.predict(A[i]), axis = 2)
    # Convert from sequence of char indices to strings
    PS = [''.join(CS[j] for j in YHi) for YHi in YH]
    for PSi, Ti in zip(PS, T):
        print(Ti + '\t->\t' + PSi)


if __name__ == "__main__":
    # generate_dataset()
    train()

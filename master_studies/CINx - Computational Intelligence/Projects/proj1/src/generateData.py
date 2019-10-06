#!/usr/bin/python3

from pycipher import Vigenere

while True:
    program = input("encode or decode:")
    if program == "encode":
        key = input("key:")
        string = input("Thing to encode:")
        vigenere = Vigenere(key)
        encrypted = vigenere.encipher(string)
        print(encrypted)
    if program == "decode":
        key = input("key:")
        string = input("Thing to decode:")
        vigenere = Vigenere(key)
        decrypted = vigenere.decipher(string)
        print(decrypted)
    else:
        print("Unknown input, Please enter encode or decode")
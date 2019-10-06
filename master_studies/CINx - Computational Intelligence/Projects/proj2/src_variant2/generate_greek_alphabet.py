#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-12-07
.. codeauthor:: Ladislav Šulák (xsulak04@stud.fit.vutbr.cz)
"""
from os import listdir, makedirs
from os.path import exists, join
from PIL import Image, ImageDraw, ImageFont
from random import randint

DATASET_DIR = "font_dataset"

GREEK_CHARS = [
    "Α", "α", "Β", "β", "Γ", "γ", "Δ", "δ", "Ε", "ε", "Ζ", "ζ", "Η", "η", "Θ", "θ",
    "Ι", "ι", "Κ", "κ", "Λ", "λ", "Μ", "μ", "Ν", "ν", "Ξ", "ξ", "Ο", "ο", "Π", "π",
    "Ρ", "ρ", "Σ", "σ", "ς", "Τ", "τ", "Υ", "υ", "φ", "Φ", "Χ", "χ", "Ψ", "ψ", "Ω", "ω"
]
GREEK_RESOURCES_FONT_FILE = join(DATASET_DIR, "greek_font_resources")
GREEK_RESULTING_IMG_DATASET = join(DATASET_DIR, "greek_imgs_dataset")

GREEK_CHARS_EXT = [
    "ᾀ", "ᾁ", "ᾂ", "ᾃ", "ᾄ", "ᾅ", "ᾆ", "ᾇ", "ἀ", "ἁ", "ἂ", "ἃ", "ἄ", "ἅ", "ἆ", "ἇ",
    "ᾰ", "ᾱ", "ᾲ", "ᾳ", "ᾴ", "ᾶ", "ᾷ", "ὰ", "ά",
    "Ἀ", "Ἁ", "Ἂ", "Ἃ", "Ἄ", "Ἅ", "Ἆ", "Ἇ", "Ᾰ", "Ᾱ", "Ὰ", "Ά", "ᾼ", "ᾈ", "ᾉ", "ᾊ",
    "ᾋ", "ᾌ", "ᾍ", "ᾎ", "ᾏ",
    "ἐ", "ἑ", "ἒ", "ἓ", "ἔ", "ἕ", "ὲ", "έ", "Ἐ", "Ἑ", "Ἒ", "Ἓ", "Ἔ", "Ἕ", "Ὲ", "Έ",
    "ῂ", "ῃ", "ῄ", "ῆ", "ῇ", "ἠ", "ἡ", "ἢ", "ἣ", "ἤ", "ἥ", "ἦ", "ἧ", "ὴ", "ή", "ᾐ",
    "ᾑ", "ᾒ", "ᾓ", "ᾔ", "ᾕ", "ᾖ", "ᾗ",
    "Ἠ", "Ἡ", "Ἢ", "Ἣ", "Ἤ", "Ἥ", "Ἦ", "Ἧ", "Ὴ", "Ή", "ῌ", "ᾘ", "ᾙ", "ᾚ", "ᾛ", "ᾜ",
    "ᾝ", "ᾞ", "ᾟ",
    "ἰ", "ἱ", "ἲ", "ἳ", "ἴ", "ἵ", "ἶ", "ἷ", "ὶ", "ί", "ῐ", "ῑ", "ῒ", "ΐ", "ῖ", "ῗ",
    "Ἰ", "Ἱ", "Ἲ", "Ἳ", "Ἴ", "Ἵ", "Ἶ", "Ἷ", "Ῐ", "Ῑ", "Ὶ", "Ί",
    "ὀ", "ὁ", "ὂ", "ὃ", "ὄ", "ὅ", "ὸ", "ό", "Ὀ", "Ὁ", "Ὂ", "Ὃ", "Ὄ", "Ὅ", "Ὸ", "Ό",
    "ὐ", "ὑ", "ὔ", "ὕ", "ὖ", "ὗ", "ὺ", "ύ", "ῠ", "ῡ", "ῢ", "ΰ", "ῦ", "ῧ", "ὒ", "ὓ",
    "Ὑ", "Ὓ", "Ὕ", "Ὗ", "Ῠ", "Ῡ", "Ὺ", "Ύ",
    "ὠ", "ὡ", "ὢ", "ὣ", "ὤ", "ὥ", "ὦ", "ὧ", "ῲ", "ῳ", "ῴ", "ῶ", "ῷ", "ὼ", "ώ", "ᾠ",
    "ᾡ", "ᾢ", "ᾣ", "ᾤ", "ᾥ", "ᾦ", "ᾧ",
    "Ὠ", "Ὡ", "Ὢ", "Ὣ", "Ὤ", "Ὥ", "Ὦ", "Ὧ", "ᾨ", "ᾩ", "ᾪ", "ᾫ", "ᾬ", "ᾭ", "ᾮ", "ᾯ", "Ὼ", "Ώ", "ῼ",
    "ῤ", "ῥ", "Ῥ",
]
GREEK_EXT_RESOURCES_FONT_FILE = join(DATASET_DIR, "greek_ext_font_resources")
GREEK_EXT_RESULTING_IMG_DATASET = join(DATASET_DIR, "greek_ext_imgs_dataset")


def generate_characters():
    """
    This function draws a picture from characters and saves each into separate folder and file.
    Var 'final_dataset' contains (chars_list, font_dir, resulting_imgs_dir):
        char_list - characters for drawing
        font_dir - directory of all styles for drawing
        resulting_imgs_dir - directory of all final character pictures
    """
    for chars_list, font_dir, resulting_imgs_dir in final_dataset:
        for font_file in listdir(font_dir):
            font_file_path = join(font_dir, font_file)

            for character in chars_list:
                curr_img_dataset_dir = join(resulting_imgs_dir, character)
                if not exists(curr_img_dataset_dir):
                    makedirs(curr_img_dataset_dir)

                char_image = Image.new('L', (image_size, image_size), background_color)
                draw = ImageDraw.Draw(char_image)

                # Specify font : Resource file, font size
                font = ImageFont.truetype(font_file_path, font_size)
                font_width, font_height = font.getsize(character)

                x = (image_size - font_width) / 2
                y = (image_size - font_height) / 2

                draw.text((x, y), character, (245 - background_color) + randint(0, 10), font=font)

                file_name = f"{curr_img_dataset_dir}/{font_file[:-4]}.png"
                char_image.save(file_name)


def extract_all_ttf_from_all_zips_in_dir():
    """
    Extraction of all '.ttf' files from all ZIP files from directory 'zips_to_extract'.
    Resulting ttf files are saved into 'dir_for_extracted'
    """
    import zipfile
    import re
    i = 1

    zips_to_extract = f"{DATASET_DIR}/zips_to_extract"
    dir_for_extracted = f"{DATASET_DIR}/extracted"
    if not exists(dir_for_extracted):
        makedirs(dir_for_extracted)

    for font_file in listdir(zips_to_extract):
        if font_file.endswith(".zip"):
            try:
                with zipfile.ZipFile(join(DATASET_DIR, font_file), "r") as zip_ref:
                    for info in zip_ref.infolist():
                        if re.match(r'.*\.(ttf|TTF)$', info.filename):
                            zip_ref.extract(info, path=dir_for_extracted)

            except Exception as e:
                print(f"File '{font_file}' not supported, continuing [{e}]")
                i += 1
    print(f"Not processed zip files: {i}")

if __name__ == "__main__":

    final_dataset = (
        (GREEK_CHARS, GREEK_RESOURCES_FONT_FILE, GREEK_RESULTING_IMG_DATASET),
        (GREEK_CHARS_EXT, GREEK_EXT_RESOURCES_FONT_FILE, GREEK_EXT_RESULTING_IMG_DATASET),
    )

    background_color = 255
    font_size = 30
    image_size = 45

    # extract_all_ttf_from_all_zips_in_dir()
    generate_characters()

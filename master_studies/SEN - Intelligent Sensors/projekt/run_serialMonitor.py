#!/usr/bin/python3

"""
  Project to subject SEN 2016/2017
  Author:  Ladislav Sulak  xsulak04@stud.fit.vutbr.cz
  File:    run_serialMonitor.py

  Description:   
    Collecting data from accelerometer(s) for determining the position of quadrocopter.
    Calculating changes in directionand speed.

    ####

    Sbírání dat z akcelerometru(ů), pro určení pozice kvadrokoptéry. 
    Pocitanie zmeny polohy a rychlosti.

  This script will output data from serial port into file with some additional 
  logging information (like time). Data are obtained from Arduino UNO (with accelerometer ADXL345).
  Requires no input arguments and for ending the process you have to press CTRL+C.

"""

import logging
import os
import serial
import sys

from serial.serialutil import SerialException

BAUDRATE = 9600
LINE_LEN = 80

SERIAL_PORT = "/dev/ttyACM0"
OUTPUT_FILE = "acceleratorData.log"

def setLogger():
  logger = logging.getLogger()
  handler = logging.FileHandler(OUTPUT_FILE, 'w')
  formatter = logging.Formatter("[%(asctime)s] %(message)s",  "%Y-%m-%d %H:%M:%S")

  handler.setFormatter(formatter)
  logger.addHandler(handler)
  logger.setLevel(logging.INFO)

def monitor(logger):
  try:
    serialPort = serial.Serial(SERIAL_PORT, timeout=None, baudrate=BAUDRATE)

  except SerialException:
    logging.error("Communication port is not open, receiving no data from Arduino.")
    sys.exit(1)

  logging.info("================ Serial Monitor Starts ================")

  try:
    while (1):
      output_line = str()
      char_read = serialPort.read()
      try:
        char_read = str(char_read.decode('utf-8'))
      # If you will run this script shortly after loading the main program
      # to Arduino, there can be trash, so just skip that character, cannot be decoded.
      except Exception: 
        continue

      if(char_read == '#'):  # start of line

        # For each line
        while(1):
          char_read = serialPort.read()
          char_read = str(char_read.decode('utf-8'))
          if(char_read == '\n'):
            break

          output_line += char_read

        logging.info(output_line)


  except KeyboardInterrupt:
    logging.info("================ Serial Monitor Ends ================")
    
  except Exception as e:
    logging.error("Unexpected Exception: {}.".format(str(e)))
    sys.exit(1)

if __name__ == "__main__":
  logger = setLogger()
  monitor(logger)

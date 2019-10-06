#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
.. Created on 2017-10-28
.. codeauthor:: Ladislav Šulák (xsulak04@stud.fit.vutbr.cz)
"""
import logging
import sys
from abc import ABCMeta, abstractmethod
from os.path import abspath, join, dirname, pardir
sys.path.append(abspath(join(dirname(__file__), pardir, pardir, pardir)))


class BaseRunner(metaclass=ABCMeta):
    @abstractmethod
    def __init__(self, log_options=None):
        self.__log_opt = log_options

    @property
    def logger(self):
        logger = logging.getLogger(self.__class__.__name__)

        if self.__log_opt:
            logging.basicConfig(**self.__log_opt)
        else:
            format = "%(asctime)s [%(name)s|%(filename)s] %(levelname)s: %(message)s"
            date_format = "%Y-%m-%d %H:%M:%S"

            logging.basicConfig(level=logging.INFO, format=format, datefmt=date_format)
        return logger

    @abstractmethod
    def main(self):
        pass

    @abstractmethod
    def cleanup(self):
        pass

    def run(self):
        try:
            self.main()
        except KeyboardInterrupt:
            self.logger.info("Interrupted by keyboard")
        except Exception:
            self.logger.critical("Unexpected exception", exc_info=True)
            sys.exit(1)
        finally:
            self.cleanup()

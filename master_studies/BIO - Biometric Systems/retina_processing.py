#!/usr/bin/python3

"""
  Project to subject BIO 2016/2017
  Author:  Ladislav Sulak  xsulak04@stud.fit.vutbr.cz
  File:    retina_processing.py

  Description:   
    The aim is to detect a specific retinopathy - blood clots and also to denote them graphically.
    Focus on hemorrhaging and aneurysm.

    ####

    Ve vybraných snímcích sítnice oka detekujte krevní srazeniny a graficky ich označte. 
    Zaměřte se zejména na hemorágie a anaeurizmy.


    MESSIDOR database:
		* Section Medical diagnoses and Download:
			http://www.adcis.net/en/Download-Third-Party/Messidor.html 
		
		* About - https://www.ias-iss.org/ojs/IAS/article/viewFile/1155/959
	    
	    * There are 2 types of diagnoses in this database: 
	    	# Retinopathy grade - in DB there are stages 2 and 3 which are interesting for us.
	    	It's according to Keith Wagener Barker (KWB) Grades,
	    	(which is there 3 and 4 but it's the same) hemorraging is on these stages:
	    		http://ophthaclassification.altervista.org/hypertensive-retinopathy-classification-keith-wagener-barker

	    	# Macular edema - not important in our work (for hemorrages absence):
	    		http://www.ophthobook.com/chapters/retina
"""
import csv
import glob
import logging
import os
import sys

import cv2
import numpy
import xlrd


class DBextractor():
	"""
		The purpose of this class is to find all filenames of retinal images in database
		and extract information about theirs hemorraging presence.

		It is necessary to have following directory structure:
		DB/
			database/ - can have arbitrary name and there can be more of them
				database_subcategory/ - the same as above. For example various years or subjects etc.
					imgs - various retinal pictures, can have various formats and names,
						   but these names MUST be in following document with its medical data 
					.xls - document containing medical diagnosis data about these retinals from pictures 

		Information in xls document MUST be in following way:
			Image name | Department | Retinopathy grade | Risk of Macular Edema
			but only First and third are used by our work. Third is a number from 0 - 3.

		Resulting DB has the same directory structure as above (just with prefix 'Result'), 
		but MUST NOT exist already - it would broke the resulting CSV files with 
		results of hemorraging. (TODO?) 
	"""

	DB_STATS_FILE_FORMAT = ".xls"
	DB_FOLDER = "DB"
	IMAGE_NAME_INDEX = 0
	RETINOPATHY_GRADE_INDEX = 2

	def __init__(self):
		self._files = [f for f in glob.iglob(self.DB_FOLDER + "/**/*" + self.DB_STATS_FILE_FORMAT, 
											 recursive=True)]

	def run(self):
		for file in self._files:
			database = file[:file.rfind('/')]

			book = xlrd.open_workbook(file)
			sh = book.sheet_by_index(0)

			for currRow in range(1, sh.nrows):
				hasHemorrage = True if int(sh.row(currRow)[self.RETINOPATHY_GRADE_INDEX].value) >= 2 else False
				yield (database + "/" + str(sh.row(currRow)[self.IMAGE_NAME_INDEX].value), hasHemorrage)


class RetinaProcessor():
	"""
		This class receives a list with filenames with information about
		presence of hemorrages in them. It processes every image with OpenCV, based 
		on thresholding and simple blob finding. Based on this it detects hemorrages and
		label them on image which is then saved to the same directory structure as from the input
		with the information (statistical file) about numbers of hemorrages in every
		picture as CSV file. 
	"""

	# Thresholding parameters.
	DEBUG_SHOW_PICTURES = False
	BLACK_COLOR = 255
	RETINA_ONLY_THRESHOLD = 30
	HEMORRAGES_VEINS_THRESHOLD = 69

	# Blob detection parameters.
	MIN_CIRCULARITY = 0.25
	MIN_CONVEXITY = 0.65
	MIN_INERTIA_RATIO = 0.15
	MIN_AREA = 100
	MAX_AREA = 99999

	# Output stats file, every DB has it in each subfolder.
	RESULT_DB_STATS_FILE = "hemorrageStats.csv"

	def __init__(self, dataset):
		self._dataset = dataset
		self._imgCorrectDetect = 0
		self._imgAll = 0

	def _thresholding(self, img):
		gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

		_, bg_mask = cv2.threshold(gray_image, 
								   self.RETINA_ONLY_THRESHOLD,
								   self.BLACK_COLOR,
								   cv2.THRESH_BINARY_INV)
		
		# Keep only retina, background will be white as every not relevant parts in retina.
		retina_only = cv2.bitwise_xor(bg_mask, gray_image)

		# Try to threshold only hemorrages, but there are also blood veins 
		# because of similar 'color' signature.
		_, hemorrages_veins = cv2.threshold(retina_only,
											self.HEMORRAGES_VEINS_THRESHOLD,
											self.BLACK_COLOR,
											cv2.THRESH_BINARY)
		
		# Areas used in the methods below.
		kernel2x2 = numpy.ones((2,2), numpy.uint8)
		kernel5x5 = numpy.ones((5,5), numpy.uint8)

		# Noise removal - try to remove veins and keep hemorrages. Morphological methods.
		# Dilate, opening and closing - remove noise, try to group near objects, fill/flood area ...
		dilation = cv2.dilate(hemorrages_veins, kernel2x2, iterations=1)
		morph_close = cv2.morphologyEx(dilation, cv2.MORPH_CLOSE, kernel2x2, iterations=3)
		morph_open = cv2.morphologyEx(morph_close, cv2.MORPH_OPEN, kernel5x5, iterations=3)

		if self.DEBUG_SHOW_PICTURES:
			cv2.imshow("Grey image", gray_image)
			cv2.waitKey(0)
			cv2.imwrite("grayImage.png", gray_image)

			cv2.imshow("Background mask", bg_mask)
			cv2.waitKey(0)
			cv2.imwrite("bgMask.png", bg_mask)

			cv2.imshow("Only retina", retina_only)
			cv2.waitKey(0)
			cv2.imwrite("retinaOnly.png", retina_only)

			cv2.imshow("Hemorrages and veins", hemorrages_veins)
			cv2.waitKey(0)
			cv2.imwrite("hemorragesVeins.png", hemorrages_veins)

			cv2.imshow("Dilation", dilation)
			cv2.waitKey(0)
			cv2.imwrite("dilation.png", dilation)

			cv2.imshow("Morphological close", morph_close)
			cv2.waitKey(0)
			cv2.imwrite("morphClose.png", morph_close)
			
			cv2.imshow("Morphological open", morph_open)
			cv2.waitKey(0)
			cv2.imwrite("morphOpen.png", morph_open)

		return morph_open

	def _detectBlobs(self, img, thresholdedImg):

		# Setup SimpleBlobDetector parameters.
		params = cv2.SimpleBlobDetector_Params()

		# Filter by Circularity - they are actually circular, because the blood is spilled.
		params.filterByCircularity = True
		params.minCircularity = self.MIN_CIRCULARITY

		# Filter by Convexity - they have to have some enclosed shape, because the blood is spilled.
		params.filterByConvexity = True 
		params.minConvexity = self.MIN_CONVEXITY

		# Filter by Inertia - not totally endwise, they are not like veins.
		params.filterByInertia = True
		params.minInertiaRatio = self.MIN_INERTIA_RATIO

		# Filter by Area - not very very small - the rests of veins are.
		params.filterByArea = True
		params.minArea = self.MIN_AREA 
		params.maxArea = self.MAX_AREA

		blobDetector = cv2.SimpleBlobDetector_create(params)
		keypoints = blobDetector.detect(thresholdedImg)

		img_with_keypoints = cv2.drawKeypoints(img, 
											   keypoints, 
											   numpy.array([]), 
											   (255,255,255), 
											   cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
		if self.DEBUG_SHOW_PICTURES:
			cv2.imshow("Image with keypoints", img_with_keypoints)
			cv2.imwrite("imgWithKeypoints.png", img_with_keypoints)
			cv2.waitKey(0)

		return img_with_keypoints, len(keypoints)

	def _writeImgResData(self, imageNamePath, resultImg, hemorragesAmount):
		""" 
			Method for writing data about resulting detection of every
			hemorrage in every image and also writing the image to filesystem.
			Resulting DB has the same directory structure as the input one,
			but with the prefix 'Result'. There CANNOT exist resulting DB already,
			because it would broke the existing data about resulting images.
		"""
		tmpIndex = imageNamePath.rfind('/')
		database = imageNamePath[:tmpIndex]
		resultDatabase = "Result" + database
		image = imageNamePath[tmpIndex+1:]

		os.makedirs(resultDatabase, exist_ok=True)
		statFileExists = os.path.isfile(resultDatabase + "/" + self.RESULT_DB_STATS_FILE)

		with open(resultDatabase + "/" + self.RESULT_DB_STATS_FILE, 'a') as csvStatsFile:
			csvWriter = csv.writer(csvStatsFile, delimiter=';')

			if not statFileExists:
				csvWriter.writerow(["Database", "Image", "Hemorrages Amount"])
			csvWriter.writerow([database, image, hemorragesAmount])

		cv2.imwrite(resultDatabase + "/" + image, resultImg)

	def run(self):
		print("###### RETINA PROCESSING HAS STARTED ######")
		for record in self._dataset:
			imageNamePath, hasHemorrage = record
			img = cv2.imread(imageNamePath)

			print("Processing: " + imageNamePath)

			thresholdedImg = self._thresholding(img)
			resultImg, hemorragesAmount = self._detectBlobs(img, thresholdedImg)

			self._writeImgResData(imageNamePath, resultImg, hemorragesAmount)
			self._imgAll += 1

			if hemorragesAmount > 0 and hasHemorrage:
				self._imgCorrectDetect += 1
			elif hemorragesAmount == 0 and not hasHemorrage:
				self._imgCorrectDetect += 1

		precision = 100 * float(self._imgCorrectDetect) / float(self._imgAll)
		print("###### FINAL PRECISION RESULT: {}% ######".format(str(precision)))

if __name__ == "__main__":
	dataset = DBextractor().run()
	RetinaProcessor(dataset).run() 
	
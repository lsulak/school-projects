#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Title:			Project for subject BIF (Bioinformatics), FIT VUT in Brno:
				The analysis of human genome GRCh38

Author: 		Ladislav Sulak,  xsulak04@stud.fit.vutbr.cz
Data: 			4/2016

Description:  	Tool for extraction of human genome data for processing information about genes, transcripts and coding segments.
				Mainly statistics about their occurence which means their count and sizes.
				The input is provided by a GTF file and the output is generated as a CSV file.
"""

import os
import sys
import csv
import string

""" 
	This script runs ~35 seconds on i5 processor, JFYI.
	Just good to know:
		* Genes --> Transcripts (different variants of gene, there is transcription including various variants for alternative splicing) ->> Exons

		* If coding genes (biotype = "protein_coding", "IG_*", "TR_*"), then there can be transcripts with CDS (coding segments), UTR (Untranslated region)...

	We will parse:
		* Genes (various biotypes - coding, small/long non coding and pseudo genes),
		* Transcripts and exons from coding genes - protein coding genes (transcripts/CDS with biotype = "protein_coding")

	On these items we will calculate the amount of genes, amount of all bases in all chromozome (all genome) and gene coverage (all bases in all chroms/length of all chroms 1-22, X, Y)

	Another good info: https://gencodegenes.wordpress.com/toolbox/ and http://www.gencodegenes.org/data_format.html
"""


"""	Output CSV file containing some statistical information about biotypes """
OUTPUT = "output.csv"

"""	Indexes to GTF line """
CHROM		= 0 # Name/Number of the chromosome or scaffold
FEATURE		= 2 # Gene, Variation, Transcript, CDS (Coding Segment), Exon...
START		= 3 # Start position of the feature
STOP		= 4 # Stop position of the feature
ATTRIBUTE	= 8 # Semicolon-separated list of tag-value pairs containing some additional information,
				# like gene_biotype

"""	Indexes to set of all biotypes. This index will always represend one of four categories of biotype. """
CODING_GENES			= 0
SMALL_NON_CODING_GENES	= 1
LONG_NON_CODING_GENES	= 2
PSEUDO_GENES			= 3
PROTEIN_CODING_GENES	= 4

""" Each category of biotype [f.e. coding genes] is divided into a few sub-categories [f.e. protein_coding]. """
BIOTYPE_CATEG  	= 0
GENE_CNT 		= 1
BASE_CNT 		= 2
GENE_COV 		= 3

"""	Lengths of all chromozomes (1-22, X, Y), from www.ensembl.org """
CHROM_LENS = [ 248956422, 242193529, 198295559, 190214555, 181538259, 170805979, 159345973, 145138636, 
138394717, 133797422, 135086622, 133275309, 114364328, 107043718, 101991189, 90338345, 83257441, 80373285, 
58617616, 64444167, 46709983, 50818468, 156040895, 57227415 ]


class GRangeList:
	"""
		Class which will serve as a storage to range of bases [chromozome, start, stop] from genes, transcripts or CDS.
		This class well have such list and with a method reduce we can determine the amount of bases in each gene. 
	"""

	def __init__(self):
		self.basesRanges = []	# Storing all ranges
		self.allBases = 0		# Sum of all bases in all chromozomes
		self.basesInChromozomes = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]	  # Sum of bases in gene, each item in list represent each chromozome 


	def reduce(self):
		""" Sum of all bases. There can be overlapping, and have to take it into account. """

		lastBaseEnd = 0
		lastChromozome = 0
		currChromIndex = 0

		sortedList = sorted(self.basesRanges, key=lambda x: (x[0], x[1]))

		for bases in sortedList:

			# Chromozomes are different, no overlapping is possible
			# Between 10 and 20 there is 11 bases, see http://gmod.org/wiki/GFF2, Identifying the reference sequence 
			if(lastChromozome != bases[0]):
				lastChromozome = bases[0]
				currChromIndex = getChromIndex(lastChromozome)
				self.basesInChromozomes[currChromIndex] +=  bases[2] - bases[1] + 1

			# Two genes/transcripts/CDS in one chromozome, overlapping is possible
			else:
				currChromIndex = getChromIndex(lastChromozome)

				# They are almost separated - they have just 1 common base
				if(lastBaseEnd == bases[1]):
					self.basesInChromozomes[currChromIndex] +=  bases[2] - bases[1]

				# If they are overlapped
				elif(lastBaseEnd > bases[1]): 

					# Part of new one is partialy inside of previous one	
					if(bases[2] > lastBaseEnd):
						self.basesInChromozomes[currChromIndex] += bases[2] - lastBaseEnd

					# The new one is completely inside the previous one
					else:
						continue

				# They are completely separated
				else:
					self.basesInChromozomes[currChromIndex] +=  bases[2] - bases[1] + 1

			#print(self.basesInChromozomes[currChromIndex])
			lastBaseEnd 	= bases[2]
			lastChromozome 	= bases[0]

		self.allBases = sum(self.basesInChromozomes)	# Sum of all bases for the current gene/transcript/CDS


class Biotypes:
	"""
		Class for saving all necessary data about specific biotype.
	"""

	def __init__(self):
		self.category = {
			None: # Key is name of the Biotype
				[
					None, # Category - Coding, Small/Long Non-coding, Pseudo, Protein coding genes
					None, # The amount of genes in the whole genome
					None, # The amount of bases which constitutes this gene
					None  # Gene coverage - a ratio of a number of bases in the gene and 
						  # bases in a chromozome in which they are located 
				]
		}
		self.summary = [
		# Items represends the amount of genes, bases and gene coverage
			[0, 0, 0], # For coding genes
			[0, 0, 0], # For small non-cod. genes
			[0, 0, 0], # For long non-cod. genes
			[0, 0, 0], # For pseudo genes
			[0, 0, 0]  # For protein coding genes
		]


	def initBiotypeStruct(self):
		"""
			This will just initialize a dictionary
			Information about biotypes names taken from:
				http://www.gencodegenes.org/gencode_biotypes.html

			and also from here:
				ftp://ftp.sanger.ac.uk/pub/gencode/_README_stats.txt

			For all biotypes there is 1 dictionary:
				* key is biotype name,
				* value is a list:
					- the first item is a flag indicating a category of biotype

					- the second item is the amount of genes in the whole human genome,

					- the third item is an object GRangeList, which will 
						contain a list of all bases (start, base) on current chromozome

					- the fourth idem is a ratio of geneCoverage, calculated as 
						the amount of all bases / length of all chomosomes
		"""

		self.category = {

			#############################
			#	!!! CODING_GENES !!!	#
			#############################

			# Contains an open reading frame (ORF)
			"protein_coding": [CODING_GENES, 0, GRangeList(), 0.0],

			# Immunoglobulin (Ig) variable chain genes
			"IG_C_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],
			"IG_D_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],
			"IG_J_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			#"IG_LV_gene"	: [CODING_GENES, 0, GRangeList(), 0.0],

			"IG_V_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],

			# T-cell receptor genes
			"TR_C_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],
			"TR_D_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],
			"TR_J_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],
			"TR_V_gene" 	: [CODING_GENES, 0, GRangeList(), 0.0],


			#####################################
			#	!!! SMALL_NON_CODING_GENES !!!	#
			#####################################

			# Non-coding RNA predicted using sequences

			# Do not proceed - specification of this project
			#"Mt_rRNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			#"Mt_tRNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],

			"miRNA"			: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			"misc_RNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			"rRNA"			: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			#"scRNA"	 		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],

			"snRNA"	 		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			"snoRNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			#"ribozyme"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			#"sRNA"			: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],
			#"scaRNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			# Short non coding RNA gene that forms part of the vault ribonucleoprotein complex
			#"vaultRNA"		: [SMALL_NON_CODING_GENES, 0, GRangeList(), 0.0],


			#####################################
			#	!!! LONG_NON_CODING_GENES !!!	#
			#####################################

			# Long, intervening noncoding (linc) RNA
			"lincRNA"						: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0], 

			# Transcript which is known from the literature to not be protein coding
			"non_coding"					: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Doesn't contain an ORF
			"processed_transcript"			: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Has transcripts that overlap the genomic span (i.e. exon or introns) 
			# of a protein-coding locus on the opposite strand
			"antisense"						: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Transcript where ditag and/or published experimental data strongly supports
			# the existence of short non-coding transcripts transcribed from the 3'UTR
			"3prime_overlapping_ncrna"		: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Long non-coding transcript in introns of a coding gene that does not overlap any exons
			"sense_intronic"				: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Long non-coding transcript that contains a coding gene in its intron on the same strand
			"sense_overlapping"				: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# ?? There is no entry for this item, and also no explanation there http://www.gencodegenes.org/gencode_biotypes.html
			# But is mentioned in the specification of this project
			"known_ncrna"					: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			# To be Experimentally Confirmed. This is used for non-spliced EST clusters that have polyA features.
			#"TEC"							: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			# http://bmcgenomics.biomedcentral.com/articles/10.1186/1471-2164-15-35
			#"bidirectional_promoter_lncrna" : [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			# Unspliced lncRNA 
			#"macro_lncRNA"					: [LONG_NON_CODING_GENES, 0, GRangeList(), 0.0],


			#############################
			#	!!! PSEUDO_GENES !!!	#
			#############################

			# Have homology to proteins but generally suffer from a disrupted coding sequence
			"pseudogene" 							: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			"processed_pseudogene"					: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"polymorphic_pseudogene"				: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			#"retrotransposed"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			"transcribed_processed_pseudogene"		: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"transcribed_unprocessed_pseudogene"	: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"transcribed_unitary_pseudogene"		: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"translated_unprocessed_pseudogene"		: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			# From specification, but do not exist on http://www.gencodegenes.org/gencode_biotypes.html
			"translated_processed_pseudogene"		: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			"unitary_pseudogene"					: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			# Pseudogene that can contain introns since produced by gene duplication. 
			"unprocessed_pseudogene"				: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			# Inactivated immunoglobulin gene.

			# Do not proceed - specification of this project
			#"IG_pseudogene"							: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			"IG_C_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"IG_J_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"IG_V_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"TR_V_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			"TR_J_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],

			# Do not proceed - specification of this project
			# Non-coding RNA predicted to be pseudogene
			#"Mt_tRNA_pseudogene"					: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"tRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"snoRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"snRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"scRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"rRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"misc_RNA_pseudogene"					: [PSEUDO_GENES, 0, GRangeList(), 0.0],
			#"miRNA_pseudogene"						: [PSEUDO_GENES, 0, GRangeList(), 0.0],


			#####################################
			#	 !!! PROTEIN_CODING_GENES !!!	#
			#####################################

			"Coding transcripts"					: [PROTEIN_CODING_GENES, 0, GRangeList(), 0.0],
			"CDS"									: [PROTEIN_CODING_GENES, 0, GRangeList(), 0.0]
		}


class BiotypesExtractor:
	"""
		Class for processing the input GTF file.
		It saves all necessary values to the dictionary 'biotypes'
		and also can prepare everything for the outputting.
	"""

	def __init__(self, inputFile):
		self.inputFile = inputFile
		self.biotypes = Biotypes()

	def fillBiotypeStructs(self):
		""" 
			This method processes the input file and after that will calculate
			the amount of bases, gene coverage and make a summary for each category of biotype. 
		"""
		try:
			self.biotypes.initBiotypeStruct()

			with open(self.inputFile, 'r') as fp:
				fileGFT = csv.reader(fp, delimiter = '\t')

				for line in fileGFT:
					if((len(line[CHROM]) in range (1, 3)) and line[CHROM] != "MT" and (line[CHROM] == "X" or line[CHROM] == "Y" or (int(line[CHROM]) in range(1, 23)) )): # X, Y, 1-22 has only 1 or 2 chars
						biotypeName = None

						# Calculate everything except the last 2 statistics - protein coding genes
						if(line[FEATURE] == "gene"):
							for item in line[ATTRIBUTE].split(';'):
								if("gene_biotype" in item):
									biotypeName = item[15:-1] # We are looking for someName, -->gene_biotype "someName"<--
									break
						
						# Protein coding genes, Coding transcripts.
						elif(line[FEATURE] == "transcript"):
							for item in line[ATTRIBUTE].split(';'):
								if("gene_biotype \"protein_coding\"" in item):
									biotypeName = "Coding transcripts"
									break

						#Protein coding genes, Coding segments - CDS.
						elif(line[FEATURE] == "CDS"):
							for item in line[ATTRIBUTE].split(';'):
								if("gene_biotype \"protein_coding\"" in item):
									biotypeName = "CDS"
									break

						# There are also following features: exon, start_codon, stop_codon, five_prime_utr, three_prime_utr, Selenocysteine. 
						# Not interesting in our statistics, skip them.
						else:
							continue

						if biotypeName == None:
							continue

						#print (line[CHROM] + " " + line[ATTRIBUTE] + "\n")
						chrom = line[CHROM]
						start = int(line[START])
						end   = int(line[STOP])
						geneRangeBP = [chrom, start, end]
						
						# We will skip some biotypes, the specification of this project is (over)simplified :-)
						if(biotypeName not in self.biotypes.category):
							continue
						
						self.biotypes.category[biotypeName][GENE_CNT] += 1								 # Increment number of genes
						self.biotypes.category[biotypeName][BASE_CNT].basesRanges.append(geneRangeBP)	 # Add another bases ranges

			for biotypeName in self.biotypes.category:
				#if self.biotypes.category[biotypeName][GENE_CNT] > 0:
				#	print("\n"+biotypeName)
				self.biotypes.category[biotypeName][BASE_CNT].reduce()	# Calculate the amount of bases
				self.__computeGeneCoverage(biotypeName)					# Calculate gene coverage in [%]
				self.__computeSummary(biotypeName)						# For each category calculate summary

		# OS or IO exception, if there is a problem with the input file.
		except (OSError, IOError):
			print("Error during opening or closing output file!")
			sys.exit(1)

		# General exception, if is not unexpected,
		# relevant message is printed
		except Exception as e:
			try:
				sys.exit(int(str(e)))

			except ValueError as msg:
				print("Unexpected error: {}" .format(msg))
				sys.exit(1)


	def __computeGeneCoverage(self, biotype):
		"""	Coverage is computed as a ratio of the sum of all bases in gene/transcript/CDS and length of all chromozomes. """

		# If there was at least one record in the input file
		if(len(self.biotypes.category[biotype][BASE_CNT].basesRanges) > 0):
			self.biotypes.category[biotype][GENE_COV] = (self.biotypes.category[biotype][BASE_CNT].allBases / sum(CHROM_LENS)) * 100


	def __computeSummary(self, biotype):
		""" Method creates a summary for each category of biotypes.	"""

		# If there was at least one record in the input file
		if(len(self.biotypes.category[biotype][BASE_CNT].basesRanges) > 0):
			indexToBiotype = self.biotypes.category[biotype][BIOTYPE_CATEG]

			self.biotypes.summary[indexToBiotype][0] += self.biotypes.category[biotype][GENE_CNT]
			self.biotypes.summary[indexToBiotype][1] += self.biotypes.category[biotype][BASE_CNT].allBases
			self.biotypes.summary[indexToBiotype][2] += self.biotypes.category[biotype][GENE_COV]
			

class BiotypesOutputter:
	"""
		Class works with fully prepared dictionary containing all information needed for outputting.
	"""
	def __init__(self, biotypes):
		self.biotypesData = biotypes.category
		self.summary = biotypes.summary


	def outputAsCSV(self):
		""" The output file is CSV. This method processes the dictionary which is already filled. """
		try:
			with open(OUTPUT, 'w') as fp:
				fileCSV = csv.writer(fp, delimiter = ';')

				for bType in range (0, 5): # Each category of biotypes is printed one by one
					codingHeader = False
					isEmpty = True

					if bType == CODING_GENES:
						bioTypeCategory = "Coding genes"

					elif bType == SMALL_NON_CODING_GENES:
						bioTypeCategory = "Small non-coding genes"

					elif bType == LONG_NON_CODING_GENES:
						bioTypeCategory = "Long non-coding genes"

					elif bType == PSEUDO_GENES:
						bioTypeCategory = "Pseudo genes"
					
					elif bType == PROTEIN_CODING_GENES:
						bioTypeCategory = "Protein Coding Genes"

					for name in self.biotypesData:
						
						if(self.biotypesData[name][BIOTYPE_CATEG] == bType): # Print only biotypes of a current category
							
							# Also print genes without a record, but do not make summary for such items - their values are empty
							if(self.biotypesData[name][GENE_CNT] == 0):
								count = 0
								sizeBp = 0
								geneCoverage = 0

							else:
								isEmpty = False # Flag, at least one record is there
								count = self.biotypesData[name][GENE_CNT]
								sizeBp = self.biotypesData[name][BASE_CNT].allBases
								geneCoverage = self.biotypesData[name][GENE_COV]

							if(codingHeader == False): # First record in category - need to add header
								fileCSV.writerow([bioTypeCategory, "Count", "Size [bp]", "G.cov. [%]"])
								codingHeader = True    # Only 1 header at each category

							fileCSV.writerow([name, count, sizeBp, geneCoverage]) # Write data about specific biotype

					# Summary in the end, but only if there was some genes. For the last category there is no need for sum.
					if not isEmpty and bType != PROTEIN_CODING_GENES:
						fileCSV.writerow([
							"Summary",
							self.summary[bType][0], # Sum of genes count
							self.summary[bType][1], # Sum of all bases amount
							self.summary[bType][2], # Sum of all gene coverage ratios
							])

						if bType != PROTEIN_CODING_GENES: # Do not write empty line after the last category
							fileCSV.writerow([])

		except (OSError, IOError):
			print("\nError during opening or closing output file!")
			sys.exit(1)

		# General exception, printed with relevant message.
		except Exception as e:
			try:
				sys.exit(int(str(e)))

			except ValueError as msg:
				print("Unexpected error: {}" .format(msg))
				sys.exit(1)


def getChromIndex(chromozome):
	""" 
		Method simply converts the input string (1-22, X or Y) to decimal number, 
		chroms X and Y are the last ones - according to the list CHROM_LENS.
	"""
	if(chromozome == "X"):
		return 22

	elif(chromozome == "Y"):
		return 23

	else:
		return int(chromozome) - 1 # Items are indexed from 0 in the list of chromozomes


def main(argv):
	""" 
		Script for analyzing and processing data from the input GFT file,
		which contains a reference human genome. As a result there will be a CSV file
		as a representation of all biotypes of genes in chromozomes 1-22, X, Y.
	"""

	if(len(argv) != 1):
		print("Please provide only 1 argument for opening the input GFT file.")
		sys.exit(1)

	biotypesExtractor = BiotypesExtractor(argv[0])
	biotypesExtractor.fillBiotypeStructs()

	biotypesOutputter = BiotypesOutputter(biotypesExtractor.biotypes)
	biotypesOutputter.outputAsCSV()


if __name__ == "__main__":
	main(sys.argv[1:])

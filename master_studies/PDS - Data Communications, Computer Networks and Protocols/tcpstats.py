#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
 Name of the project		TcpStats
 Author						Ladislav Sulak
 Login 						xsulak04
 Date						4/2016
 File 						tcpstats.py
 Description				Project in Python for subject PDS (Data Communications, Computer Networks and Protocols)
							The aim of this project is to display some statistical data from the input provided
							from pcap-like file.

The solution operates in these steps:
		* firstly, there is a script written in Python. It requires 1 argument, 
		which is the name of the input pcap-like file. As a result, 
		it outputs a log file in Json format, so the result statistical 
		data are saved in Json representation. For parsing the input pcap file I used a 
		Scapy library,
		* secondly, a data from such Json file are displayed in charts on HTML page. 
		It uses Javascript and CSS languages for this purpose. To be more specific, 
		it operates with JQuery, Bootstrap and Highcharts libraries. 
		Metro Dashboard free CSS template has been used for a design part.
"""

import os
import sys
import json

import scapy.all as scapy
from scapy.all import Ether,IP, TCP,Raw

import time
import datetime
from datetime import date

class EndPoint:
	"""
	This class represends 1 endpoint in network. After initialization we know its MAC/IP adresses and used port
	"""
	def __init__(self, _srcMac, _dstMac, _srcIP, _dstIP, _srcPort, _dstPort):
		self.srcMac = _srcMac
		self.dstMac = _dstMac

		self.srcIP = _srcIP
		self.dstIP = _dstIP

		self.srcPort = _srcPort
		self.dstPort = _dstPort

		self.wndScale = 1
		self.startWndSize = 0

		self.wasFin = False
		self.wasHandshake = False

		self.allPkts = [] # List of object Packet
		self.pktAmount = 0

		self.startComm = None
		self.endComm = None	


	def show(self):
		""" For debugging purposes - print info about endpoint """
		print("------- Endpoint Info -------\n" + \
			"Start       => " + self.startComm + "\nEnd:        => " + self.endComm + \
			"\n[srcMac]    => " + self.srcMac + "\n[dstMac]    => " + self.dstMac + \
			"\n[srcIP]     => " + self.srcIP + "\n[dstIP]     => " + self.dstIP + \
			"\n[Wnd scale] => " + str(self.wndScale) + \
			"\n[wasFin]    => " + ( "YES" if self.wasFin else "NO") + \
			"\n[PktAmount] => " + (str(self.pktAmount)) + \
			"\n[srcPort]   => " + (str(self.srcPort)) + \
			"\n[dstPort]   => " + (str(self.dstPort)) + \
			"\n------- Packets -------")
		map(sys.stdout.write, (str(pkt) for pkt in self.allPkts))
		print("\n\n")


	def computeStartEndTime(self):
		""" Time boundaries of communication """
		firstPktTime = self.allPkts[0].timestamp
		lastPktTime	 = self.allPkts[len(self.allPkts)-1].timestamp

		self.startComm = datetime.datetime.fromtimestamp(firstPktTime).strftime("%Y-%m-%d %H:%M:%S.%f")
		self.endComm   = datetime.datetime.fromtimestamp(lastPktTime).strftime("%Y-%m-%d %H:%M:%S.%f")


	def	computeWindowSizes(self):
		""" Window sizes in time. Everything is already prepared from parsing stage. """
		startEpochTime = self.allPkts[0].timestamp
		currentWndSize = self.startWndSize * self.wndScale

		allWindowSizes = []
		for pkt in self.allPkts:
			if pkt.isSyn:
				continue

			elif pkt.isAck:
				currentWndSize = pkt.wndSize * self.wndScale # Shift is basically multiplication (wndScale is powered)

			elif not ptk.isFin:
				pass

			else:
				continue

			allWindowSizes.append([round(pkt.timestamp - startEpochTime, 5), currentWndSize])

		return allWindowSizes


	def	computeThroughput(self):
		""" Unfortunetaly not completed - just print packet sizes in their timestamps... """
		startEpochTime = self.allPkts[0].timestamp
		throughput = []
		for pkt in self.allPkts:
			throughput.append([round(pkt.timestamp - startEpochTime, 5), pkt.size])

		return throughput


	def computeRTT(self, receivPkts):
		""" 
			Iterating through data packets and searching for its ACK from opposite side.
			ACK = SEQ + data size
		"""
		rttValues = []

		for pkt in self.allPkts:
			if pkt.dataLen > 0:
				for opPkt in receivPkts:
					if (opPkt.tcpAck == (pkt.tcpSeq + pkt.dataLen - 1) and opPkt.isAck):
						rttValues.append([pkt.tcpSeq, 1000*(opPkt.timestamp - pkt.timestamp)])
						break
		return rttValues


	def	computeSeqNumbers(self):
		""" Calculate relative sequential numbers for each endpoint. """
		startEpochTime = self.allPkts[0].timestamp
		allSeqNumbers = []
		for pkt in self.allPkts:
			if pkt.isSyn or pkt.isAck or pkt.isFin:
				allSeqNumbers.append([round(pkt.timestamp - startEpochTime, 5), pkt.tcpSeq])

			else:
				continue

		return allSeqNumbers



class Packet:
	""" Class for packet representation - prepare its data and then better examination """
	def __init__(self, _id, _timestamp, _isSyn, _isAck, _isFin, _tcpSeq, _tcpAck, _wndSize, _size, _dataLen):
		self.id = _id
		self.timestamp = _timestamp # Epoch time - according to WireShark
		self.isSyn = _isSyn
		self.isAck = _isAck
		self.isFin = _isFin
		self.tcpSeq = _tcpSeq
		self.tcpAck = _tcpAck
		self.wndSize = _wndSize
		self.size = _size
		self.dataLen = _dataLen

	def __str__(self):
		""" For printing Packet class, you just have to print(str(pkt)) if pkt is the instance of Packet. """

		return "[" + str(self.id) + "] {:.2f}".format(self.timestamp) + "s" \
		", [SYN|ACK|FIN]: " + ( "YES|" if self.isSyn else "NO|") + ( "YES|" if self.isAck else "NO|") + \
		( "YES" if self.isFin else "NO") + \
		", [SeqN]: " + str(self.tcpSeq) + ", [AckN]: " + str(self.tcpAck) + \
		", [WndSz]: " + str(self.wndSize) + "\n"


def pcapParser(packets):
	""" Function fills Endpoint classes from the input Pcap-like file. """
	endPointA = None
	endPointB = None
	pktId = 1
	firstPktA = True
	firstPktB = True
	
	
	for pkt in packets:
		# Just for the first packet - save adresses and ports of each of 2 endpoints via instantiation of its object
		endPointA = EndPoint(pkt[Ether].src, pkt.dst, pkt[IP].src, pkt[IP].dst, pkt[TCP].sport, pkt[TCP].dport)
		endPointB = EndPoint(pkt[Ether].dst, pkt.src, pkt[IP].dst, pkt[IP].src, pkt[TCP].dport, pkt[TCP].sport)
		break

	# Save all necessary information about each packet to endpoint.allPkts list. 
	for pkt in packets:
		isSyn = False
		isAck = False
		wasFin = False

		if (pkt[TCP].flags & 1) > 0:
			wasFin = True

		if (pkt[TCP].flags & 2) > 0:
			isSyn = True

		if (pkt[TCP].flags & 16) > 0:
			isAck = True


		if endPointA.srcMac == pkt[Ether].src:
			if isSyn:
				options = pkt[TCP].options
				options = dict(options)
				endPointA.wndScale = 2**options['WScale'] # Compute Window scale

			if firstPktA and (isSyn or isAck):
				firstPktA = False
				relativeSeqValA = pkt[TCP].seq # Initial Seq/Ack numbers,
				relativeAckValA = pkt[TCP].ack # important for calculation of relative Seq/Ack numbers 

			processedPkt = Packet(pktId, pkt.time, isSyn, isAck, wasFin, pkt[TCP].seq - relativeSeqValA, pkt[TCP].ack - relativeAckValA, pkt[TCP].window, len(pkt), len(pkt[TCP].payload))
			endPointA.allPkts.append(processedPkt)

		elif endPointB.srcMac == pkt[Ether].src:
			if isSyn:
				options = pkt[TCP].options
				options = dict(options)
				endPointB.wndScale = 2**options['WScale'] # Compute Window scale

			if firstPktB and (isSyn or isAck):
				firstPktB = False
				relativeSeqValB = pkt[TCP].seq # Initial Seq/Ack numbers,
				relativeAckValB = pkt[TCP].ack # important for calculation of relative Seq/Ack numbers

			processedPkt = Packet(pktId, pkt.time, isSyn, isAck, wasFin, pkt[TCP].seq - relativeSeqValB, pkt[TCP].ack - relativeAckValB, pkt[TCP].window, len(pkt), len(pkt[TCP].payload))
			endPointB.allPkts.append(processedPkt)

		if isSyn and isAck and wasFin:
			endPointA.wasHandshake = True # Information if the handshake was presented
			endPointB.wasHandshake = True

		pktId += 1 # Id of packet - counter, like in wireshark. For better debugging

	return (endPointA, endPointB)


def main(argv):

	try:
		if(len(argv) != 1):
			print("Please provide only 1 input argument, the name of the input pcap file.")
			return 1

		# Proceed the input pcap file and save all packets and information about endpoints 
		# to two objects.
		packets = scapy.sniff(offline=argv[0], filter="tcp")
		(endPointA, endPointB) = pcapParser(packets)

		endPointA.computeStartEndTime()
		endPointB.computeStartEndTime()

		# Compute Receiver/Sender's Window Sizes
		senderWndSizes   = endPointA.computeWindowSizes()
		receivWndSizes = endPointB.computeWindowSizes()

		# Compute Relative Sequential numbers
		senderSeqNum = endPointA.computeSeqNumbers()
		receivSeqNum = endPointB.computeSeqNumbers()

		# Compute Transfer speed (Bandwidth)
		senderThroughput = endPointA.computeThroughput()
		receivThroughput = endPointB.computeThroughput()

		# Compute Round-trip time
		senderRTT = endPointA.computeRTT(endPointB.allPkts)
		receivRTT = endPointB.computeRTT(endPointA.allPkts)

		#endPointA.show()
		#endPointB.show()

		# Save statistical data in Json representation.
		jsonToOutput = {
			'SendWindow'	: [{'name': 'Window Size', 	'showInLegend': False, "data": senderWndSizes}],
			'RecWindow'		: [{'name': 'Window Size', 	'showInLegend': False, "data": receivWndSizes}],
			'SendBandwith'	: [{'name': 'Throughput', 	'showInLegend': False, "data": senderThroughput}],
			'RecBandwith'	: [{'name': 'Throughput', 	'showInLegend': False, "data": receivThroughput}],
			'SendRTT'		: [{'name': 'RTT', 			'showInLegend': False, "data": senderRTT}],
			'RecRTT'		: [{'name': 'RTT', 			'showInLegend': False, "data": receivRTT}],
			'SendSeqNum'	: [{'name': 'Seq num', 		'showInLegend': False, "data": senderSeqNum}],
			'RecSeqNum'		: [{'name': 'Seq num', 		'showInLegend': False, "data": receivSeqNum}]
			}

		with open('log/tcpLog.json', 'w') as outputFile:
			json.dump(jsonToOutput, outputFile)

	except Exception:
		print("An error, probably during input/output file.")
		exit(1)

if __name__ == "__main__":
	main(sys.argv[1:])

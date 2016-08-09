'''
Created on 03-09-2011

The purpose of this program is to convert a text file containing dead and noisy channels to XML.
The text file is provided by program RawDeadChannelsObtainer

@author: Marcin Borratynski
'''

import sys, csv
from xml.dom.minidom import Document

class D(dict): 
    """ This is a special multidimensional dictionary. 
    It allows following usage: dict[key1][key2]..[keyn] """
    def __missing__(self, key): 
        result = self[key] = D() 
        return result
    
deadChannelsMap = D() #map of dead channels, the keys are [arm][plane][chamber][channelType][channel]

def usage():
    """ This function is called in the case of improper program call. """
    print "Wrong program call, usage: python2.6 T1DeadChannelToXML.py deadChannelsFile.txt"
    exit(-1)
  
def createXMLFile():
    # Create the minidom document
    doc = Document()
    # Create the <top> base element
    top = doc.createElement("top")
    doc.appendChild(top)
    detectorSet = doc.createElement("t1_detector_set")
    detectorSet.setAttribute("id", "1")
    top.appendChild(detectorSet)
    for (arm, planes) in deadChannelsMap.items(): #for every arm add arm tag and its childs
        armElement = doc.createElement("t1_arm")
        armElement.setAttribute("id", arm)
        for (plane, chambers) in planes.items(): #for every plane add plane tag
            planeElement = doc.createElement("t1_plane")
            planeElement.setAttribute("id", plane)
            for (chamber, channelTypes) in chambers.items(): #for every chamber add chamber tag
                chamberElement = doc.createElement("t1_csc")
                chamberElement.setAttribute("id", chamber)
                for (channelType, channels) in channelTypes.items(): #for every channelType add plane tag
                    channelTypeElement = doc.createElement("t1_channel_type")
                    channelTypeElement.setAttribute("id", channelType)
                    channelTypeElement.setAttribute("fullmask", "no")
                    for (channel, trash) in channels.items(): #for every channel write tag
                        channelElement = doc.createElement("channel");
                        channelElement.setAttribute("id", channel)
                        channelTypeElement.appendChild(channelElement)
                    chamberElement.appendChild(channelTypeElement)
                planeElement.appendChild(chamberElement)
            armElement.appendChild(planeElement)
        detectorSet.appendChild(armElement)
    
    print doc.toprettyxml(indent="  ") #print XML document in readable way
        
def main():
	""" This is a main function of a program. The aim of this function is to parse a text file which contains 
	dead channels and then create an XML file following settled format. """
	if(len(sys.argv) != 2): #number of input arguments is not correct
		usage()
	fileName = sys.argv[1]    
	fileReader = csv.reader(open(fileName, 'rb'), delimiter=':', quotechar='\n')
	for row in fileReader:
		if len(row) == 5:
			arm, plane, chamber, channelType, channel = row
			deadChannelsMap[arm][plane][chamber][channelType][channel] = True;
	createXMLFile()

if __name__ == "__main__":
	main()

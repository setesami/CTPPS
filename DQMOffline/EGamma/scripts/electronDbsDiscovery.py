#!/usr/bin/env python

import os, sys

import DQMOffline.EGamma.electronDbsDiscovery as dbs

if os.environ['DBS_TIER_SECONDARY'] == "":
  files = dbs.search()
  print "dataset has", len(files), "files:"
  for file in files:
    print file
else:
  files = dbs.search()
  print "dataset has", len(files), "primary files:"
  for file in files:
    print file
  files = dbs.search2()
  print "dataset has", len(files), "secondary files:"
  for file in files:
    print file

	
	


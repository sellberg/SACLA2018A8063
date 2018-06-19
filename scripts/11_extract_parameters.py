#!/usr/bin/env python
####!/home/software/SACLA_tool/bin/python3.5
import numpy as np
import h5py as h
import matplotlib
import matplotlib.pyplot as plt
import argparse
import time
import sys

from argparse import ArgumentParser

parser = ArgumentParser()
parser = ArgumentParser(description="Analyze fluence dependence in runs pre-processed using DataConvert4")

parser.add_argument("-start", "--start-run",  type=int, dest="start", required=True,
                help="first run to process")
parser.add_argument("-stop", "--stop-run", type=int, dest="stop", default=0,
                help="last run to process (default: only process first run)")
parser.add_argument("-max", "--maximum",  type=int, dest="max", default=1000,
                help="maximum number of shots to analyze (default: 1000)")
parser.add_argument("-tag", "--output-tag", type=str, dest="tag", default="run",
                help="tag for output folder (default: run)")
parser.add_argument("-o", "--output-flag", type=str, dest="outputFlag",
                help="where to process runs. 'W' refers to /work/fperakis/ and 'UD' refers to '/UserData/fperakis' (default: UD)",
                choices=['W','UD'], default='UD')

args = parser.parse_args()
# -- default parameters
#run = 689614
srun = args.start
erun = args.stop
if erun == 0:
	erun = srun
runs = np.arange(srun,erun+1)
nshots = args.max
if args.outputFlag == 'UD':
	file_folder = '/UserData/fperakis/2018_6/'
else:
	file_folder = '/work/fperakis/2018_6/'
file_folder = '/Users/sellberg/kth/experiments/SACLA/SACLA2018A8063/data/'
mask_folder = '/Users/sellberg/kth/experiments/SACLA/SACLA2018A8063/masks/'
#src_folder  = '/home/fperakis/2018_06/git/SACLA2018A8063/scripts/' # src files folder
src_folder  = '/Users/sellberg/kth/experiments/SACLA/SACLA2018A8063/source/SACLA2018A8063/scripts/' # src files folder

defaultEphoton = 12000 # in eV
defaultWavelength = 1.0332 # in A
defaultDetectorDistance = 76.5/1000 # in m
defaultPixelSize = 50E-6 # in m
epsilon = 3.65 # energy to create electron-hole pair in Si, in eV/electron
systemGain = 18.1497 # in electron/ADU
defaultGain = defaultEphoton/epsilon/systemGain
print "Gain: %.1f ADU/photon" % defaultGain

xc = 1200.064 # center in pixels
yc = 1200.844 # center in pixels
center = [xc,yc]

# -- arrays for multirun comparison
run_im_on = []
run_im_off = []
run_q_on = []
run_q_off = []
run_Iq_on = []
run_Iq_off = []
run_Iq_norm_on = []
run_Iq_norm_off = []
run_hist = []
run_bin_centers = []
for run in runs:
	# -- files and folders
	file_name = '%d_%dshots_corrected.h5' % (run, nshots)
	file_path = file_folder + 'run%d/' % run + file_name
	
	# -- import data
	fh5       = h.File(file_path, 'r')
	run_key   = [ k for k in fh5.keys() if k.startswith('run_') ][0]
	tags      = fh5['/%s/detector_2d_assembled_1'%run_key].keys()[1:]
	tag_number = [ int(t[4:]) for t in tags ]
	Ephoton   = fh5['/%s/event_info/bl_3/oh_2/photon_energy_in_eV'%run_key][:]
	#laser_on  = fh5['/%s/event_info/bl_3/lh_1/laser_pulse_selector_status'%run_key][:].astype(np.bool)
	#bm1       = fh5['/%s/event_info/bl_3/oh_2/bm_1_pulse_energy_in_joule'%run_key][:]
	bm2       = fh5['/%s/event_info/bl_3/oh_2/bm_2_pulse_energy_in_joule'%run_key][:]*1000
	
	# -- process metadata
	Ephoton_median = np.median(Ephoton)
	Ephoton_std = Ephoton.std()
	print "Average photon energy %.0f +/- %.0f eV" % (Ephoton_median, Ephoton_std)
	#bm_avg = (bm1 + bm2)/2.
	print "Average pulse energy %.3f +/- %.3f mJ" % (bm2.mean(), bm2.std())
	
	fname = '%d_%dshots_corrected.dat' % (run, nshots)
	np.savetxt(file_folder + 'run%d/' % run + fname, np.array([tag_number, Ephoton, bm2]).transpose(), fmt=['%d','%.6e','%.6e'], delimiter='\t', newline='\n', header='tag number, photon energy (eV), pulse energy (mJ)', footer='', comments='# ')
	print "Saved parameters to: %s" % fname

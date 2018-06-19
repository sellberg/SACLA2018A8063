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
parser.add_argument("-exp", "--exp-year",  type=int, dest="exp", default=2016,
                help="experimental year to compress (default: 2016)")
#parser.add_argument("-multi", "--multi-run", action="store_true", dest="multi", required=False, default=False,
#                help="process multi-file run converted using DataConvert4")
parser.add_argument("-max", "--maximum",  type=int, dest="max", default=1000,
                help="maximum number of shots to analyze (default: 1000)")
parser.add_argument("-tag", "--output-tag", type=str, dest="tag", default="run",
                help="tag for output folder (default: run)")
parser.add_argument("-o", "--output-flag", type=str, dest="outputFlag",
                help="where to process runs. 'W' refers to /work/perakis/ and 'UD' refers to '/UserData/fperakis' (default: UD)",
                choices=['W','UD'], default='UD')

args = parser.parse_args()
# -- default parameters
#run = 498541
run = args.start
#runs = np.arange(498541, 498550)
runs = np.array([498541, 498543, 498544, 498545, 498547])
laser_fluence = np.array([1000, 700, 450, 280, 120])
#file_folder = '/UserData/fperakis/2016_10/'
file_folder = '/Users/sellberg/kth/experiments/SACLA/SACLA2016B8055/data/'
mask_folder = '/Users/sellberg/kth/experiments/SACLA/SACLA2016B8055/masks/'
#src_folder  = '/home/fperakis/2016_06/python_scripts/src/' # src files folder
src_folder  = '/Users/sellberg/kth/experiments/SACLA/SACLA2016B8055/source/SACLA2016B8055/src/' # src files folder

defaultEphoton = 12139.57 # in eV
defaultWavelength = 1.02132 # in A
defaultDetectorDistance = 76.095/1000 # in m
defaultPixelSize = 50E-6 # in m
epsilon = 3.65 # energy to create electron-hole pair in Si, in eV/electron
systemGain = 18.1497 # in electron/ADU
defaultGain = defaultEphoton/epsilon/systemGain
print "Gain: %.1f ADU/photon" % defaultGain

xc = 1199.058 # center in pixels
yc = 1199.507 # center in pixels
center = [xc,yc]

# -- import source functions
sys.path.insert(0, src_folder)
from img_class import *
from radial_profile import *

# -- import mask
mask_name = 'run%d_mask.npy' % (runs[0])
mask_path = mask_folder + mask_name
#mask_name = 'run%d_mask.npy' % (run)
#mask_path = mask_folder + mask_name
mask = np.load(mask_path).astype(bool)

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
	file_name = '%d_corrected.h5' % (run)
	file_path = file_folder + 'run%d/' % run + file_name
	
	# -- import data
	fh5       = h.File(file_path, 'r')
	run_key   = [ k for k in fh5.keys() if k.startswith('run_') ][0]
	tags      = fh5['/%s/detector_2d_assembled_1'%run_key].keys()[1:]
	Ephoton   = fh5['/%s/event_info/bl_3/oh_2/photon_energy_in_eV'%run_key][:]
	laser_on  = fh5['/%s/event_info/bl_3/lh_1/laser_pulse_selector_status'%run_key][:].astype(np.bool)
	bm1       = fh5['/%s/event_info/bl_3/oh_2/bm_1_pulse_energy_in_joule'%run_key][:]
	bm2       = fh5['/%s/event_info/bl_3/oh_2/bm_2_pulse_energy_in_joule'%run_key][:]
	
	# -- process metadata
	Ephoton_median = np.median(Ephoton)
	Ephoton_std = Ephoton.std()
	print "Average photon energy %.0f +/- %.0f eV" % (Ephoton_median, Ephoton_std)
	bm_avg = (bm1 + bm2)/2.
	
	# -- image generator
	img_gen   = ( fh5['%s/detector_2d_assembled_1/%s/detector_data'%(run_key,tag) ].value for tag in tags )
	num_im = len(tags)
	mean_int = np.zeros(num_im)
	
	# -- sanity check
	assert len(Ephoton) == num_im
	assert len(laser_on) == num_im
	assert len(bm_avg) == num_im
	
	# -- average image
	i = 0
	j = 0
	k = 0
	if laser_on[i]:
	    im_on = img_gen.next()/bm_avg[i]
	    im_off = np.zeros_like(im_on)
	    j += 1
	else:
	    im_off = img_gen.next()/bm_avg[i]
	    im_on = np.zeros_like(im_off)    
	    k += 1
	
	for im_next in img_gen:
	    t1 = time.time() 
	    mean_int[i] = np.average(im_next[mask].flatten())/(bm_avg[i]*1000)
	    if ((Ephoton[i] - Ephoton_median) < Ephoton_std) and (mean_int[i] > 2000) and (mean_int[i] < 5000):
	        if laser_on[i]:
	            im_on += im_next/bm_avg[i]
	            j += 1
	        else:
	            im_off += im_next/bm_avg[i]
	            k += 1
	    i += 1
	    print 'R.%d | S.%d/%.d | %.1f Hz'%(run,i+1,num_im,1.0/(time.time() - t1))
	    if (j >= args.max):
	        break
	# s.%.d || 'i,time.time() - t1
	i += 1
	im_on /= j*1000
	im_off /= k*1000
	print "Mean intensity %.2e +/- %.2e ADU/pixel/mJ" % (mean_int.mean(), mean_int.std())
	
	# -- radial mask (include only non zero)
	threshold = 1
	if mask is None:
	    mask = np.ones(im_on.shape,dtype=bool)
	    mask[im_on < threshold]=0
	
	q,Iq_on = radialProfile(im_on, center, mask=None, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)
	q,Iq_on_m = radialProfile(im_on, center, mask=mask, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)
	run_q_on.append(q)
	q,Iq_off_m = radialProfile(im_off, center, mask=mask, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)
	run_q_off.append(q)
	
	norm_q = np.where((q < 3.0) & (q > 1.5))
	norm_on = Iq_on_m[norm_q].sum()
	norm_off = Iq_off_m[norm_q].sum()
	print "Normalization: ", norm_on/norm_off
	
	# -- mean intensity histogram
	#hist,bins = np.histogram(mean_int[:i], bins=300)
        bins = np.arange(0, 10025, 25)
	hist,bins = np.histogram(mean_int[:i], bins=bins)
	bin_centers = np.array([(bins[i] + bins[i+1])/2. for i in range(len(bins)-1)])

        # -- append to run arrays
	run_im_on.append(im_on)
	run_im_off.append(im_off)
	run_Iq_on.append(Iq_on_m)
	run_Iq_off.append(Iq_off_m)
	run_Iq_norm_on.append(norm_on)
	run_Iq_norm_off.append(norm_off)
	run_hist.append(hist)
	run_bin_centers.append(bin_centers)

plt.figure()
plt.subplot(131)
plt.title('normalized radial profile')
#plt.plot(q,Iq_on,label='unmasked')
colors = ['b', 'g', 'r', 'm', 'c']
i = 0
#for r in runs:
for r in laser_fluence:
    plt.plot(run_q_on[i],run_Iq_on[i]/defaultGain/(run_Iq_norm_on[i]/run_Iq_norm_off[i]),color=colors[i],label='%d uJ - laser on' % r)
    plt.plot(run_q_off[i],run_Iq_off[i]/defaultGain,color=colors[i],ls='--',label='%d uJ - laser off' % r)
    i += 1
plt.xlabel(r'$\rm Q_x (\AA^{-1}$)')
#plt.ylabel(r'$\rm I_q$ (ADU/shot)')
plt.ylabel(r'$\rm I_q$ (photon/shot/mJ)')
plt.legend(frameon=False)

plt.subplot(132)
plt.title('normalized difference (laser on - laser off)')
i = 0
#for r in runs:
for r in laser_fluence:
    #plt.plot(q,Iq_on_m/defaultGain/(norm_on/norm_off) - Iq_off_m/defaultGain, label='laser on - laser off')
    plt.plot(run_q_off[i],run_Iq_on[i]/defaultGain/(run_Iq_norm_on[i]/run_Iq_norm_off[i]) - run_Iq_off[i]/defaultGain, label='%d uJ' % r)
    i += 1
plt.xlabel(r'$\rm Q_x (\AA^{-1}$)')
#plt.ylabel(r'$\rm I_q$ (ADU/shot)')
plt.ylabel(r'$\rm \Delta I_q$ (photon/shot/mJ)')
plt.legend(frameon=False)
#plt.imshow(im, vmin=0)
#plt.colorbar()

plt.subplot(133)
plt.title('mean intensity histogram')
i = 0
#for r in runs:
for r in laser_fluence:
    plt.plot(run_bin_centers[i], run_hist[i], label='%d uJ' % r)
    i += 1
plt.xlabel('intensity (ADU/mJ/pixel)')
plt.ylabel('counts (shots)')
plt.legend(frameon=False)
#plt.show()
plt.draw()

im = img_class(im_on*bm_avg.mean()*1000/defaultGain, 'run %d - laser on - %d shots (photon)' % (run, j))
im.draw_img()

im = img_class(im_off*bm_avg.mean()*1000/defaultGain, 'run %d - laser off - %d shots (photon)' % (run, k))
im.draw_img()

#plt.show()

#!/home/doniach/dermen/epd731/bin/python
import numpy as np
import h5py 
import matplotlib
import matplotlib.pyplot as plt
import argparse
import time
import pandas as pd
import sys

# -- default parameters
run = 448539
file_folder = '/UserData/fperakis/2016_6/01_test/'
src_folder  = '/home/fperakis/2016_06/python_scripts/src' # src files folder

defaultEphoton = 5000. # in eV
defaultWavelength = 2.48 # in A
defaultDetectorDistance = 0.25 # in m
defaultPixelSize = 50E-6 # in m
defaultSystemGain = 18.1497 # in electrons/ADU
xc = 1199#.582 # center in pixels
yc = 1200#.499 # center in pixels
center = [xc,yc]

# -- import source functions
sys.path.insert(0, src_folder)
from img_class import *
from radial_profile import *

# -- files and folders
file_name = '%d.h5'%(run)
file_path = file_folder+file_name

# -- import data
fh5       = h5py.File(file_path, 'r')
run_key   = [ k for k in fh5.keys() if k.startswith('run_') ][0]
tags      = fh5['/%s/detector_2d_assembled_1'%run_key].keys()[1:]

# -- image generator
num_im = len(tags)
img_gen   = (  fh5['%s/detector_2d_assembled_1/%s/detector_data'%(run_key,tag) ].value for tag in tags )
num_im = len(tags)
mean_int = np.zeros(num_im)

# -- average image
im = img_gen.next()
i=0
for im_next in img_gen:
    t1 = time.time() 
    mean_int[i] = np.average(im_next.flatten())
    im += im_next
    i  += 1
    print 'R.%d | S.%d/%.d | %.1f Hz'%(run,i,num_im,1.0/(time.time() - t1))
# s.%.d || 'i,time.time() - t1
im /= num_im

# -- radial mask (include only non zero)
threshold=1
mask=np.ones(im.shape,dtype=bool)
mask[im<threshold]=0


q,Iq = radialProfile(im, center, mask=None, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)
q,Iq_m = radialProfile(im, center, mask=mask, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)

plt.figure()
plt.plot(q,Iq,label='unmasked')
plt.plot(q,Iq_m,label='masked')
plt.xlabel(r'$\rm Q_x (\AA^{-1}$)')
plt.ylabel(r'$\rm I_q$ (ADUs/shot)')
plt.legend(frameon=False)

plt.show()


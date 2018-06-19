#!/home/software/SACLA_tool/bin/python2.7
import numpy as np
import h5py 
import matplotlib
import matplotlib.pyplot as plt
import argparse
import time
#import pandas as pd
import sys
  
# -- default parameters
run = 448571#448539
nshots = 100
threshold = 40

# -- folders
file_folder = '/UserData/fperakis/2016_6/run%d/'%(run) 
src_folder  = '/home/fperakis/2016_06/git/SACLA2016A8015/src/'
fig_folder  = '/home/fperakis/2016_06/figs/'

# -- filenames and paths
file_name = '%d.h5'%(run)
file_path = file_folder+file_name
sys.path.insert(0, src_folder)
from img_class import *

# -- import data
fh5       = h5py.File(file_path, 'r')
#h5keys = fh5.keys()
run_key   = [ k for k in fh5.keys() if k.startswith('run_') ][0]
#print fh5,run_key
#tags      = fh5['/%s/detector_2d_assembled_1'%run_key].keys()[1:]
tags      = fh5['/%s/detector_2d_1'%run_key].keys()[1:]

#gp =  fh5.get(h5keys[1])
#subgp =  gp.items()
#print subgp

# -- image generator
num_im = len(tags)
img_gen   = ( fh5['%s/detector_2d_1/%s/detector_data'%(run_key,tag) ].value for tag in tags )
num_im = len(tags)
mean_int = np.zeros(num_im)
#mean_int = np.zeros(num_im)

# -- get first image
im1 = img_gen.next()

# -- make mask
#mask = np.ones(im1.shape)
#mask[im1>threshold]=0

# -- loop over shots
im = img_gen.next()#im1*mask
i=0

for im_next in img_gen:
#for ishot in range(nshots):#img_gen:
    #im_next = img_gen.next()#img_gen[ishot]
    t1 = time.time() 
    #mean_int[i] = np.average(im_next[im_next<threshold].flatten())
    mean_int[i] = np.average(im_next.flatten())#[im_next<threshold].flatten())
    im += im_next#*mask
    i  += 1
    print 'R.%d | S.%d/%.d | %.1f Hz'%(run,i,num_im,1.0/(time.time() - t1))
im /= num_im

# -- run mean
#total_mean = np.average(mean_int)


# -- histogram
bi,bf,db = 5,30,0.1# min(sum_int),max(sum_int),1#0,1e6,1
hy,hx = np.histogram(mean_int,bins = np.arange(bi,bf,db))

# -- hit rate


# -- plot
plt.figure()

plt.subplot(2,1,1)
plt.imshow(np.rot90(im),vmax=24,vmin=12)
plt.colorbar()
plt.title('r.%d'%(run))

plt.subplot(2,1,2)
plt.bar(hx[:-1]-db/2.,hy,width = db)
plt.yscale('log',nonposy='clip')
plt.xlabel('mean ADUs per shot')
plt.ylabel('number of shots')
#plt.title('hit rate: %.3f percent'%(hit_rate))

plt.tight_layout()
# -- save figure
plt.savefig(fig_folder+'run%d.png'%(run))



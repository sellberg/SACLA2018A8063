#!/home/doniach/dermen/epd731/bin/python

import numpy as np
import h5py 
import matplotlib
import matplotlib.pyplot as plt
import argparse
import time
import pandas as pd

# -- default parameters
run = 448539
file_folder = '/UserData/fperakis/2016_6/01_test/'

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

# -- histogram
bi,bf,db = 1,100,1
hy,hx = np.histogram(im.flatten(),bins = np.arange(bi,bf,db)) 

# -- running mean
N=num_im/10
r_mean_int = pd.rolling_mean(mean_int, N)[N-1:]

# -- run mean
total_mean = np.average(im.flatten())


# -- plot
plt.figure()

plt.subplot(1,2,1)
plt.bar(hx[:-1]-db/2.,hy,width = db)
plt.title('mean: %.3f ADUS/pixel'%(total_mean))
plt.yscale('log',nonposy='clip')
plt.xlabel('ADUs per pixel')
plt.ylabel('number of pixels')

plt.subplot(1,2,2)
plt.plot(mean_int[:-1])
plt.plot(np.arange(N/2,num_im-N/2),r_mean_int[:-1],c='red',lw=1.5,label='running avg')
plt.xlabel('Shot number')
plt.ylabel('ADUS/pixels')
plt.legend(frameon=False)
plt.tight_layout()

plt.show()

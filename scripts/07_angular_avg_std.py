#!/home/software/SACLA_tool/bin/python2.7

import numpy as np
import h5py 
import matplotlib
import matplotlib.pyplot as plt
import argparse
import time
import sys
from argparse import ArgumentParser

# -- parse
parser = ArgumentParser()
parser.add_argument("-r", "--run",  type=int, dest="run", required=True, help="first run to process")
parser.add_argument("-n", "--n_shots",  type=int, dest="n_shots", help="number of shots",default=100)
args = parser.parse_args()
run = args.run#448566#448571
n_shots=args.n_shots#100

# -- parameters
ADU_thr = [50,300]
hit_thr = 75 # threshold for hit rate
std_dx = 200 #range to zoom in around the center
std_thr = 80


# -- constants
defaultEphoton = 4920#@5000. # in eV
defaultWavelength = 2.52 # in A
defaultDetectorDistance = 0.216 # in m
defaultPixelSize = 50E-6 # in m
defaultSystemGain = 18.1497 # in electrons/ADU
xc = 1201#.582 # center in pixels
yc = 1201#.499 # center in pixels
center = [xc,yc]


# -- files and folders
file_folder = '/UserData/fperakis/2016_6/run%d/'%(run)
fig_folder  = '/home/fperakis/2016_06/figs/'
save_folder = '/home/fperakis/2016_06/figs/analysed_data/'
src_folder  = '/home/fperakis/2016_06/python_scripts/src' # src files
file_name = '%d.h5'%(run)
file_path = file_folder+file_name

# -- import source functions
sys.path.insert(0, src_folder)
from radial_profile import *

# -- import data
fh5       = h5py.File(file_path, 'r')
run_key   = [ k for k in fh5.keys() if k.startswith('run_') ][0]
tags      = fh5['/%s/detector_2d_assembled_1'%run_key].keys()[1:]

# -- image generator
num_im = len(tags)
img_gen   = (  fh5['%s/detector_2d_assembled_1/%s/detector_data'%(run_key,tag) ].value for tag in tags )
num_im = len(tags)
mean_int = np.zeros(n_shots,dtype=float)
std_int = np.zeros(n_shots,dtype=float)

# -- average image
im1 = img_gen.next()

# -- make mask
mask = np.ones(im1.shape,dtype=bool)
mask[im1<ADU_thr[0]]=0
mask[im1>ADU_thr[1]]=0
im = np.array(im1)#*mask
im_bg,im_hit=np.array(im1),np.array(im1)


# -- loop
for i_shot in range(n_shots):
#for im_next in img_gen:
    im_next = np.array(img_gen.next())
    t1 = time.time() 
    mean_int[i_shot] = np.average(im_next.flatten())
    im += im_next
    if mean_int[i_shot]>hit_thr:
        std_int[i_shot] = np.std(im_next[xc-std_dx:xc+std_dx,yc-std_dx:yc+std_dx].flatten()) 
        if std_int[i_shot]<std_thr:
            im_hit += im_next
    else:
        im_bg += im_next
    print 'R.%d | S.%d/%.d | %.1f Hz'%(run,i_shot,n_shots,1.0/(time.time() - t1))

# -- hit rate
num_streaks = len(std_int[std_int>std_thr])
num_hits = float(len(mean_int[mean_int>hit_thr]))-num_streaks
hit_rate = num_hits/float(n_shots)

# -- normalise
im /= float(n_shots)
im_hit /= num_hits
im_bg /= (n_shots-num_hits)
im_corr = im_hit-im_bg

## -- histogram ADUs of mean image
bi,bf,db1 = -200,500,10#ADU_thr[0],ADU_thr[1],5#70,100,1#3e6,1e4
hy1,hx1 = np.histogram(im_corr,bins = np.arange(bi,bf,db1)) 

# -- histogram shots
bi,bf,db2 = 70.,100.,0.2#70,100,1#3e6,1e4
hy2,hx2 = np.histogram(mean_int,bins = np.arange(bi,bf,db2)) 

# -- histogram std
bi,bf,db3 = 50.,1000,10#70,100,1#3e6,1e4
hy3,hx3 = np.histogram(std_int,bins = np.arange(bi,bf,db3)) 

# -- angular integration
q,Iq_hit = radialProfile(np.array(im_hit,dtype=float), center, mask=mask, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)
q,Iq_bg = radialProfile(np.array(im_bg,dtype=float), center, mask=mask, wavelength=defaultWavelength, detectorDistance=defaultDetectorDistance, pixelSize=defaultPixelSize)

# -- subtract background
Iq = Iq_hit-Iq_bg
np.save(save_folder+'Iq_r%d.npy'%(run),np.array([q,Iq]))

print std_int

# -- plot
plt.figure(figsize=[15,8])
vmin,vmax=0,50#ADU_thr[1]
plt.subplot(2,3,1)
plt.imshow(im_corr,vmin=vmin,vmax=vmax)#,vmin=0,vmax=0.3)
plt.colorbar()
plt.title('r.%d'%(run))

plt.subplot(2,3,4)
plt.bar(hx1[:-1]-db1/2.,hy1,width = db1,color='green')
plt.axvline(x=ADU_thr[0],ls='--',color='gray')
plt.axvline(x=ADU_thr[1],ls='--',color='gray')
plt.yscale('log',nonposy='clip')
plt.xlabel('ADUs/shot')
plt.ylabel('number of pixels')
plt.title('Pixels histogram')

plt.subplot(2,3,2)
plt.bar(hx2[:-1]-db2/2.,hy2,width = db2)
plt.axvline(x=hit_thr,ls='--',color='gray',label='threshold=%d'%(hit_thr))
plt.yscale('log',nonposy='clip')
plt.xlabel('mean ADUs/shot')
plt.ylabel('number of shots')
#plt.title('Shots histogram')
plt.title('hit rate: %.2f percent'%(hit_rate*100))#mean: %.3f ADUS/pixel'%(total_mean))
plt.ylim([0.1,n_shots])
plt.legend(frameon=False)

plt.subplot(2,3,5)
plt.bar(hx3[:-1]-db3/2.,hy3,width = db3,color='red')
#plt.axvline(x=hit_thr,ls='--',color='gray',label='threshold=%d'%(hit_thr))
plt.yscale('log',nonposy='clip')
plt.xlabel('std ADUs/shot')
plt.ylabel('number of shots')
#plt.title('Shots histogram')
#plt.title('hit rate: %.2f percent'%(hit_rate*100))#mean: %.3f ADUS/pixel'%(total_mean))
plt.ylim([0.1,n_shots])
plt.legend(frameon=False)

'''
plt.subplot(2,3,5)
plt.plot(mean_int[:n_shots],'ro')
plt.axhline(y=hit_thr,ls='--',color='gray')
plt.title('hit rate: %.2f percent'%(hit_rate*100))#mean: %.3f ADUS/pixel'%(total_mean))
plt.xlabel('Shot number')
plt.ylabel('mean ADUS/shot')
'''
plt.subplot(2,3,3)
plt.plot(q,Iq_hit,label='hit')
plt.plot(q,Iq_bg,label='background')
plt.xlabel(r'$\rm Q_x (\AA^{-1}$)')
plt.ylabel(r'$\rm I_q$ (ADUs/shot)')
plt.legend(frameon=False)
plt.title('Angular average')
plt.xlim([0.05,0.55])
#plt.ylim([80,120])

plt.subplot(2,3,6)
plt.plot(q,Iq,label='hit-background')
plt.xlabel(r'$\rm Q_x (\AA^{-1}$)')
plt.ylabel(r'$\rm I_q$ (ADUs/shot)')
plt.legend(frameon=False)
plt.title('Angular average')
plt.xlim([0.05,0.55])
plt.ylim([10,50])

plt.tight_layout()

plt.savefig(fig_folder+'SAXSstd_run%d.png'%(run))
#plt.show()

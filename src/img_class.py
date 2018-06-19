########################################################
# Imaging class copied from Ingrid Ofte's pyana_misc code
########################################################

'''
An awesome class for plotting 2d images

Example of use: 
i = img_class(image, title)
i.draw_img()
'''

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

class img_class(object):
    def __init__(self, inarr, filename):
        #self.inarr = inarr*(inarr>0)
        self.inarr = inarr[:]
        self.filename = filename
        self.cmax = self.inarr.max()
        self.cmin = self.inarr.min()

    def on_keypress(self,event):
        if event.key == 'p':
            if not os.path.exists(write_dir + runtag):
                os.mkdir(write_dir + runtag)
            pngtag = write_dir + runtag + "/%s.png" % (self.filename)
            print "saving image as " + pngtag
            plt.savefig(pngtag)
        if event.key == 'r':
            plt.clim(self.cmin, self.cmax)
            plt.draw()

    def on_click(self, event):
        if event.inaxes:
            lims = self.axes.get_clim()
            colmin = lims[0]
            colmax = lims[1]
            range = colmax - colmin
            value = colmin + event.ydata * range
            if event.button is 1 :
                if value > colmin and value < colmax :
                    colmin = value
            elif event.button is 2 :
                colmin, colmax = self.orglims
            elif event.button is 3 :
                if value > colmin and value < colmax:
                    colmax = value
            plt.clim(colmin, colmax)
            plt.draw()

    def draw_img(self):
        fig = plt.figure()
        cid1 = fig.canvas.mpl_connect('key_press_event', self.on_keypress)
        cid2 = fig.canvas.mpl_connect('button_press_event', self.on_click)
        canvas = fig.add_subplot(111)
        canvas.set_title(self.filename)
        #plt.rc('image',origin='lower',interpolation='nearest')
        ##self.axes = plt.imshow(self.inarr[:,::-1], vmin = 0, vmax = self.cmax)
        self.axes = plt.imshow(self.inarr, interpolation='nearest', vmin=0, vmax=self.cmax)
        #if not canvas.xaxis_inverted():
        #   canvas.invert_xaxis() 
        self.colbar = plt.colorbar(self.axes, pad=0.01)
        self.orglims = self.axes.get_clim()
        self.printInstructions()
        plt.show()

    def draw_map(self):
        fig = plt.figure()
        cid1 = fig.canvas.mpl_connect('key_press_event', self.on_keypress)
        cid2 = fig.canvas.mpl_connect('button_press_event', self.on_click)
        canvas = fig.add_subplot(111)
        canvas.set_title(self.filename)
        #plt.rc('image',origin='lower',interpolation='nearest')
        ##self.axes = plt.imshow(self.inarr[:,::-1], vmin = 0, vmax = self.cmax)
        #self.axes = plt.imshow(self.inarr, interpolation='nearest', vmin=self.cmin, vmax=self.cmax, extent=[-1,1,-1,1])
        print "scale in Y-direction is inverted, need to change order of extent!!"
        aspectRatio = (np.max(q_ref) - np.min(q_ref))/(np.max(detectorMotorValues) - np.min(detectorMotorValues))
        self.axes = plt.imshow(self.inarr, interpolation='nearest', vmin=self.cmin, vmax=self.cmax, extent=[np.min(q_ref),np.max(q_ref),np.max(detectorMotorValues),np.min(detectorMotorValues)], aspect=aspectRatio)
        #if not canvas.xaxis_inverted():
        #   canvas.invert_xaxis() 
        self.colbar = plt.colorbar(self.axes, pad=0.01)
        self.orglims = self.axes.get_clim()
        self.printInstructions()
        plt.show()

    def printInstructions(self):
        print "Right-click on colorbar to set maximum scale."
        print "Left-click on colorbar to set minimum scale."
        print "Center-click on colorbar (or press 'r') to reset color scale."
        print "Interactive controls for zooming at the bottom of figure screen (zooming..etc)."
        print "Press 'p' to save PNG of image (with the current colorscales) in the appropriately named folder."
        print "Hit Ctl-\ or close all windows (Alt-F4) to terminate viewing program."

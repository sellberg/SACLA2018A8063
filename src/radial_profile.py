#!/home/doniach/dermen/epd731/bin/python
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

def radialProfile(image, center, mask=None, wavelength=1.12, detectorDistance=0.0625, pixelSize=50E-6):
    """
    Compute the radial intensity profile of `image`.(thanks dermen!)

    Parameters
    ----------
    image : np.ndarray
        The image to perform the radial profile on.

    center : tuple of floats (x, y)
        The center to use, in pixel units

    Returns
    -------
    q : np.ndarray
        The momentum transfer (A-1)

    Iq
        The average intensities as a function of q.
    """

    # compute the radii for image[y, x]
    x = np.arange(image.shape[1])
    y = np.arange(image.shape[0])
    xx, yy = np.meshgrid(x, y)
    xx -= center[0]
    yy -= center[1]
    rad = np.sqrt(xx*xx + yy*yy)

    # convert to momentum transfer in A-1
    qRad = 2.*np.pi*2.*np.sin(0.5*np.arctan2(rad*pixelSize, detectorDistance))/wavelength
    assert qRad.shape == image.shape

    # histogram the intensities and normalize by number of pixels in each bin to obtain average intensity
    nBins = max(image.shape)/2
    if mask is None:
        bin_values, bin_edges = np.histogram(qRad, weights=image, bins=nBins) 
        bin_normalizations = np.histogram(qRad, bins=bin_edges)
    else:
        bin_values, bin_edges = np.histogram(qRad[mask], weights=image[mask], bins=nBins)
        bin_normalizations = np.histogram(qRad[mask], bins=bin_edges)
    Iq = bin_values[np.where(bin_normalizations[0] > 0)]/bin_normalizations[0][np.where(bin_normalizations[0] > 0)]
    q = np.array([(bin_edges[i] + bin_edges[i+1])/2 for i in range(len(bin_values))])

    # run into 'High memory usage error', try to delete
    del x, y
    del xx, yy
    del rad, qRad
    del bin_values, bin_edges, bin_normalizations
    return q, Iq
 

"""
# -------------
def radial_map(center_x,center_y,nx,ny):
    '''
    radial map of the matrix (nx,ny) centered at (center_x,cnter_y)
    '''

    x,y= np.mgrid[0:nx,0:ny]
    x-=center_x
    y-=center_y
    rho = np.sqrt(x**2+y**2)

    return rho 

def take_ring(nx,ny,rho,inner_radius,outer_radius):
    '''
    Takes a ring with given inner and outer diameter
    '''
    ring = np.ones((nx,ny),dtype=bool)
    ring[rho<=inner_radius]=0
    ring[rho>outer_radius]=0
    return ring

def angular_integrate(data,mask,center_x,center_y,dr=2):
    '''
    Do the angular intergation with given dr
    '''
    nx,ny = data.shape[0],data.shape[1]
    rho = radial_map(center_x,center_y,nx,ny)
    rmax = max(rho.flatten())
    r_values = np.arange(0,rmax,dr)
    I_r = np.zeros(len(r_values))#np.zeros((rmax/dr,rmax/dr))
    for ir in range(len(r_values)):
        i_ring = take_ring(nx,ny,rho,r_values[ir],r_values[ir]+dr)*mask
        I_r[ir] = np.average(data[i_ring>0])
    rx = np.arange(0,rmax,dr)
    return rx,I_r 
"""

#!/home/software/SACLA_tool/bin/python2.7
import os
import sys

from argparse import ArgumentParser

parser = ArgumentParser()
parser = ArgumentParser(description="Process run data from the detector server to the analysis servers, saving the reconstructed images. This uses DataConvert4 and Kim's C++ and MATLAB code")

parser.add_argument("-start", "--start-run",  type=int, dest="start", required=True, 
                help="first run to process")
parser.add_argument("-stop", "--stop-run", type=int, dest="stop", default=0,
                help="last run to process (default: only process first run)")
parser.add_argument("-exp", "--exp-year",  type=int, dest="exp", default=2016, 
                help="experimental year to compress (default: 2016)")
parser.add_argument("-multi", "--multi-run", action="store_true", dest="multi", required=False, default=False,
                help="process multi-file run converted using DataConvert4")
parser.add_argument("-force", "--force-run", action="store_true", dest="force", required=False, default=False,
                help="force run to process (overwrite existing folder)")
parser.add_argument("-tag", "--output-tag", type=str, dest="tag", default="run",
                help="tag for output folder (default: run)")
parser.add_argument("-o", "--output-flag", type=str, dest="outputFlag", 
                help="where to process runs. 'W' refers to /work/perakis/ and 'UD' refers to '/UserData/fperakis' (default: UD)",
                choices=['W','UD'], default='UD')

args = parser.parse_args()

# where you want the output run h5 file to be stored
if args.outputFlag == 'W':
    outdir = '/work/fperakis/'
    print "Running dataCompress3.py in '%s' is currently not supported..." % outdir
    sys.exit(1)
elif args.outputFlag == 'UD':
    basedir = '/UserData/fperakis/'
    src_dir = basedir + 'code/'
    outdir = basedir + '%d_6/' % args.exp

if (args.stop < args.start):
    args.stop = args.start

assert( args.stop >= args.start )

for run in xrange( args.start, args.stop+1 ):
    RUN = str(run) # run number as string

    # make folder
    runFolder = os.path.join( outdir, '%s%s' % (args.tag, RUN) )
    warning_msg = 'Folder "%s" already exists! Please rename it and try again, or use -force to overwrite existing folder.' % runFolder
    assert( not os.path.exists(runFolder) or args.force ) , warning_msg
    if (args.force and os.path.exists(runFolder)):
        print 'Overwriting existing folder: %s' % runFolder
        os.system('rm -r %s' % runFolder)
    else:
        print 'Creating run folder: %s' % runFolder
    os.mkdir(runFolder)
    
    # copy scripts and enter directory
    print "Copying Kim's code to run folder:"
    scripts = ['matlab_background.csh', 'sacla_2016_6.m', 'av.out', 'integ.out', 'data_process.out', 'fit2d_old']
    for s in scripts:
        os.system('cp %s/%s %s/.' % (src_dir, s, runFolder))
        print '\t%s' % s
    org_dir = os.getcwd()
    os.chdir(runFolder)
    os.system("sed -i -e 's/448539/%s/g' sacla_2016_6.m" % RUN)
    #os.remove('sacla_2016_6.m-e')

    # make sure output is printed before we execute C-code
    sys.stdout.flush()
    os.system('./data_process.out %s' % RUN)
    

# check the status of the submitted job using:
# 'qstat'
# check output of the submitted job using:
# 'tail /home/fperakis/qsub_dump/compressed/%s_processed.o*'
# check error of the submitted job using:
# 'tail /home/fperakis/qsub_dump/compressed/%S_compress.e*'

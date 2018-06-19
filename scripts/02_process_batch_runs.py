#!/home/software/SACLA_tool/bin/python2.7
import os
import sys

from argparse import ArgumentParser

parser = ArgumentParser()
parser = ArgumentParser(description="Wrapper to run 01_process_runs.py that can be run as batch job")

parser.add_argument("-start", "--start-run", type=int, dest="start", required=True, 
                help="first run to process")
parser.add_argument("-stop", "--stop-run", type=int, dest="stop", default=0,
                help="last run to process (default: only process 1 run)")
parser.add_argument("-exp", "--exp-year",  type=int, dest="exp", default=2016, 
                help="experimental year to compress")
parser.add_argument("-multi", "--multi-run", action="store_true", dest="multi", required=False, default=False,
                help="process multi-file run converted using DataConvert4")
parser.add_argument("-force", "--force-run", action="store_true", dest="force", required=False, default=False,
                help="force run to process (overwrite existing folder)")
parser.add_argument("-tag", "--output-tag", type=str, dest="tag", default="run",
                help="tag for output folder (default: run)")
parser.add_argument("-o", "--output-flag", type=str, dest="outputFlag", 
                help="where to process runs. 'W' refers to /work/perakis/ and 'UD' refers to '/UserData/fperakis'",
                choices=['W','UD'], default='UD')

args = parser.parse_args()

log_dump = '/home/fperakis/qsub_dump/processed/'

if (args.stop < args.start):
    args.stop = args.start

assert( args.stop >= args.start )

for run in xrange( args.start, args.stop+1 ):
    RUN = str(run) # run number as string

#   create a batchfile as a string (first line)
    #batchFile  =  '#PBS -u fperakis\n'
    batchFile  =  '#PBS -N %s_process\n' % RUN
    batchFile +=  '#PBS -d %s\n' % log_dump
#   second line is the command
    cmd = ['%s/01_process_runs.py -start %s -exp %d -tag %s -o %s' % (os.getcwd(), RUN, args.exp, args.tag, args.outputFlag)]
    if args.multi:
        cmd.append('-multi')
    if args.force:
        cmd.append('-force')
    batchFile = batchFile + ' '.join(cmd)
    
#   save a temp  bacthfile and direct it to qsub
    outfile = open('tmpc.batch', 'w')
    outfile.write(batchFile)
    outfile.close()
    
    os.system('qsub < tmpc.batch')    
    os.remove('tmpc.batch')

# check the status of the submitted job using:
# 'qstat'
# check output of the submitted job using:
# 'tail /home/fperakis/qsub_dump/processed/%s_process.o*'
# check error of the submitted job using:
# 'tail /home/fperakis/qsub_dump/processed/%S_process.e*'

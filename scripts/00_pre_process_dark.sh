#!/bin/bash

if [ $# -eq 0 ];
then
    echo "Pre-process dark run using DataConvert4, including background averaging"
    echo ""
    echo "USAGE: pre_process_dark.sh <RUN_NUMBER> [<RUN_NUMBER2> ...]"
else
    for (( i=1; i<=$#; i++ )); do
        eval RUNNUMBER=\${$i}
        WORKDIR="/UserData/fperakis/2016_10/"
        SRCDIR=$WORKDIR"codes/"
        TAGDIR=$SRCDIR"tags/"
        LOGDIR=$SRCDIR"logs/"
        
        if test ! -e $WORKDIR; then
            mkdir $WORKDIR
            echo "Created "$WORKDIR
        fi
        if test ! -e $SRCDIR; then
            mkdir $SRCDIR
            echo "Created "$SRCDIR
        fi
        if test ! -e $TAGDIR; then
            mkdir $TAGDIR
            echo "Created "$TAGDIR
        fi
        if test ! -e $LOGDIR; then
            mkdir $LOGDIR
            echo "Created "$LOGDIR
        fi
        
        RUNDIR=$WORKDIR"run"$RUNNUMBER"/"
        echo 'Run directory: '$RUNDIR
        
        if test ! -e $RUNDIR; then
            mkdir $RUNDIR
            echo "Created "$RUNDIR
        fi
        CURRDIR=`pwd`
        
        cd $SRCDIR
        
        JOB=$RUNDIR/$RUNNUMBER.sh
        echo 'Creating qsub script: '$JOB
        
        echo "#/!bin/bash" > $JOB
        echo "#PBS -o "$RUNDIR/$RUNNUMBER.log >> $JOB
        echo "#PBS -e "$RUNDIR/$RUNNUMBER.err >> $JOB
        echo "#PBS -N DC4_dark"$RUNNUMBER >> $JOB
        echo "" >> $JOB
        echo "echo 'Making tag lists...'" >> $JOB
        echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -out "$TAGDIR"/tag_"$RUNNUMBER.list >> $JOB
        echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_off_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER"_dark.list" >> $JOB
        
        echo "echo 'Converting run"$RUNNUMBER"...'" >> $JOB
        echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER".h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
        
        echo "echo 'Averaging run"$RUNNUMBER"...'" >> $JOB
        echo "ImgAvg -inp "$RUNDIR/$RUNNUMBER".h5 -out "$RUNDIR/$RUNNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_avg" >> $JOB
        
        echo "echo 'Converting shots with x-rays off'" >> $JOB
        echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_dark.list -dir "$RUNDIR" -o "$RUNNUMBER"_dark.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark" >> $JOB
        
        echo "echo 'Averaging shots with x-rays off'" >> $JOB
        echo "ImgAvg -inp "$RUNDIR/$RUNNUMBER"_dark.h5 -out "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark_avg" >> $JOB
        
        qsub $JOB
        
        #echo 'Making tag lists...'
        #MakeTagList -b 3 -r $RUNNUMBER -det 'MPCCD-8-2-002' -inp xrays_on_condition_list.txt -out $TAGDIR/tag_$RUNNUMBER.list
        #MakeTagList -b 3 -r $RUNNUMBER -det 'MPCCD-8-2-002' -inp xrays_off_condition_list.txt -out $TAGDIR/tag_$RUNNUMBER'_dark.list'
        #
        #echo 'Converting background shots'
        #DataConvert4 -l $TAGDIR/tag_$RUNNUMBER'_dark.list' -dir $RUNDIR -o $RUNNUMBER'_dark.h5' > $LOGDIR/log_$RUNNUMBER'_dark'
        #
        #echo 'Averaging background shots'
        #ImgAvg -inp $RUNDIR/$RUNNUMBER'_dark.h5' -out $RUNDIR/$RUNNUMBER'_dark_avg.h5' > $LOGDIR/log_$RUNNUMBER'_dark_avg'
        #
        #echo 'Converting BG-subtracted run'$RUNNUMBER'...'
        #DataConvert4 -l $TAGDIR/tag_$RUNNUMBER.list -dir $RUNDIR -o $RUNNUMBER'_corrected.h5' -bkg $RUNDIR/$RUNNUMBER'_dark_avg.h5' > $LOGDIR/log_$RUNNUMBER'_corrected'
        
        cd $CURRDIR
        echo 'Done!'
    done
fi

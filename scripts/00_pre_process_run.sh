#!/bin/bash

if [ $# -eq 0 ];
then
    echo "Pre-process run using DataConvert4, including background subtraction"
    echo ""
    echo "USAGE: pre_process_run.sh <RUN_NUMBER>"
    echo "USAGE: pre_process_run.sh <RUN_NUMBER> <BG_NUMBER> [<RUN_NUMBER2> <BG_NUMBER2> ...]"
elif [ $# -eq 1 ];
then
    RUNNUMBER=$1
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
    echo "#PBS -N DC4_run"$RUNNUMBER >> $JOB
    echo "" >> $JOB
    echo "echo 'Making tag lists...'" >> $JOB
    echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER.list >> $JOB
    echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_off_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER"_dark.list" >> $JOB
    
    echo "echo 'Converting run"$RUNNUMBER"...'" >> $JOB
    echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER".h5 > "$LOGDIR"/log_"$RUNNUMBER >> $JOB
    
    echo "echo 'Converting background shots'" >> $JOB
    echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_dark.list -dir "$RUNDIR" -o "$RUNNUMBER"_dark.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark" >> $JOB
    
    echo "echo 'Averaging background shots'" >> $JOB
    echo "ImgAvg -inp "$RUNDIR/$RUNNUMBER"_dark.h5 -out "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark_avg" >> $JOB
    
    echo "echo 'Converting BG-subtracted run"$RUNNUMBER"...'" >> $JOB
    echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
    
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
else
    for (( i=1; i<=$#/2; i++ )); do
        ((j=2*i-1))
        eval RUNNUMBER=\${$j}
        ((j=2*i))
        eval BGNUMBER=\${$j}
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
	BGDIR=$WORKDIR"run"$BGNUMBER"/"
	echo 'Background directory: '$BGDIR

        if test ! -e $RUNDIR; then
            mkdir $RUNDIR
            echo "Created "$RUNDIR
        fi
        if test ! -e $BGDIR; then
            mkdir $BGDIR
            echo "Created "$BGDIR
        fi
        CURRDIR=`pwd`

	cd $SRCDIR

	JOB=$RUNDIR/$RUNNUMBER.sh
	echo 'Creating qsub script: '$JOB
	
	echo "#/!bin/bash" > $JOB
	echo "#PBS -o "$RUNDIR/$RUNNUMBER.log >> $JOB
	echo "#PBS -e "$RUNDIR/$RUNNUMBER.err >> $JOB
	echo "#PBS -N DC4_run"$RUNNUMBER >> $JOB
	echo "" >> $JOB
	echo "echo 'Making tag lists...'" >> $JOB
	#echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -out "$TAGDIR"/tag_"$RUNNUMBER.list >> $JOB
	echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER.list >> $JOB
	if test ! -e $BGDIR/$BGNUMBER'_avg.h5'; then
	    echo "MakeTagList -b 3 -r "$BGNUMBER" -det 'MPCCD-8-2-002' -out "$TAGDIR"/tag_"$BGNUMBER.list >> $JOB

	    echo "echo 'Converting background run"$BGNUMBER"...'" >> $JOB
	    echo "DataConvert4 -l "$TAGDIR"/tag_"$BGNUMBER".list -dir "$BGDIR" -o "$BGNUMBER".h5 > "$LOGDIR"/log_"$BGNUMBER >> $JOB

	    echo "echo 'Averaging background run"$BGNUMBER"...'" >> $JOB
	    echo "ImgAvg -inp "$BGDIR"/"$BGNUMBER".h5 -out "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$BGNUMBER"_avg" >> $JOB
	fi
	echo "echo 'Converting BG-subtracted run"$RUNNUMBER"...'" >> $JOB
	echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB

	qsub $JOB
    
	#echo 'Making tag lists...'
	##MakeTagList -b 3 -r $RUNNUMBER -det 'MPCCD-8-2-002' -out $TAGDIR/tag_$RUNNUMBER.list
	#MakeTagList -b 3 -r $RUNNUMBER -det 'MPCCD-8-2-002' -inp xrays_on_condition_list.txt -out $TAGDIR/tag_$RUNNUMBER.list
	#if test ! -e $BGDIR/$BGNUMBER'_avg.h5'; then
	#    MakeTagList -b 3 -r $BGNUMBER -det 'MPCCD-8-2-002' -out $TAGDIR/tag_$BGNUMBER.list
	#
	#    echo 'Converting background run'$BGNUMBER'...'
	#    DataConvert4 -l $TAGDIR/tag_$BGNUMBER.list -dir $BGDIR -o $BGNUMBER.h5 > $LOGDIR/log_$BGNUMBER
	#
	#    echo 'Averaging background run'$BGNUMBER'...'
	#    ImgAvg -inp $BGDIR/$BGNUMBER.h5 -out $BGDIR/$BGNUMBER'_avg.h5' > $LOGDIR/log_$BGNUMBER'_avg'
	#fi
	#echo 'Converting BG-subtracted run'$RUNNUMBER'...'
	#DataConvert4 -l $TAGDIR/tag_$RUNNUMBER.list -dir $RUNDIR -o $RUNNUMBER'_corrected.h5' -bkg $BGDIR/$BGNUMBER'_avg.h5' > $LOGDIR/log_$RUNNUMBER'_corrected'
	
        cd $CURRDIR
	echo 'Done!'
    done
fi

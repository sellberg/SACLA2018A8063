#!/bin/bash

if [ $# -lt 2 ];
then
    echo "Pre-process run using DataConvert4, including background subtraction"
    echo ""
    echo "USAGE: pre_process_run.sh <CUTOFF> <RUN_NUMBER>"
    echo "USAGE: pre_process_run.sh <CUTOFF> <RUN_NUMBER> <BG_NUMBER> [<RUN_NUMBER2> <BG_NUMBER2> ...]"
elif [ $# -eq 2 ];
then
    CUTOFF=$1
    RUNNUMBER=$2
    CONFFILE="/home/fperakis/conf/RunData_Effective_Event_Info.conf"
    WORKDIR="/UserData/fperakis/2018_6/"
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
    
    # taglists
    if test ! -e $TAGDIR/tag_$RUNNUMBER.list; then
	# cannot run this unless qsub
	echo "Need to run initial MakeTagList to determine length of run "$RUNNUMBER"..."
	JOB="temp.sh"
	echo "#/!bin/bash" > $JOB
	echo "#PBS -N MTL_run"$RUNNUMBER >> $JOB
	echo "" >> $JOB
	echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER".list" >> $JOB
	OUT=`qsub $JOB`
	echo "Submitted: "$OUT
	rm $JOB
	while test ! -e $TAGDIR/tag_$RUNNUMBER.list; do
	    echo "..."
	    sleep 5
	done
    fi
    NTOT=`head -n 1 $TAGDIR/tag_$RUNNUMBER.list | awk '{print $4}' | sed 's/.*://'`
    echo "Run "$RUNNUMBER" includes "$NTOT" shots with x-rays on"

    # taglists + run
    if [ $NTOT -gt $CUTOFF ]; then
        
        LTOT=`more $TAGDIR/tag_$RUNNUMBER.list | wc -l`
        #OFFSET=`expr $LTOT - $NTOT`
        OFFSET=$(($LTOT - $NTOT + 1))
        START=`sed $OFFSET'!d' $TAGDIR/tag_$RUNNUMBER.list`
        LEND=$(($OFFSET + $CUTOFF - 1))
    	START=`sed $OFFSET'!d' $TAGDIR/tag_$RUNNUMBER.list`
    	END=`sed $LEND'!d' $TAGDIR/tag_$RUNNUMBER.list`
        echo "Running "$CUTOFF" shots from run "$RUNNUMBER" with starttag="$START", and endtag="$END"..."
	echo "Tag "$START"-"$END" will be saved in: "$TAGDIR/tag_$RUNNUMBER'_'$CUTOFF'shots.list'

        JOB=$RUNDIR/$RUNNUMBER'_'$CUTOFF'shots.sh'
        echo 'Creating qsub script: '$JOB
        
        echo "#/!bin/bash" > $JOB
        echo "#PBS -o "$RUNDIR/$RUNNUMBER"_"$CUTOFF'shots.log' >> $JOB
        echo "#PBS -e "$RUNDIR/$RUNNUMBER"_"$CUTOFF'shots.err' >> $JOB
        echo "#PBS -N DC4_run"$RUNNUMBER"_"$CUTOFF'shots' >> $JOB
        echo "" >> $JOB
	    
    	echo "echo 'Making tag list for "$CUTOFF" shots with tag "$START" to tag "$END"...'" >> $JOB
    	echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -starttag "$START" -endtag "$END >> $JOB    	
	
    	echo "echo 'Converting "$CUTOFF" shots from run"$RUNNUMBER"...'" >> $JOB
    	if test -e $CONFFILE; then
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -f "$CONFFILE" -dir "$RUNDIR" -o "$RUNNUMBER"_"$CUTOFF"shots.h5 > "$LOGDIR"/log_"$RUNNUMBER"_"$CUTOFF"shots" >> $JOB
    	else
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -dir "$RUNDIR" -o "$RUNNUMBER"_"$CUTOFF"shots.h5 > "$LOGDIR"/log_"$RUNNUMBER"_"$CUTOFF"shots" >> $JOB
    	fi
    	
	OUT=`qsub $JOB`
	echo "Submitted: "$OUT
        
    else
	
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
    	if test -e $CONFFILE; then
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -f "$CONFFILE" -dir "$RUNDIR" -o "$RUNNUMBER".h5 > "$LOGDIR"/log_"$RUNNUMBER >> $JOB
    	else
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER".h5 > "$LOGDIR"/log_"$RUNNUMBER >> $JOB
    	fi
    	
    	echo "echo 'Converting background shots'" >> $JOB
    	echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_dark.list -dir "$RUNDIR" -o "$RUNNUMBER"_dark.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark" >> $JOB
    	
    	echo "echo 'Averaging background shots'" >> $JOB
    	echo "ImgAvg -inp "$RUNDIR/$RUNNUMBER"_dark.h5 -out "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_dark_avg" >> $JOB
    	
    	echo "echo 'Converting BG-subtracted run"$RUNNUMBER"...'" >> $JOB
    	if test -e $CONFFILE; then
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -f "$CONFFILE" -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
    	else
    		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$RUNDIR/$RUNNUMBER"_dark_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
    	fi
	echo "echo 'Done!'" >> $JOB
    	
    	OUT=`qsub $JOB`
    	echo "Submitted: "$OUT
    fi
    
    cd $CURRDIR
else
    for (( i=1; i<=$#/2; i++ )); do
        ((j=2*i))
        eval RUNNUMBER=\${$j}
        ((j=2*i+1))
        eval BGNUMBER=\${$j}
	CUTOFF=$1
	CONFFILE="/home/fperakis/conf/RunData_Effective_Event_Info.conf"
	WORKDIR="/UserData/fperakis/2018_6/"
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
	
	# taglists
	if test ! -e $TAGDIR/tag_$RUNNUMBER.list; then
	    # cannot run this unless qsub
	    echo "Need to run initial MakeTagList to determine length of run "$RUNNUMBER"..."
	    JOB="temp.sh"
	    echo "#/!bin/bash" > $JOB
	    echo "#PBS -N MTL_run"$RUNNUMBER >> $JOB
	    echo "" >> $JOB
	    echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER".list" >> $JOB
	    OUT=`qsub $JOB`
	    echo "Submitted: "$OUT
	    rm $JOB
	    while test ! -e $TAGDIR/tag_$RUNNUMBER.list; do
		echo "..."
		sleep 5
	    done
	fi
	NTOT=`head -n 1 $TAGDIR/tag_$RUNNUMBER.list | awk '{print $4}' | sed 's/.*://'`
	echo "Run "$RUNNUMBER" includes "$NTOT" shots with x-rays on"
	
	# taglists + bg-corrected run
	if [ $NTOT -gt $CUTOFF ]; then
	    
            LTOT=`more $TAGDIR/tag_$RUNNUMBER.list | wc -l`
            #OFFSET=`expr $LTOT - $NTOT`
            OFFSET=$(($LTOT - $NTOT + 1))
            START=`sed $OFFSET'!d' $TAGDIR/tag_$RUNNUMBER.list`
            LEND=$(($OFFSET + $CUTOFF - 1))
    	    START=`sed $OFFSET'!d' $TAGDIR/tag_$RUNNUMBER.list`
    	    END=`sed $LEND'!d' $TAGDIR/tag_$RUNNUMBER.list`
            echo "Running "$CUTOFF" shots from run "$RUNNUMBER" with starttag="$START", and endtag="$END"..."
	    echo "Tag "$START"-"$END" will be saved in: "$TAGDIR/tag_$RUNNUMBER'_'$CUTOFF'shots.list'
	    
            JOB=$RUNDIR/$RUNNUMBER'_'$CUTOFF'shots.sh'
            echo 'Creating qsub script: '$JOB
            
            echo "#/!bin/bash" > $JOB
            echo "#PBS -o "$RUNDIR/$RUNNUMBER"_"$CUTOFF'shots.log' >> $JOB
            echo "#PBS -e "$RUNDIR/$RUNNUMBER"_"$CUTOFF'shots.err' >> $JOB
            echo "#PBS -N DC4_run"$RUNNUMBER"_"$CUTOFF'shots' >> $JOB
            echo "" >> $JOB
	        
    	    echo "echo 'Making tag list for "$CUTOFF" shots with tag "$START" to tag "$END"...'" >> $JOB
    	    echo "MakeTagList -b 3 -r "$RUNNUMBER" -det 'MPCCD-8-2-002' -inp "$SRCDIR"xrays_on_condition_list.txt -out "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -starttag "$START" -endtag "$END >> $JOB    	
	else
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
	fi
	
	# background
	if test ! -e $BGDIR/$BGNUMBER'_avg.h5'; then
	    echo "MakeTagList -b 3 -r "$BGNUMBER" -det 'MPCCD-8-2-002' -out "$TAGDIR"/tag_"$BGNUMBER.list >> $JOB
	
	    echo "echo 'Converting background run"$BGNUMBER"...'" >> $JOB
	    echo "DataConvert4 -l "$TAGDIR"/tag_"$BGNUMBER".list -dir "$BGDIR" -o "$BGNUMBER".h5 > "$LOGDIR"/log_"$BGNUMBER >> $JOB
	
	    echo "echo 'Averaging background run"$BGNUMBER"...'" >> $JOB
	    echo "ImgAvg -inp "$BGDIR"/"$BGNUMBER".h5 -out "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$BGNUMBER"_avg" >> $JOB
	fi
	
	# BG-subtracted run
	if [ $NTOT -gt $CUTOFF ]; then
    	    echo "echo 'Converting "$CUTOFF" shots from BG-subtracted run"$RUNNUMBER"...'" >> $JOB
	    if test -e $CONFFILE; then
		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -f "$CONFFILE" -dir "$RUNDIR" -o "$RUNNUMBER"_"$CUTOFF"shots_corrected.h5 -bkg "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_"$CUTOFF"shots_corrected" >> $JOB
	    else
		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER"_"$CUTOFF"shots.list -dir "$RUNDIR" -o "$RUNNUMBER"_"$CUTOFF"shots_corrected.h5 -bkg "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_"$CUTOFF"shots_corrected" >> $JOB
	    fi
	else
	    echo "echo 'Converting BG-subtracted run"$RUNNUMBER"...'" >> $JOB
	    if test -e $CONFFILE; then
		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -f "$CONFFILE" -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
	    else
		echo "DataConvert4 -l "$TAGDIR"/tag_"$RUNNUMBER".list -dir "$RUNDIR" -o "$RUNNUMBER"_corrected.h5 -bkg "$BGDIR"/"$BGNUMBER"_avg.h5 > "$LOGDIR"/log_"$RUNNUMBER"_corrected" >> $JOB
	    fi
	fi
	echo "echo 'Done!'" >> $JOB
	
	OUT=`qsub $JOB`
	echo "Submitted: "$OUT
	
        cd $CURRDIR
    done
fi

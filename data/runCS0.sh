#!/bin/bash
: '
The purpose of this script is to produce ROC, DET and CMC curves for each split of CS0 using the evaluation utilites under janus/src/utils
Utility: Description
janus_create_templates: Enrolls a janus_metadata file, outputs templates to file on disk.  This output is used in janus_evaluate_verify and for the query gallery of janus_evaluate_search.
janus_create_gallery: Enrolls a janus_metadata file, outputs a janus_flat_gallery to file on disk.  This output is used as the target gallery of janus_evaluate_search.
janus_evaluate_verify: Perform 1:1 verification on two galleries.  Output matrices are used to generate ROC/DET curves.
janus_evaluate_search: Perform 1:n search on two galleries.  Output matrices are used to generate CMC curve. 

Please email results to noblis@libjanus.org
The T&E team will use this output as a reference while reproducing results.
'
# Set output file name (no extension)
PERFORMER="performer_name"
SDK_PATH="/usr/local/"
CS0_DIR="/path/to/CS0/"
TEMP_PATH=$TMPDIR
RESULTS="/path/to/results"
ALGORITHM=""

for i in `seq 1 10`;
do
	mkdir ${RESULTS}/split${i}
	for PROTOCOL in A B
	do
		# Enroll galleries using "janus_create_templates" for use in "janus_evaluate_verify"
		echo janus_create_templates probe > $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_probe.csv $RESULTS/test_${i}_${PROTOCOL}_probe_templates.gal $ALGORITHM >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		echo janus_create_templates gallery >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_gal.csv $RESULTS/test_${i}_${PROTOCOL}_gal_templates.gal $ALGORITHM >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt

		# Evaluate protocol A verification using "janus_evaluate_verify"
		echo janus_evaluate_verify >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		janus_evaluate_verify $SDK_PATH $TEMP_PATH $RESULTS/test_${i}_${PROTOCOL}_gal_templates.gal $RESULTS/test_${i}_${PROTOCOL}_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_probe.csv $RESULTS/split${i}/verify_${i}_${PROTOCOL}.mtx $RESULTS/split${i}/verify_${i}_${PROTOCOL}.mask $ALGORITHM >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt

		# Enroll target gallery using "janus_create_gallery" for use in "janus_evaluate_search"
		echo janus_create_gallery >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		janus_create_gallery $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_gal.csv $RESULTS/test_${i}_${PROTOCOL}_gal_flat.gal $ALGORITHM >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt

		#Evaluate protocol A search using "janus_evaluate_search"
		echo janus_evaluate_search >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt
		janus_evaluate_search $SDK_PATH $TEMP_PATH $RESULTS/test_${i}_${PROTOCOL}_gal_flat.gal $RESULTS/test_${i}_${PROTOCOL}_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_${PROTOCOL}_probe.csv $RESULTS/split${i}/search_${i}_${PROTOCOL}.mtx $RESULTS/split${i}/search_${i}_${PROTOCOL}.mask 150 $ALGORITHM >> $RESULTS/split${i}/split${i}_${PROTOCOL}_log.txt

		# Evaluate output matrices using OpenBR
		if type br &> /dev/null; then
			br -eval $RESULTS/split${i}/verify_${i}_${PROTOCOL}.mtx $RESULTS/split${i}/verify_${i}_${PROTOCOL}.mask $RESULTS/split${i}/verify_${i}_${PROTOCOL}.csv -plot $RESULTS/split${i}/verify_${i}_${PROTOCOL}.csv $RESULTS/split${i}/verify_${i}_${PROTOCOL}.pdf
			br -eval $RESULTS/split${i}/search_${i}_${PROTOCOL}.mtx $RESULTS/split${i}/search_${i}_${PROTOCOL}.mask $RESULTS/split${i}/search_${i}_${PROTOCOL}.csv -plot $RESULTS/split${i}/search_${i}_${PROTOCOL}.csv $RESULTS/split${i}/search_${i}_${PROTOCOL}.pdf
			# Take ROC/DET curves from verify and CMC curve from search
			if type pdftk &> /dev/null; then
				pdftk A=$RESULTS/split${i}/verify_${i}_${PROTOCOL}.pdf B=$RESULTS/split${i}/search_${i}_${PROTOCOL}.pdf cat A1-5 B6 A7-8 output $RESULTS/split${i}/split${i}_${PROTOCOL}_results.pdf
				rm $RESULTS/split${i}/verify_${i}_${PROTOCOL}.pdf
				rm $RESULTS/split${i}/search_${i}_${PROTOCOL}.pdf
			fi
		fi
	done
	rm $RESULTS/*.gal

	# Plot results on protocol A & B on same plots
	if type br &> /dev/null; then
		br -plot $RESULTS/split${i}/verify_${i}_A.csv $RESULTS/split${i}/verify_${i}_B.csv $RESULTS/split${i}/verify_${i}.pdf
		br -plot $RESULTS/split${i}/search_${i}_A.csv $RESULTS/split${i}/search_${i}_B.csv $RESULTS/split${i}/search_${i}.pdf
		if type pdftk &> /dev/null; then
			pdftk A=$RESULTS/split${i}/verify_${i}.pdf B=$RESULTS/split${i}/search_${i}.pdf cat A1-5 B6 A7-8 output $RESULTS/split${i}/results_${i}_AB.pdf
			rm $RESULTS/split${i}/verify_${i}.pdf
			rm $RESULTS/split${i}/search_${i}.pdf
		fi
		rm $RESULTS/split${i}/*.R
	fi
done

# Plot results across all splits
if type br &> /dev/null; then
	for PROTOCOL in A B
	do	
		br -plot $RESULTS/split1/verify_1_${PROTOCOL}.csv $RESULTS/split2/verify_2_${PROTOCOL}.csv $RESULTS/split3/verify_3_${PROTOCOL}.csv $RESULTS/split4/verify_4_${PROTOCOL}.csv $RESULTS/split5/verify_5_${PROTOCOL}.csv $RESULTS/split6/verify_6_${PROTOCOL}.csv $RESULTS/split7/verify_7_${PROTOCOL}.csv $RESULTS/split8/verify_8_${PROTOCOL}.csv $RESULTS/split9/verify_9_${PROTOCOL}.csv $RESULTS/split10/verify_10_${PROTOCOL}.csv $RESULTS/verify_${PROTOCOL}.pdf
		br -plot $RESULTS/split1/search_1_${PROTOCOL}.csv $RESULTS/split2/search_2_${PROTOCOL}.csv $RESULTS/split3/search_3_${PROTOCOL}.csv $RESULTS/split4/search_4_${PROTOCOL}.csv $RESULTS/split5/search_5_${PROTOCOL}.csv $RESULTS/split6/search_6_${PROTOCOL}.csv $RESULTS/split7/search_7_${PROTOCOL}.csv $RESULTS/split8/search_8_${PROTOCOL}.csv $RESULTS/split9/search_9_${PROTOCOL}.csv $RESULTS/split10/search_10_${PROTOCOL}.csv $RESULTS/search_${PROTOCOL}.pdf
		if type pdftk &> /dev/null; then
			pdftk A=$RESULTS/verify_${PROTOCOL}.pdf B=$RESULTS/search_${PROTOCOL}.pdf cat A3-5 B6 A7 output $RESULTS/protocol_${PROTOCOL}_results.pdf
			rm $RESULTS/verify_${PROTOCOL}.pdf
			rm $RESULTS/search_${PROTOCOL}.pdf
		fi
	done
	br -plot $RESULTS/split1/verify_1_A.csv $RESULTS/split2/verify_2_A.csv $RESULTS/split3/verify_3_A.csv $RESULTS/split4/verify_4_A.csv $RESULTS/split5/verify_5_A.csv $RESULTS/split6/verify_6_A.csv $RESULTS/split7/verify_7_A.csv $RESULTS/split8/verify_8_A.csv $RESULTS/split9/verify_9_A.csv $RESULTS/split10/verify_10_A.csv $RESULTS/split1/verify_1_B.csv $RESULTS/split2/verify_2_B.csv $RESULTS/split3/verify_3_B.csv $RESULTS/split4/verify_4_B.csv $RESULTS/split5/verify_5_B.csv $RESULTS/split6/verify_6_B.csv $RESULTS/split7/verify_7_B.csv $RESULTS/split8/verify_8_B.csv $RESULTS/split9/verify_9_B.csv $RESULTS/split10/verify_10_B.csv $RESULTS/verify.pdf
	br -plot $RESULTS/split1/search_1_A.csv $RESULTS/split2/search_2_A.csv $RESULTS/split3/search_3_A.csv $RESULTS/split4/search_4_A.csv $RESULTS/split5/search_5_A.csv $RESULTS/split6/search_6_A.csv $RESULTS/split7/search_7_A.csv $RESULTS/split8/search_8_A.csv $RESULTS/split9/search_9_A.csv $RESULTS/split10/search_10_A.csv $RESULTS/split1/search_1_B.csv $RESULTS/split2/search_2_B.csv $RESULTS/split3/search_3_B.csv $RESULTS/split4/search_4_B.csv $RESULTS/split5/search_5_B.csv $RESULTS/split6/search_6_B.csv $RESULTS/split7/search_7_B.csv $RESULTS/split8/search_8_B.csv $RESULTS/split9/search_9_B.csv $RESULTS/split10/search_10_B.csv $RESULTS/search.pdf
	if type pdftk &> /dev/null; then
		pdftk A=$RESULTS/verify.pdf B=$RESULTS/search.pdf cat A3-5 B6 A7 output $RESULTS/results_AB.pdf
		rm $RESULTS/verify.pdf
		rm $RESULTS/search.pdf
	fi
	rm $RESULTS/*.R
fi
cd $RESULTS
tar -cvzf results_${PERFORMER}_${ALGORITHM}.tar.gz split* *.pdf

# OPTIONAL
# email results with "mutt" http://www.guckes.net/Mutt/install.php3
# apt-get install mutt
# echo "CS0 results" | mutt -a results_${PERFORMER}_${ALGORITHM}.tar.gz -s "CS0 results" -- noblis@libjanus.org
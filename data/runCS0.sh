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
OUT_FILE="results"
# Set SDK path
SDK_PATH="/usr/local/"
# Path to images
CS0_DIR="/path/to/CS0/"
# Set temp_path
TEMP_PATH="/temp/path/"
# Save directory for .gal files
GALLERY="/path/to/galleries/"
# Save directory for .mtx and .mask files
MATRIX="/path/to/matrices/"
# Save directory for results
RESULTS="/path/to/results/"
# Set algorithm
ALGORITHM=""

for i in `seq 1 10`;
do
	mkdir ${results}/split${i}
	# Protocol A
	# Enroll galleries using "janus_create_templates" for use in "janus_evaluate_verify"
	janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_A_probe.csv $GALLERY/test_${i}_A_probe_templates.gal $ALGORITHM
	janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_A_gal.csv $GALLERY/test_${i}_A_gal_templates.gal $ALGORITHM

	# Evaluate protocol A verification using "janus_evaluate_verify"
	janus_evaluate_verify $SDK_PATH $TEMP_PATH $GALLERY/test_${i}_A_gal_templates.gal $GALLERY/test_${i}_A_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_A_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_A_probe.csv $MATRIX/verify_${i}_A.mtx $MATRIX/verify_${i}_A.mask $ALGORITHM

	# Enroll target gallery using "janus_create_gallery" for use in "janus_evaluate_search"
	janus_create_gallery $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_A_gal.csv $GALLERY/test_${i}_A_gal_flat.gal $ALGORITHM

	#Evaluate protocol A search using "janus_evaluate_search"
	janus_evaluate_search $SDK_PATH $TEMP_PATH $GALLERY/test_${i}_A_gal_flat.gal $GALLERY/test_${i}_A_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_A_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_A_probe.csv $MATRIX/search_${i}_A.mtx $MATRIX/search_${i}_A.mask 150 $ALGORITHM

	# Protocol B
	# Enroll galleries using "janus_create_templates" for use in "janus_evaluate_verify"
	janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_B_probe.csv $GALLERY/test_${i}_B_probe_templates.gal $ALGORITHM
	janus_create_templates $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_B_gal.csv $GALLERY/test_${i}_B_gal_templates.gal $ALGORITHM

	# Evaluate protocol B verification using "janus_evaluate_verify"
	janus_evaluate_verify $SDK_PATH $TEMP_PATH $GALLERY/test_${i}_B_gal_templates.gal $GALLERY/test_${i}_B_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_B_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_B_probe.csv $MATRIX/verify_${i}_B.mtx $MATRIX/verify_${i}_B.mask $ALGORITHM

	# Enroll target gallery using "janus_create_gallery" for use in "janus_evaluate_search"
	janus_create_gallery $SDK_PATH $TEMP_PATH $CS0_DIR $CS0_DIR/protocol/split${i}/test_${i}_B_gal.csv $GALLERY/test_${i}_B_gal_flat.gal $ALGORITHM

	# Evaluate protocol B search using "janus_evaluate_search"
	janus_evaluate_search $SDK_PATH $TEMP_PATH $GALLERY/test_${i}_B_gal_flat.gal $GALLERY/test_${i}_B_probe_templates.gal $CS0_DIR/protocol/split${i}/test_${i}_B_gal.csv $CS0_DIR/protocol/split${i}/test_${i}_B_probe.csv $MATRIX/search_${i}_B.mtx $MATRIX/search_${i}_B.mask 150 $ALGORITHM

	# Evaluate output matrices using OpenBR
	# Protocol A
	br -eval $MATRIX/verify_${i}_A.mtx $MATRIX/verify_${i}_A.mask $RESULTS/split${i}/verify_${i}_A.csv -plot $RESULTS/split${i}/verify_${i}_A.csv $RESULTS/split${i}/verify_${i}_A.pdf
	br -eval $MATRIX/search_${i}_A.mtx $MATRIX/search_${i}_A.mask $RESULTS/split${i}/search_${i}_A.csv -plot $RESULTS/split${i}/search_${i}_A.csv $RESULTS/split${i}/search_${i}_A.pdf
	# Take ROC/DET curves from verify and CMC curve from search
	pdftk A=$RESULTS/split${i}/verify_${i}_A.pdf B=$RESULTS/split${i}/search_${i}_A.pdf cat A1-5 B6 A7-8 output $RESULTS/split${i}/split${i}_A_results.pdf
	rm $RESULTS/split${i}/verify_${i}_A.pdf
	rm $RESULTS/split${i}/search_${i}_A.pdf

	# Protocol B
	br -eval $MATRIX/verify_${i}_B.mtx $MATRIX/verify_${i}_B.mask $RESULTS/split${i}/verify_${i}_B.csv -plot $RESULTS/split${i}/verify_${i}_B.csv $RESULTS/split${i}/verify_${i}_B.pdf
	br -eval $MATRIX/search_${i}_B.mtx $MATRIX/search_${i}_B.mask $RESULTS/split${i}/search_${i}_B.csv -plot $RESULTS/split${i}/search_${i}_B.csv $RESULTS/split${i}/search_${i}_B.pdf
	# Take ROC/DET curves from verify and CMC curve from search
	pdftk A=$RESULTS/split${i}/verify_${i}_B.pdf B=$RESULTS/split${i}/search_${i}_B.pdf cat A1-5 B6 A7-8 output $RESULTS/split${i}/split${i}_B_results.pdf
	rm $RESULTS/split${i}/verify_${i}_B.pdf
	rm $RESULTS/split${i}/search_${i}_B.pdf
	rm $RESULTS/split${i}/*.R
done

cd $RESULTS
tar -cvzf ${OUT_FILE}_${ALGORITHM}.tar.gz split*

# OPTIONAL
# email results with "mutt" http://www.guckes.net/Mutt/install.php3
# apt-get install mutt
# echo "CS0 results" | mutt -a ${OUT_FILE}_${ALGORITHM}.tar.gz -s "CS0 results" -- noblis@libjanus.org
#!/bin/bash

set -e

# Generates all the YAML fields from using Papadakis' algorithm
# to detect subsuming mutants.

if ! test -d .venv; then
  virtualenv .venv
fi
source .venv/bin/activate
pip install -r requirements.txt

for i in V8_killed_*.csv; do
  python 01-subsumption.py "$i" --output "${i%.csv}-subsuming.yaml"
done

# Computes subsumption relationships across all tests
python 01-subsumption.py V8_killed_*.csv --output V8_killed_all.yaml

# Computes subsumption relationships across all tests while
# considering one of each set of duplicated mutants (30 samples)
mkdir -p sample-duplicated
for i in `seq -w 1 30`; do
  OUTPUT="sample-duplicated/$i-subsumed.yaml"
  python 01-subsumption.py V8_killed_*.csv --sample-duplicated --output "$OUTPUT"
  python 02-operators.py "$OUTPUT" --output "sample-duplicated/$i-stats.csv"
done

# Computes operator statistics (across all tests)
python 02-operators.py V8_killed_all.yaml --output V8_operatorStats_noDups.csv

# Computes times each operator is part of the subsuming operators set
python 03-within-subsuming.py V8_operatorStats_noDups.csv --output V8_inSubsuming_noDups.csv
python 03-within-subsuming.py sample-duplicated/*-stats.csv --output V8_inSubsuming_sampleDups.csv

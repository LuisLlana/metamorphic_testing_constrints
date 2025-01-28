#!/bin/bash

# Generates all the YAML fields from using Papadakis' algorithm
# to detect subsuming mutants.

if ! test -d .venv; then
  virtualenv .venv
fi
source .venv/bin/activate
pip install -r requirements.txt

for i in V8_killed_*.csv; do
  python subsumption.py "$i" --output "${i%.csv}-subsuming.yaml"
done

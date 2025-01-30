#!/usr/bin/env python

import csv
from collections import defaultdict
import yaml

def subsumes(comparisons, other_comparisons):
    """
    Computes whether the left mutant subsumes the other, that is, if
    for every i in 1..len(comparisons), comparisons[i] == 1 implies
    other_comparisons[i] == 1.

    Will raise an error if len(comparisons) != len(other_comparisons).

    >>> subsumes([], [])
    True
    >>> subsumes([1], [0])
    False
    >>> subsumes([0], [0])
    True
    >>> subsumes([1], [1])
    True
    >>> subsumes([0, 1], [1, 1])
    True
    >>> subsumes([1, 1], [0, 1])
    False
    """
    if len(comparisons) != len(other_comparisons):
        raise Exception("Comparisons do not have the same length: {} != {}".format(len(comparisons), len(other_comparisons)))

    return all(other_comparisons[i] == 1 if comparisons[i] == 1 else True for i in range(len(comparisons)))


def compute_subsuming(all_results):
    """
    Classifies the mutants into duplicate, alive, and subsuming. Uses
    the algorithm discussed in:

      M. Papadakis, C. Henard, M. Harman, Y. Jia, and Y. Le Traon, ‘Threats to
      the validity of mutation-based test assessment’, in Proceedings of the 25th
      International Symposium on Software Testing and Analysis, in ISSTA 2016. New
      York, NY, USA: Association for Computing Machinery, Jul. 2016, pp. 354–365.
      doi: 10.1145/2931037.2931040.
    """
    # Preprocess the data (use mutant ID column and ignore some columns)
    grouped_by_row = defaultdict(lambda: [])
    for mutant, comparisons in all_results.items():
        comparison_columns = tuple(comparisons.values())
        grouped_by_row[comparison_columns].append(mutant)

    # Divide into live/duplicate/subsuming mutants
    grouped_by_status = {
        'duplicated': {},
        'alive': [],
        'subsuming': {}
    }
    for comparisons, mutants in grouped_by_row.items():
        if all(cmp == 0 for cmp in comparisons):
            grouped_by_status['alive'].extend(mutants)
        elif len(mutants) > 1:
            grouped_by_status['duplicated'][str(comparisons)] = mutants
        else:
            grouped_by_status['subsuming'][comparisons] = mutants.pop()

    # Greedily compute a set of subsuming mutants (may not be the only one)
    subsuming = grouped_by_status['subsuming']
    maximalSubsuming = {}
    while subsuming:
        mostSubsumedMutants = []
        mostSubsumingMutant = None
        for comparisons, mutant in subsuming.items():
            subsumedMutants = {
                other_comparisons: other_mutant
                for other_comparisons, other_mutant in subsuming.items()
                if subsumes(comparisons, other_comparisons)
            }
            if len(subsumedMutants) > len(mostSubsumedMutants):
                mostSubsumedMutants = subsumedMutants
                mostSubsumingMutant = (comparisons, mutant)

        maximalSubsuming[str(mostSubsumingMutant[0])] = {
            'mutant': mostSubsumingMutant[1],
            'subsumedMutants': {
                str(other_comparisons): other_mutant
                for other_comparisons, other_mutant in subsumedMutants.items()
                if other_mutant != mostSubsumingMutant[1]
            }
        }
        for subsumedMutant in mostSubsumedMutants.keys():
            del subsuming[subsumedMutant]

    grouped_by_status['subsuming'] = maximalSubsuming
    return grouped_by_status


def compute_subsuming_from_file(f_csv, delimiter):
    reader = csv.DictReader(f_csv, delimiter=delimiter)
    return compute_subsuming(reader)


def read_all_csvs(csv_files, delimiter, mutant_column, exclude_columns):
    """
    Reads one or more CSVs in this format:

      Mutant;test1;test2;...;testN;excluded1;...;excludedM
      mut-op-loc;1;0;...;0;abc;...;xyz
    
    Produces a map of this form:

      {"mut-op-loc": {"0-test1": 1, "0-test2": 0, ..., "0-testN": 0}}
    
    Test entries will be prefixed with the position of the CSV file they
    came from.
    """
    all_results = defaultdict(lambda: {})
    for i_file, csv_file in enumerate(csv_files):
        with open(csv_file, 'r') as f_csv:
            reader = csv.DictReader(f_csv, delimiter=delimiter)
            for row in reader:
                all_results[row[mutant_column]].update({
                    '{}-{}'.format(i_file, k): int(v)
                    for k, v in row.items()
                    if k != mutant_column and k not in exclude_columns
                })

    return all_results

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog='subsumption.py',
        description='Classifies mutants into duplicate, alive, and subsuming mutants'
    )
    parser.add_argument('csv_files', nargs='+')
    parser.add_argument('-d', '--delimiter', type=str, default=';')
    parser.add_argument('-o', '--output', type=str, default='output.yaml')
    parser.add_argument('-m', '--mutant-column', type=str, default='Mutant')
    parser.add_argument('-x', '--exclude-column', type=str, nargs='+', default=['Result'])

    args = parser.parse_args()
    all_results = read_all_csvs(args.csv_files,
        delimiter=args.delimiter,
        mutant_column=args.mutant_column,
        exclude_columns=args.exclude_column)

    subsuming = compute_subsuming(all_results)
    with open(args.output, 'w') as f_yaml:
        print(yaml.dump(subsuming), file=f_yaml)

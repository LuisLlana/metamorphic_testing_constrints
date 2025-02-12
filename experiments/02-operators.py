#!/usr/bin/env python

from collections import defaultdict
import csv
import os
import yaml


FLD_OPERATOR = 'operator'
FLD_SUBSUMING = 'subsuming'
FLD_SUBSUMED = 'subsumed'
FLD_DUPLICATED = 'duplicated'
FLD_ALIVE = 'alive'
ALL_FIELDS = [FLD_OPERATOR, FLD_SUBSUMING, FLD_SUBSUMED, FLD_DUPLICATED, FLD_ALIVE]


def operator_from_mutant(mutant_id):
    """
    Extracts the operator from a mutant identifier.

    >>> operator_from_mutant('mut-LT2NE-1')
    'LT2NE'
    """
    return mutant_id.split('-')[1]


def group_by_operator(subsuming):
    counts = defaultdict(lambda: {
        FLD_SUBSUMING: 0,
        FLD_SUBSUMED: 0,
        FLD_DUPLICATED: 0,
        FLD_ALIVE: 0
    })

    for mutant in subsuming['alive']:
        counts[operator_from_mutant(mutant)][FLD_ALIVE] += 1

    for row, mutants in subsuming['duplicated'].items():
        for mutant in mutants:
            counts[operator_from_mutant(mutant)][FLD_DUPLICATED] += 1

    for row, subsuming_info in subsuming['subsuming'].items():
        counts[operator_from_mutant(subsuming_info['mutant'])][FLD_SUBSUMING] += 1
        for subsumed_row, subsumed_mutant in subsuming_info['subsumedMutants'].items():
            counts[operator_from_mutant(subsumed_mutant)][FLD_SUBSUMED] += 1

    return counts

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description='Produces a report of subsuming/subsumed/duplicated mutants per operator'
    )
    parser.add_argument('subsuming_yaml')
    parser.add_argument('-o', '--output', type=str, default='output.csv')

    args = parser.parse_args()
    with open(args.subsuming_yaml) as f_yaml:
        subsuming = yaml.load(f_yaml, Loader=yaml.Loader)
        stats_by_operator = group_by_operator(subsuming)

    with open(args.output, 'w') as f_csv:
        writer = csv.DictWriter(f_csv, fieldnames=ALL_FIELDS)
        writer.writeheader()
        for op, stats in stats_by_operator.items():
            stats[FLD_OPERATOR] = op
            writer.writerow(stats)

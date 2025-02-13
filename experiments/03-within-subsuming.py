#!/usr/bin/env python

from collections import defaultdict
import csv
import os


def count_subsuming(rows):
    return {
        row['operator']: 1 if int(row['subsuming']) > 0 else 0
        for row in rows
    }

def aggregate_subsuming(operator_stats_csv_paths):
    aggregated = defaultdict(lambda: 0)
    for csv_path in operator_stats_csv_paths:
        with open(csv_path, 'r') as f_csv:
            rows = csv.DictReader(f_csv)
            csv_counts = count_subsuming(rows)
            for operator, count in csv_counts.items():
                aggregated[operator] += count

    return aggregated

def write_subsuming_counts(aggregated, output_path):
    with open(output_path, 'w') as f_csv:
        writer = csv.DictWriter(f_csv, fieldnames=['operator', 'count_in_subsuming'])
        writer.writeheader()
        for operator, count in aggregated.items():
            writer.writerow({'operator': operator, 'count_in_subsuming': count})


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description='Produces a report of how many times each operator appears in the subsuming set'
    )
    parser.add_argument('stats_csv', nargs='+')
    parser.add_argument('-o', '--output', type=str, default='output.csv')

    args = parser.parse_args()

    aggregated = aggregate_subsuming(args.stats_csv)
    write_subsuming_counts(aggregated, args.output)

#!/usr/bin/env python

from collections import defaultdict
import csv
import os


def count_subsuming(rows):
    subsuming_operators = set(row['operator'] for row in rows if int(row['subsuming']) > 0)
    return {
        row['operator']: {
            'count': 1 if row['operator'] in subsuming_operators else 0,
            'min_set_size': len(subsuming_operators) if row['operator'] in subsuming_operators else None
        }
        for row in rows
    }

def aggregate_subsuming(operator_stats_csv_paths):
    aggregated = defaultdict(lambda: {
        'count': 0,
        'min_set_size': None
    })
    for csv_path in operator_stats_csv_paths:
        with open(csv_path, 'r') as f_csv:
            rows = list(csv.DictReader(f_csv))
            csv_counts = count_subsuming(rows)
            for operator, stats in csv_counts.items():
                aggregated[operator]['count'] += stats['count']
                if stats['min_set_size']:
                    aggregated[operator]['min_set_size'] = (
                        min(stats['min_set_size'], aggregated[operator]['min_set_size'])
                        if aggregated[operator]['min_set_size']
                        else stats['min_set_size']
                    )

    return aggregated

def write_subsuming_counts(aggregated, output_path):
    with open(output_path, 'w') as f_csv:
        writer = csv.DictWriter(f_csv, fieldnames=['operator', 'count_in_subsuming', 'min_set_size'])
        writer.writeheader()
        for operator, stats in aggregated.items():
            writer.writerow({'operator': operator, 'count_in_subsuming': stats['count'], 'min_set_size': stats['min_set_size']})


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

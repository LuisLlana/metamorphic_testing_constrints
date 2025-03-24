#!/usr/bin/env python

from common import OPERATOR_SETS
from jinja2 import Environment, FileSystemLoader, select_autoescape
import csv
import os

env = Environment(
    loader=FileSystemLoader("templates"),
    autoescape=select_autoescape(),
    trim_blocks=True,
    lstrip_blocks=True
)


def generate_table(data):
    template = env.get_template("within-subsuming-table.tex")
    data_by_operator = {row['operator']: row for row in data if int(row['count_in_subsuming']) > 0}
    return template.render(
        data=data_by_operator,
        opsets=OPERATOR_SETS
    )


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description='Produces a LaTeX table with per-operator statistics on when they produced subsuming mutants'
    )
    parser.add_argument('stats_csv')
    parser.add_argument('-o', '--output', type=str, default='output.tex')
    args = parser.parse_args()

    with open(args.stats_csv) as f_csv:
        data = csv.DictReader(f_csv)
        with open(args.output, 'w') as f_tex:
            print(generate_table(list(data)), file=f_tex)

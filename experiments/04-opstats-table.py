#!/usr/bin/env python

from jinja2 import Environment, FileSystemLoader, select_autoescape
import csv
import os

env = Environment(
    loader=FileSystemLoader("templates"),
    autoescape=select_autoescape(),
    trim_blocks=True,
    lstrip_blocks=True
)

# From the operator table in the paper
OPERATOR_SETS = [
    [
        ["A2S", "S2A"],
        ["A2M", "M2A"],
        ["A2DV", "DV2A"],
        ["M2S", "S2M"],
        ["M2DV", "DV2M"],
        ["S2DV", "DV2S"]
    ],
    [
        ["LE2LT", "LT2LE"],
        ["LE2GT", "GT2LE"],
        ["LE2NE", "NE2LE"],
        ["LE2GE", "GE2LE"],
        ["LE2EQ", "EQ2LE"],
        ["LT2GT", "GT2LT"],
        ["LT2GE", "GE2LT"],
        ["LT2EQ", "EQ2LT"],
        ["LT2NE", "NE2LT"],
        ["GT2EQ", "EQ2GT"],
        ["GT2NE", "NE2GT"],
        ["EQ2NE", "NE2EQ"]
    ],
    [
        ["C2D", "D2C"]
    ],
    [
        ["E2F", "F2E"]
    ],
    [
        ["CSWAP"]
    ]
]

def generate_table(data):
    template = env.get_template("opstats-table.tex")
    data_by_operator = {row['operator']: row for row in data}
    return template.render(data=data_by_operator, opsets=OPERATOR_SETS)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description='Produces a LaTeX table with per-operator statistics'
    )
    parser.add_argument('stats_csv')
    parser.add_argument('-o', '--output', type=str, default='output.tex')
    args = parser.parse_args()

    with open(args.stats_csv) as f_csv:
        data = csv.DictReader(f_csv)
        with open(args.output, 'w') as f_tex:
            print(generate_table(data), file=f_tex)

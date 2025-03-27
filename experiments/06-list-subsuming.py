#!/usr/bin/env python

import os
import yaml

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description='Lists the subsuming mutants in a V8*.yaml file'
    )
    parser.add_argument('yaml_path')
    args = parser.parse_args()

    with open(args.yaml_path) as f_yaml:
        data = yaml.load(f_yaml, Loader=yaml.Loader)
        for row, mutant in data['subsuming'].items():
            print(mutant['mutant'])

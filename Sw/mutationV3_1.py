#!/usr/bin/env python3

import argparse
import re


class StringReplacementMutant:
    def __init__(self, operator, original, start, end, replacement):
        self.operator = operator
        self.original = original
        self.start = start
        self.end = end
        self.replacement = replacement

    def generate(self):
        return self.original[:self.start] + self.replacement + self.original[self.end:]


class StringReplacementOperator:
    def __init__(self, name, regex, replacement):
        self.name = name
        self.regex = regex
        self.replacement = replacement

    def analyze(self, source):
        for m in self.regex.finditer(source):
            yield StringReplacementMutant(
                operator=self,
                original=source,
                start=m.start(),
                end=m.end(),
                replacement=self.replacement
            )


class ArgumentMappingMutant:
    def __init__(self, operator, original, args, arg_mapping):
        self.operator = operator
        self.original = original
        self.args = args
        self.arg_mapping = arg_mapping

    def generate(self):
        mutant = self.original[:self.args[0].start]
        for i, arg in enumerate(self.args):
            if i > 0:
                mutant += ','
            if i in self.arg_mapping.keys():
                arg = self.args[self.arg_mapping[i]]
            mutant += self.original[arg.start:arg.end]
        mutant += self.original[self.args[-1].end:]
        return mutant


class ArgumentRange:
    def __init__(self, start, end):
        self.start = start
        self.end = end

    def __repr__(self):
        return 'ArgumentRange({}:{})'.format(self.start, self.end)


class ArgumentSwappingOperator:
    BALANCING_TOKENS = {'(': ')', '{': '}', '[': ']'}
    RE_TOKENS = re.compile(r'[({[\]}),]')

    def __init__(self, name, function_name, arg_number_a, arg_number_b):
        self.name = name
        self.function_regex = re.compile(r'\b' + function_name + r'\b\s*\(')
        self.arg_number_a = arg_number_a
        self.arg_number_b = arg_number_b

    def analyze(self, source):
        for m_call in self.function_regex.finditer(source):
            token_stack = ['(']
            arg_start = m_call.end()
            args = []
            for m_token in self.RE_TOKENS.finditer(source, pos=m_call.end()):
                token = m_token.group()
                if token == ',':
                    if len(token_stack) == 1:
                        # This is an argument-separating comma
                        args.append(ArgumentRange(start=arg_start, end=m_token.start()))
                        arg_start = m_token.end()
                elif token in self.BALANCING_TOKENS.keys():
                    # opening token - add to stack
                    token_stack.append(token)
                elif token in self.BALANCING_TOKENS.values():
                    # closing token - check if matches our stack top first
                    opening_token = token_stack.pop()
                    if self.BALANCING_TOKENS[opening_token] != token:
                        raise Exception("Closing token '{}' at character {} does not match opening token '{}'".format(
                            token, m_token.start(), opening_token))
                    if len(token_stack) == 0:
                        # end of last argument
                        args.append(ArgumentRange(start=arg_start, end=m_token.start()))
                        yield ArgumentMappingMutant(
                            operator=self, original=source, args=args,
                            arg_mapping={
                                self.arg_number_a: self.arg_number_b,
                                self.arg_number_b: self.arg_number_a
                            }
                        )
                        break


OPERATORS = [
    StringReplacementOperator('E2F', re.compile(r'\bexists\b'), 'forall'),
    StringReplacementOperator('F2E', re.compile(r'\bforall\b'), 'exists'),
    StringReplacementOperator('LE2LT', re.compile(r'<='), '<'),
    StringReplacementOperator('LT2LE', re.compile(r' < '), '<='), 
    StringReplacementOperator('LE2GT', re.compile(r'<='), '>'), 
    StringReplacementOperator('GT2LE', re.compile(r' > '), '<='),
    StringReplacementOperator('LE2NE', re.compile(r'<='), '!='),
    StringReplacementOperator('NE2LE', re.compile(r'!='), '<='),
    StringReplacementOperator('LE2GE', re.compile(r'<='), '>='),  
    StringReplacementOperator('GE2LE', re.compile(r'>='), '<='),
    StringReplacementOperator('LE2EQ', re.compile(r'<='), '=='),
    StringReplacementOperator('EQ2LE', re.compile(r'=='), '<='),
    StringReplacementOperator('LT2GT', re.compile(r' < '), '>'), 
    StringReplacementOperator('GT2LT', re.compile(r' > '), '<'), 
    StringReplacementOperator('LT2GE', re.compile(r' < '),'>='),
    StringReplacementOperator('GE2LT', re.compile(r'>='), '<'), 
    StringReplacementOperator('LT2EQ', re.compile(r' < '),'=='),
    StringReplacementOperator('EQ2LT', re.compile(r'=='), '<'),
    StringReplacementOperator('LT2NE', re.compile(r' < '),'!='),
    StringReplacementOperator('NE2LT', re.compile(r'!='), '<'),
    StringReplacementOperator('GT2EQ', re.compile(r' > '), '=='),
    StringReplacementOperator('EQ2GT', re.compile(r'=='), '>'),
    StringReplacementOperator('GT2NE', re.compile(r' > '), '!='),
    StringReplacementOperator('NE2GT', re.compile(r'!='), '>'),
    StringReplacementOperator('EQ2NE', re.compile(r'=='), '!='),
    StringReplacementOperator('NE2EQ', re.compile(r'!='), '=='),
    StringReplacementOperator('C2D', re.compile(r'/\\'), '\\/'),
    StringReplacementOperator('D2C', re.compile(r'\\/'), '/\\'),
    StringReplacementOperator('A2S', re.compile(r' \+ '), '-'),
    StringReplacementOperator('S2A', re.compile(r' \- '), '+'),
    StringReplacementOperator('A2M', re.compile(r' \+ '), '*'),
    StringReplacementOperator('M2A', re.compile(r' \* '), '+'),
    StringReplacementOperator('A2DV', re.compile(r' \+ '), '/'),
    StringReplacementOperator('DV2A', re.compile(r' \/ '), '+'),
    StringReplacementOperator('M2S', re.compile(r' \* '), '-'),
    StringReplacementOperator('S2M', re.compile(r' \- '), '*'),
    StringReplacementOperator('M2DV', re.compile(r' \* '), '/'),
    StringReplacementOperator('DV2M', re.compile(r' \/ '), '*'),
    StringReplacementOperator('S2DV', re.compile(r' \- '), '/'),
    StringReplacementOperator('DV2S', re.compile(r' \/ '), '-'),
    ArgumentSwappingOperator('CSWAP', 'cumulative', 2, 3),
]


def generate_mutants(path_to_mzn):
    with open(path_to_mzn, 'r', encoding='utf-8') as f_source:
        source = f_source.read()

        for op in OPERATORS:
            for i, mut in enumerate(op.analyze(source)):
                mutant_filename = 'mut-{0}-{1}.mzn'.format(op.name, i + 1)
                with open(mutant_filename, 'w', encoding='utf-8') as f_m:
                    f_m.write(mut.generate())


if __name__ == "__main__":
  parser = argparse.ArgumentParser(
      prog="MuMiniZinc", description="Mutant generation for MiniZinc")
  parser.add_argument("mzn")

  args = parser.parse_args()
  generate_mutants(args.mzn)

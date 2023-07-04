import os
import os.path
import re
import subprocess
import datetime
import sys
from multiprocessing import Pool
import time

TIMEOUT=30
MINIZINC='/opt/minizinc/current/bin/minizinc'
OUTDIR = 'out'

def log(comment):
    now = datetime.datetime.now()
    LOG.write("[{0:%Y-%m-%d %H:%M:%S}] - {1}\n".format(now,comment))



def execute_command(command, stdout=sys.stdout, stderr=sys.stderr):
    log(f'{command} start')
    #_stdout=sys.stdout
    stdout.write(f'{command}\n')
    stdout.flush()
    rcode=subprocess.call(command.split(), stdout=stdout,
                          stderr=stderr,
                          timeout=TIMEOUT)
    log(f'{command} stop')
    return rcode

def execute_test(params):
    model, data = params
    basemodel = os.path.basename(model).strip()
    basedata = os.path.basename(data).strip()
    print(f'processsing {basemodel}--{basedata}')
    outfile = f'{OUTDIR}/{basemodel}--{basedata}.out'
    with open(outfile, 'w' ) as out:
        ini = time.perf_counter()
        try:
            execute_command(f'{MINIZINC} {data.strip()} {model.strip()}', out, out)
            end = time.perf_counter()
            out.write(f'OK\n')
        except subprocess.TimeoutExpired:
            out.write(f'TIMEOUT\n')
        finally:
            out.write(f'total time: {end-ini} ms\n')

def main(model_file, data_file):
    with open(data_file) as dfile:
        datalst = dfile.readlines()
    with open(model_file) as mfile:
        modellst = mfile.readlines()

    with Pool() as pool:
        pool.map(execute_test,
                 [(model, data)
                  for model in modellst for data in datalst])



def sum(x,y):
    print(f'processing sum {x} {y} {x+y}')

def g(t):
    sum(*t)

if __name__ == '__main__':
    if len(sys.argv)<2:
        print(f'usage: python3 {sys.argv[0]} model_file data_file')
        sys.exit(1)
    model_file = sys.argv[1]
    data_file = sys.argv[2]

    with open('tests.log', 'w') as LOG:
        main(model_file, data_file)

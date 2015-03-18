#! /usr/bin/python

import sys
import re

logs = {}
algorithms = []

def main():
    #print("\t")
    for l in sys.argv[1:]:
        author = l[9:]
        author = author.replace(".log", "")
        #print(author)
        log = logs[author] = {}
        log["alg"] = {}
        log["real"] = {}
        log["cores"] = {}
        LOG = open(l)
        for line in LOG:
            r = alginfo(line)
            if r:
                alg = log["alg"][r] = []
                real = log["real"][r] = []
                if r not in algorithms:
                    algorithms.append(r)
                    #print algorithms

            r = times(line)
            if r:
                alg.append(r[1] + r[2])
                real.append(r[0])
                #print alg

            r = cpuinfo(line, "physical id")
            if r != None:
                cpu_id = r
            r = cpuinfo(line, "siblings")
            if r != None:
                if not cpu_id in log["cores"]:
                    log["cores"][cpu_id] = r
                    #print log["cores"]
        LOG.close()


    for a in algorithms:
        print("\t%s" % (a,)),
    print("\tcores")

    for author in logs.keys():
        l = logs[author]
        print("%s" % (author)),
        for a in algorithms:
            print("\t%5.3f" % (sum(l["alg"][a])/len(l["alg"][a]),)),
            #print("\t%5.3f\t%5.3f" % ( sum(l["real"][a])/len(l["real"][a]), sum(l["alg"][a])/len(l["alg"][a])  )),
        print("\t%d" % (sum(l["cores"].values())))



def cpuinfo(line, field):
    r = re.match(re.escape(field) + '\s*:\s*(\d+)', line)
    if r:
        return int(r.group(1))
    return None

def alginfo(line):
    r = re.match(r'-- \[(.+?)\] --', line)
    if r:
        return r.group(1)
    return None

def times(line):
    r = re.match(r'\s*times: +real +0:([\d\.]+), +user +([\d\.]+), +sys ([\d\.]+)', line)
    if not r:
        r = re.match(r'\s*real\s+0m([\d\.]+)s\s+user\s+0m([\d\.]+)s\s+sys\s+0m([\d\.]+)s', line)
    if r:
        times = r.group(1, 2, 3)
        times = [float(x) for x in times]
        return times
    return None

if __name__ == "__main__":
    main()

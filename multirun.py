import argparse
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--threads', type=int, default=1,
                        help="How many threads do you want to run?")
    parser.add_argument('-s', '--isSequential', action='store_true')
    parser.add_argument('-m', '--modify', action='store_true')
    parser.add_argument('--test', action='store_true')
    parser.add_argument('--minlg2', type=int, default=6,
                        help="lower array data size (log2)")
    parser.add_argument('--maxlg2', type=int, default=15,
                        help="big array data size (log2)")
    args = parser.parse_args()

    reps = 0
    raw_output = []
    for i in range(args.minlg2, args.maxlg2+1):
        if i < 12:
            if args.modify:
                reps = 100000
            else:
                reps = 1000000
        elif i < 16:
            if args.modify:
                reps = 10000
            else:
                reps = 100000
        else:
            if args.modify:
                reps = 1000
            else:
                reps = 10000

        if args.test:
            reps = 100

        if args.isSequential:
            SeqOrRand = '-s'
        else:
            SeqOrRand = '-r'

        cpus = [str(g) for g in range(args.threads)]
        cpus = ",".join(cpus)
        cmd = ['taskset', '-c', cpus]

        cmd.extend(['./cachetime', '-t:'+str(args.threads),
                    SeqOrRand, '--reps:'+str(reps), '--log2array:'+str(i)])
        if args.modify:
            cmd.extend(['-m'])
        try:
            print(cmd)
            raw_output.append(subprocess.check_output(cmd))
        except:
            print("Error: running multirun")
            exit(-1)


# cpus = range(threads)
if __name__ == "__main__":
    main()

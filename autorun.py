import subprocess
import itertools


def main():
	SeqOrRand = ['-s', '']
	threads = [str(i+1) for i in range(8)]
	modify = ['-m', '']
	allCombinations = [threads, SeqOrRand, modify]
	combinations = list(itertools.product(*allCombinations))
	for each_combination in combinations:
		cmd = ['python3', 'multirun.py', '--minlg2', str(16), '--maxlg2', str(24), '--test','-t']
		each_comblist = list(each_combination)
		try:
			each_comblist.remove('')
			each_comblist.remove('')
		except ValueError:
			pass
		cmd.extend(each_comblist)
		raw_output = []
		try:
			print(cmd)
			raw_output.append(subprocess.check_output(cmd))
		except:
			print("Error: running autorun")
			exit(-1)

if __name__ == '__main__':
	main()

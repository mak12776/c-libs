
import utils
import io
from printf import printf, printf_error

comment_prefix = ';'

class Error(Exception):
	pass

class CompileError(Error):
	pass

def compile_buffer(buffer):
	return compile_lines(buffer.splitlines())

def compile_lines(lines):
	lnum = 1
	for line in lines:
		line = line.strip(' \t')
		if line:
			line.split(' ')

		lnum += 1

def main(argv):
	program_name = argv[0] if len(argv) != 0 else 'bc.py'
	if len(argv) != 2:
		printf('usage: {} [file]', argv[0])
		return utils.EXIT_NORMAL
	try:
		file = open(argv[1], 'rb', 0)
	except FileNotFoundError as e:
		printf_error('can\'t open \'{}\': {}', argv[1], e)
		return utils.EXIT_ERROR

	buffer = file.read()
	compile_buffer(buffer)

utils.run_main(main)

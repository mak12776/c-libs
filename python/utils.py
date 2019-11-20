
import sys

EXIT_NORMAL = 0
EXIT_ERROR = 1
EXIT_ARGUMENT_ERROR = 2

def run_main(func):
    sys.exit(func(sys.argv) or 0)

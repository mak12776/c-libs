
import sys

def printf(fmt, *args, **kwargs):
    print(fmt.format(*args, **kwargs), end='')

def printf_error(fmt, *args, **kwargs):
    print('error: ' + fmt.format(*args, **kwargs))

def printf_line(fmt, *args, **kwargs):
    print(fmt.format(*args, **kwargs))

def print_sep(char = '-', width = 80):
    print(char * width)

__all__ = ['printf', 'printf_line', 'print_sep']

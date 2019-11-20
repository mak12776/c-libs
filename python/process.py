
import sys
import ctypes
import re
import collections
import io
import json

def printf(fmt, *args, **kwargs):
    print(fmt.format(*args, **kwargs), end='')

def printf_line(fmt, *args, **kwargs):
    print(fmt.format(*args, **kwargs))

def printf_error(fmt, *args, **kwargs):
    print("error:", fmt.format(*args, **kwargs))

def printf_compile_error(file_name, lnum, fmt, *args, **kwargs):
    printf_line('{}:{}: {}', file_name, lnum, fmt.format(*args, **kwargs))
    printf('\t{}', line)

VARIABLE_NAME_PATTERN = '[a-zA-Z_][a-zA-Z_0-9]*'

def is_valid_variable_name(string):
    return re.fullmatch(VARIABLE_NAME_PATTERN, string)

EXIT_NORMAL = 0
EXIT_ERROR = 1

DWORD = ctypes.c_ulong
STD_OUTPUT_HANDLE = -11
INVALID_HANDLE_VALUE = -1
ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004

GetStdHandle = ctypes.windll.kernel32.GetStdHandle
GetConsoleMode = ctypes.windll.kernel32.GetConsoleMode
SetConsoleMode = ctypes.windll.kernel32.SetConsoleMode
GetLastError = ctypes.windll.kernel32.GetLastError

def set_vt_mode(enable):
    handle = GetStdHandle(STD_OUTPUT_HANDLE)
    if handle == INVALID_HANDLE_VALUE or handle == None:
        return False
    mode = DWORD()
    if not GetConsoleMode(handle, ctypes.addressof(mode)):
        return False
    if enable:
        mode.value |= ENABLE_VIRTUAL_TERMINAL_PROCESSING
    else:
        mode.value &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING
    if not SetConsoleMode(handle, mode):
        return False
    return True

PROCESS_BYTE_CODE_FILE_MAGIC = b'PBC.'

BYTE_CODE_WRITE         = b'\x00'       # code, string
BYTE_CODE_WRITE_EVAL    = b'\x01'       # code, string
BYTE_CODE_EXEC          = b'\x02'       # code, string


BYTE_CODE_LOAD          = b'\x03'       # code, name
BYTE_CODE_RUN           = b'\x04'       # code, name
BYTE_CODE_PLACE         = b'\x05'       # code, name
BYTE_CODE_EXEC_FILE     = b'\x06'       # code, name

BYTE_CODE_DEFINE        = b'\x03'       # code, name, value
BYTE_CODE_JUMP_IF       = b'\x04'       # code, condition, address
BYTE_CODE_JUMP_IF_NOT   = b'\x05'       # code, condition, address

BYTE_CODE_SIZE          = 1

MACRO_REQUIRES = '{} macro requires {}'
INVALID_VARIABLE_NAME = 'invalid variable name: {}'
UNKNOWN_MACRO = 'unknown macro: {}'
WITHOUT_PRECEDING = '{} macro without preceding {}'
UNTERMINATED_MACRO = 'unterminated {} macro'


class CompilerError(Exception):
    def __init__(self, lnum, line, fmt, *args, **kwargs):
        super().__init__(fmt.format(*args, **kwargs))
        self.lnum = lnum
        self.line = line

ADDRESS_SIZE = 8
NULL_ADDRESS = b'\x00' * ADDRESS_SIZE
STRING_LENGTH_SIZE = 4

def write_string(string, outfile):
    buffer = string.encode('ascii')
    outfile.write(len(buffer).to_bytes(STRING_LENGTH_SIZE, 'big',
        signed = False))
    outfile.write(buffer)

def read_string(infile):
    buffer = infile.read(STRING_LENGTH_SIZE)
    if len(buffer) != STRING_LENGTH_SIZE:
        raise EOFError('while reading string')
    return infile.read(int.from_bytes(buffer, 'big', signed = False)) \
        .decode('ascii')

def read_address(infile):
    buffer = infile.read(ADDRESS_SIZE)
    if len(buffer) != ADDRESS_SIZE:
        raise EOFError('while reading address')
    return int.from_bytes(buffer, 'big', signed = True)

def compile_file(
    infile,                     # open in "rt" mode
    outfile,                    # open in "rb" mode
    macro_prefix = '//#',
    macro_suffix = '',
    statement_prefix = '//@',
    statement_suffix = '',
    comment_prefix = '//!',
    comment_suffix = '',
    variable_prefix = "#{",
    variable_suffix = "}",
    evaluation_prefix = "@{",
    evaluation_suffix = "}@",
        ):

    variable_evaluation_re = re.compile('{}({}){}|{}(.*){}'.format(
        re.escape(variable_prefix),
        VARIABLE_NAME_PATTERN,
        re.escape(variable_suffix),
        re.escape(evaluation_prefix),
        re.escape(evaluation_suffix),
        ))

    outfile.write(PROCESS_BYTE_CODE_FILE_MAGIC)

    macro_stack = collections.deque()
    write_buffer = bytearray()

    line = infile.readline()
    lnum = 1
    while line:

        striped_line = line.strip(' \t\n')
        if  striped_line.startswith(macro_prefix) and \
            striped_line.endswith(macro_suffix):

            if len(write_buffer) != 0:
                outfile.write(BYTE_CODE_WRITE)
                outfile.write(len(write_buffer).to_bytes(
                    STRING_LENGTH_SIZE, 'big', signed = False))
                outfile.write(write_buffer)

                write_buffer.clear()

            macro, *args = \
                striped_line[   len(macro_prefix): \
                                len(striped_line) - len(macro_suffix)] \
                .strip(' \t').split(maxsplit = 1)
            args = args[0] if args else ''

            if macro == 'define':
                args = args.split(maxsplit = 1)
                if len(args) != 2:
                    raise CompilerError(lnum, line,
                        MACRO_REQUIRES, 'define', 'a name and a definition')
                if not is_valid_variable_name(args[0]):
                    raise CompilerError(lnum, line,
                        INVALID_VARIABLE_NAME, args[0])
                # write define
                outfile.write(BYTE_CODE_DEFINE)
                write_string(args[0], outfile)
                write_string(args[1], outfile)

            elif macro == 'load':
                if not args:
                    raise CompilerError(lnum, line,
                        MACRO_REQUIRES, 'load', 'a file name')
                # write load
                outfile.write(BYTE_CODE_LOAD)
                write_string(args, outfile)

            elif macro == 'exec':
                if not args:
                    raise CompilerError(lnum, line,
                        MACRO_REQUIRES, 'exec', 'a file name')
                # write exec
                outfile.write(BYTE_CODE_EXEC_FILE)
                write_string(args, outfile)

            elif macro == 'end':
                try:
                    last_macro = macro_stack.pop()
                except IndexError:
                    raise CompilerError(lnum, line,
                        WITHOUT_PRECEDING, 'end', 'if or for or while or ...')

                if args:
                    if args != last_macro[0]:
                        raise CompilerError(lnum, line,
                            0 / 0)

                if last_macro[0] == 'if':
                    address = outfile.tell()

                    outfile.seek(last_macro[2], 0)
                    outfile.write((address - last_macro[3]).to_bytes(
                        ADDRESS_SIZE, 'big', signed = True))
                    outfile.seek(address)

                elif last_macro[0] == 'while':

                    outfile.write(BYTE_CODE_JUMP_IF)
                    write_string(last_macro[4], outfile)
                    outfile.write(
                        (last_macro[3] - outfile.tell() - ADDRESS_SIZE) \
                        .to_bytes(ADDRESS_SIZE, 'big', signed = True))

                    address = outfile.tell()

                    outfile.seek(last_macro[2], 0)
                    outfile.write((address - last_macro[3]).to_bytes(
                        ADDRESS_SIZE, 'big', signed = True))
                    outfile.seek(address)

            elif macro == 'if':
                if not args:
                    raise CompilerError(lnum, line,
                        MACRO_REQUIRES, 'if', 'a condition')
                # write if
                outfile.write(BYTE_CODE_JUMP_IF_NOT)
                write_string(args, outfile)

                address = outfile.tell()
                outfile.write(NULL_ADDRESS)

                macro_stack.append(
                    ('if', (lnum, line), address, outfile.tell()) )

            elif macro == 'while':
                if not args:
                    raise CompilerError(lnum, line,
                        MACRO_REQUIRES, 'while', 'a condition')
                # write while

                outfile.write(BYTE_CODE_JUMP_IF_NOT)
                write_string(args, outfile)

                address = outfile.tell()
                outfile.write(NULL_ADDRESS)

                macro_stack.append(
                    ('while', (lnum, line), address, outfile.tell(), args) )

            else:
                raise CompilerError(lnum, line,
                    UNKNOWN_MACRO, macro)

        elif striped_line.startswith(statement_prefix) and \
            striped_line.endswith(statement_suffix):

            exec_string = \
                striped_line[   len(macro_prefix): \
                                len(striped_line) - len(macro_suffix)] \
                .strip(' \t')

            if exec_string:

                if len(write_buffer) != 0:
                    outfile.write(BYTE_CODE_WRITE)
                    outfile.write(len(write_buffer).to_bytes(
                        STRING_LENGTH_SIZE, 'big', signed = False))
                    outfile.write(write_buffer)

                    write_buffer.clear()

                outfile.write(BYTE_CODE_EXEC)
                write_string(exec_string, outfile)

        elif striped_line.startswith(comment_prefix) and \
            striped_line.endswith(comment_suffix):
            pass

        else:
            m = re.search(variable_evaluation_re, line)
            while m:
                span = m.span()
                if span[0] != 0:
                    write_buffer += line[:span[0]].encode('ascii')

                if len(write_buffer) != 0:
                    outfile.write(BYTE_CODE_WRITE)
                    outfile.write(len(write_buffer).to_bytes(
                        STRING_LENGTH_SIZE, 'big', signed = False))
                    outfile.write(write_buffer)

                    write_buffer.clear()

                eval = m.group(1)
                if eval is None:
                    eval = m.group(2)

                # write evaluation
                outfile.write(BYTE_CODE_WRITE_EVAL)
                write_string(eval, outfile)

                line = line[span[1]:]

                if span[1] == len(line):
                    break

                m = re.search(variable_evaluation_re, line)

            else:
                write_buffer += line.encode('ascii')

        line = infile.readline()
        lnum += 1

    if len(write_buffer) != 0:
        outfile.write(BYTE_CODE_WRITE)
        outfile.write(len(write_buffer).to_bytes(
            STRING_LENGTH_SIZE, 'big', signed = False))
        outfile.write(write_buffer)

    try:
        last_macro = macro_stack.pop()
    except IndexError:
        pass
    else:
        raise CompilerError(*last_macro[1],
            UNTERMINATED_MACRO, last_macro[0])

class FileStructureError(Exception):
    def __init__(self, fmt, *args, **kwargs):
        super().__init__(fmt.format(*args, **kwargs))

class ProcessError(Exception):
    def __init__(self, fmt, *args, **kwargs):
        super().__init__(fmt.format(*args, **kwargs))

def process_byte_code(
        infile,
        outfile,
        variables = None,
        outfile_variable_name = '__outfile__',
        ):
    buffer = infile.read(len(PROCESS_BYTE_CODE_FILE_MAGIC))
    if buffer != PROCESS_BYTE_CODE_FILE_MAGIC:
        raise FileStructureError('file magic number mismatch: {}', buffer)

    if variables is None:
        variables = {}

    if outfile_variable_name not in variables:
        pass

    buffer = infile.read(BYTE_CODE_SIZE)
    while buffer:
        if buffer == BYTE_CODE_WRITE:
            string = read_string(infile)
            outfile.write(string)

        elif buffer == BYTE_CODE_WRITE_EVAL:
            string = read_string(infile)
            outfile.write(str(eval(string, variables)))

        elif buffer == BYTE_CODE_EXEC:
            string = read_string(infile)
            exec(string, variables)

        elif buffer == BYTE_CODE_LOAD:
            string = read_string(infile)
            string = eval(string, variables)
            if not isinstance(string, str):
                raise ProcessError('load macro argument must be type of str')
            try:
                with open(string) as json_file:
                    try:
                        variables.update(json.load(json_file))
                    except json.JSONDecodeError as e:
                        raise ProcessError(
                            'decoding json file \'{}\' raise error: {}', e)
            except FileNotFoundError:
                raise ProcessError('json file \'{}\' not found', string)

        elif buffer == BYTE_CODE_EXEC_FILE:
            string = read_string(infile)
            string = eval(string, variables)
            if not isinstance(string, str):
                raise ProcessError('exec macro argument must be type of str')
            try:
                with open(string) as exec_file:
                    try:
                        exec(exec_file.read())
                    except Exception as e:
                        raise ProcessError(
                            'python file \'{}\' raise exception: {}', e)
            except FileNotFoundError:
                raise ProcessError('python file \'{}\' not found', string)

        elif buffer == BYTE_CODE_DEFINE:
            name = read_string(infile)
            value = read_string(infile)
            variables[name] = eval(value, variables)

        elif buffer == BYTE_CODE_JUMP_IF:
            test = read_string(infile)
            address = read_address(infile)
            if bool(eval(test, variables)):
                infile.seek(address, 1)

        elif buffer == BYTE_CODE_JUMP_IF_NOT:
            test = read_string(infile)
            address = read_address(infile)
            if not bool(eval(test, variables)):
                infile.seek(address, 1)

        else:
            raise FileStructureError('unknown byte code: {}', buffer)

        buffer = infile.read(BYTE_CODE_SIZE)

def main(argv):
    program_name = argv[0] if argv else "frocess.py"
    if len(argv) < 2:
        printf_line('usage: python3 {} [OPTION]... [FILE]...', program_name)
        return 0

    for name in argv[1:]:
        try:
            infile = open(name, "rt")
        except FileNotFoundError as e:
            printf_error("can't open '{}': {}", name, e.strerror)
            return EXIT_ERROR

        with open('temp', 'wb') as temp_file:
            compile_file(infile, temp_file)
        with open('temp', 'rb') as temp_file:
            process_byte_code(temp_file, sys.stdout)

# design:

#   //@ define <name> <value>

#   //@ load <filename>

#   //@ place <filename>

#   //@ include <filename>

#   //@ function <name>( <args> ... )
#   //@ end

#   //@ call <name> [ <args> ... ]

#   //@ for <name> <iterable>
#   //@ end

#   //@ while <cond>
#   //@ end

#   //@ break
#   //@ continue

#   //@ if <cond>
#   //@ elif <cond>
#   //@ else
#   //@ end [ if | for | while ]

if __name__ == '__main__':
    sys.exit(main(sys.argv))

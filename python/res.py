
import sys
import io

def read_file_name(name):
    with open(name, 'rb', 0) as file:
        return read_file(file)

def get_file_size(file):
    file.seek(0, io.SEEK_END)
    size = file.tell()
    file.seek(0, io.SEEK_SET)

def read_file(file):
    size = get_file_size(file)
    buffer = bytearray(size)
    
    read_number = file.readinto(buffer)
    if read_number != size:
        raise EOFError(format('while reading: {}', file.name))

    return buffer

NL = ord('\n')
CR = ord('\r')
SP = ord(' ')
TB = ord('\t')
KEY_a = ord('a')
KEY_z = ord('z')

class Line:
    __slots__ = ['buffer', 'start', 'end']
    def __init__(self, buffer, start, end, num):
        self.buffer = buffer
        self.start = start
        self.end = end
        self.num = num

def read_lines(file):
    buffer = read_file(file)
    start, end = 0, 0
    while end < len(buffer):
        if buffer[end] == CR:
            end += 1
            buffer[end] = NL
            yield buffer, start, end

            if end == len(buffer):
                break
            if buffer[end] == NL:
                end += 1
            start = end

        elif buffer[end] == NL:
            end += 1
            yield buffer, start, end
            start = end

        else:
            end += 1

class AnalyzerError(Exception):
    def __init__(self, fmt, *args, **kwargs):
        super().__init__(fmt.format(*args, **kwargs))

class Token:
    __slots__ = ('type', 'data', 'good')
    def __init__(self, type, data):
        self.type = type
        self.data = data

TOKEN_TYPE_NEWLINE = 1
TOKEN_TYPE_FROM = 2
TOKEN_TYPE_AS = 3

TOKEN_TYPE_GOTO = 4
TOKEN_TYPE_LABEL = 5

def analyze_file(file):
    for buffer, start, end in read_lines(file):
        index = start
        while index < end:
            if buffer[index] == SP or buffer[index] == TB:
                pass
            elif KEY_a <= buffer[index] <= KEY_z:
                index += 1
                while index < end:
                    pass
            else:
                raise AnalyzerError('unknown character: {}', buffer[index])
            index += 1

def main(argv):
    for name in argv[1:]:
        infile = open(name, 'rb', 0)
        for item in analyze_file(infile):
            print(item)


if __name__ == '__main__':
    sys.exit(main(sys.argv))

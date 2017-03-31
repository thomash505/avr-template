import os
import sys

source_dir = sys.argv[1]

includes = '#include "application.h"\n'
init_functions = ''
for arg in sys.argv[2:]:
    path, module = os.path.split(arg)
    includes += '#include "' + os.path.join(module, module + '.h') + '"\n'
    init_functions += '    ' + module + '::init();\n'

f = open(os.path.join(source_dir, 'application.cpp'), 'w+')

f.write('// THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT\n' + includes + '\nvoid application::init()\n{\n' + init_functions + '}')

f.close()

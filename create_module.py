import os
import sys

module_name = sys.argv[1]
module_root = 'module'

includes = '#include "application.h"\n'
init_functions = ''
for arg in sys.argv[2:]:
    path, module = os.path.split(arg)
    includes += '#include "' + os.path.join(module, module + '.h') + '"\n'
    init_functions += '    ' + module + '::init();\n'


module_dir = os.path.join(module_root, module_name)

if os.path.exists(module_dir):
    print 'The module: "' + module_name + '" already exists!\nAborting.'
else:
    os.makedirs(module_dir)

    f = open(os.path.join(module_dir, 'CMakeLists.txt'), 'w+')
    f.write('file(GLOB_RECURSE ' + module_name + '_source "*.c" "*.cpp" "*.hpp")\n\nadd_library(' + module_name + ' STATIC ${' + module_name + '_source})\ninclude_directories("module/' + module_name + '")')
    f.close()

    f = open(os.path.join(module_dir, module_name + '.h'), 'w+')
    f.write('#ifndef ' + module_name.upper() + '_H\n#define ' + module_name.upper() + '_H\n\nnamespace ' + module_name + ' {\n\nvoid init();\n\n}\n\n#endif // ' + module_name.upper() + '_H')
    f.close()

    f = open(os.path.join(module_dir, module_name + '.cpp'), 'w+')
    f.write('#include "' + module_name + '.h"\n\nvoid ' + module_name + '::init()\n{\n}')
    f.close()

    print 'The module: "' + module_name + '" has been created.\nPlease run CMake to register the new files.'

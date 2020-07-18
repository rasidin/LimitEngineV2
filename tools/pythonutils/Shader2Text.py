import sys
import os
import subprocess

def printLogo():
    print('Shader2Text for LimitEngine')
    print('coded by minseob')

def printUsage():
    print('Usage : shader2text [ShaderFile] [OutTextFile]')
    print('Options : ')
    print('-T : vs, ps, cs')
    print('-E : entry')
    print('-I : include path')
    print('-D : Definition')
    print('-C : Compiler file path (default = C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x86\\fxc.exe)')

def getArguments(argv):
    output = {};
    for arg in argv:
        if arg[0] == '-':
            argtoken = arg.split('=')
            if len(argtoken) > 2:
                output[argtoken[0]] = argtoken[1] + '=' + argtoken[2]
            else:
                output[argtoken[0]] = argtoken[1]
        elif 'in' not in output:
            output['in'] = arg
        elif 'out' not in output:
            output['out'] = arg

    return output;

def compileShader(args, tempfilepath):
    command = [args['-C']]
    if args['-T'] == 'vs':
        command.append('/T')
        command.append('vs_5_0')
        if '-E' not in args:
            command.append('/E')
            command.append('vs_main')
    elif args['-T'] == 'ps':
        command.append('/T')
        command.append('ps_5_0')
        if '-E' not in args:
            command.append('/E')
            command.append('ps_main')
    elif args['-T'] == 'cs':
        command.append('/T')
        command.append('cs_5_0')
        if '-E' not in args:
            command.append('/E')
            command.append('cs_main')
    if '-E' in args:
        command.append('/E' + args['-E'])
    if '-I' in args:
        command.append('/I' + args['-I'])
    if '-D' in args:
        command.append('/D' + args['-D'])
    if '-O' in args:
        command.append('/O' + args['-O'])
    command.append('/Fo')
    command.append(tempfilepath)
    command.append(args['in'])
    print('Run : ' + ' '.join(command))
    subprocess.call(command)

def convertBinToText(args, tempfilepath):
    print('Load -> %s'%tempfilepath)
    f = open(tempfilepath, 'rb')
    bin = f.read()
    f.close()

    fo = open(args['out'], 'w')
    for idx in range(len(bin)):
        if idx > 0:
            fo.write(', ')
        fo.write('0x%02x'%(bin[idx]))

    os.remove(tempfilepath)

def main():
    printLogo()
    args = getArguments(sys.argv[1:])
    if 'in' not in args or 'out' not in args or '-T' not in args:
        printUsage()
        return
    if '-C' not in args:
        args['-C'] = 'C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x86\\fxc.exe'
    
    print(args)

    tempfilepath = args['out'] + '.temp.obj'
    compileShader(args, tempfilepath)

    if os.path.exists(tempfilepath):
        convertBinToText(args, tempfilepath)

main()
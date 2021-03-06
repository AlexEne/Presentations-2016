import os.path
import subprocess
import json
import csv
import time

vspaths = [dict(name="vs2015", path="C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\VC\\vcvarsall.bat"),
           dict(name="vs2012", path="C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\VC\\vcvarsall.bat"),
           dict(name="vs2010", path="C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\VC\\vcvarsall.bat")
           ]


class Result:
    def __init__(self):
        self.values = []
        self.params = None

    def setParams(self, params):
        self.params = params

    def add(self, pstring, prm):
        lines = pstring.splitlines(True)
        for line in lines:
            res = line.split(':')
            mili = res[1].split('ms')[0]
            self.values.append(dict(descr=res[0], val=mili, params=prm))


def get_compiler():
    for pt in vspaths:
        if os.path.isfile(pt["path"]):
            print "Using compiler:" + pt["name"]
            return pt["path"]
    print "ERROR !!NO vcvarsall.bat found!"
    return None


def get_build_bat(vcfile, defines, filename, results):
    filename_noexe = os.path.splitext(filename)[0]
    time.sleep(0.5)
    if os.path.isfile(filename_noexe + '.exe'):
        os.remove(filename_noexe + '.exe')
    if os.path.isfile(filename_noexe + '.obj'):
        os.remove(filename_noexe + '.obj')

    filebat = open("buildFile.bat", 'w')
    filebat.write('call "' + vcfile + '"' + '\n')

    filebat.write("cl ")
    filebat.write(" /EHsc ")
    for df in defines:
        df = df.rstrip()
        df = df.lstrip()
        df = df.strip('\"')
        df = df.replace('\"', "")
        filebat.write(' /D' + df)
    filebat.write(' /DPYTHON_TESTER /O2 ')
    pdbname = filename_noexe + '.pdb'
    filebat.write("/GS /GL /analyze- /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl /Fd\"+pdbname+\" /Zc:inline /fp:precise /D \"_MBCS\" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi /MD /EHsc ")

    filebat.write(filename)
    filebat.close()

    process = subprocess.Popen("buildFile.bat", shell=True, bufsize=1024, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    process.wait()
    stdout_value = process.communicate()[0]
    print 'Build Output:' + stdout_value

    process = subprocess.Popen(filename_noexe + '.exe', shell=True, bufsize=1024, stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE)
    process.wait()
    stdout_value = process.communicate()[0]
    print 'Run Output:' + stdout_value
    if filename not in results:
        results[filename] = Result()

    results[filename].add(stdout_value, defines)


def run_exe(filename):
    time.sleep(1.0)
    compilerEnviroment = get_compiler()
    first_command = '"' + compilerEnviroment + '"'
    second_command = '/DPYTHON_TESTER ' + filename
    fullcmd = 'buildFile.bat ' + first_command + ' ' + second_command

    process = subprocess.Popen(fullcmd, shell=True, bufsize=1024, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    stdout_value = process.communicate('"' + compilerEnviroment + '" \n')[0]
    print 'Build Output:' + stdout_value


file = open('testerData.txt', 'r+')
reader = csv.reader(file)
vscompiler = get_compiler()

results = dict()
for line in reader:
    cppfilename = line[0].rstrip() + '.cpp'
    print 'building :' + cppfilename
    line.pop(0)
    get_build_bat(vscompiler, line, cppfilename, results)

# ugly hack to get results arranged for a csv file.
for key, res in results.iteritems():
    print 'results for: ' + key
    i = 0
    c = 0
    for val in res.values:
        if i % 4 == 0:
            print ''
            print c * 5000,
            c += 1
        i += 1
        print str(val['val']),

    print ''
    print '---------'

import os.path
import subprocess

vspaths =[dict(name="vs2015",path="C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\VC\\vcvarsall.bat"),
          dict(name="vs2012",path="C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\VC\\vcvarsall.bat"),
          dict(name="vs2010",path="C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\VC\\vcvarsall.bat")
          ]
def getCompiler():
    current_vc_path = "NONE"
    for pt in vspaths:
        if os.path.isfile(pt["path"]):
            current_vc_path = pt["path"]
            print "Using compiler:"+ pt["name"]
            return pt["path"]
    print "ERROR !!NO vcvarsall.bat found!"
    return current_vc_path

def buildExeFromFile(filename):
    compilerEnviroment = getCompiler()
    first_command = "\""+compilerEnviroment+"\""
    second_command = filename
    fullcmd = "buildFile.bat "+first_command+" "+ second_command

    process  = subprocess.Popen(fullcmd, shell=True, bufsize=1024, stdin=subprocess.PIPE,stdout=subprocess.PIPE)

    stdout_value = process.communicate("\""+compilerEnviroment+"\" \n")[0]
    print "Build Output:"+ stdout_value

file = open("programs.txt", "r+")
lines = file.readlines()
file.close()
for line in lines:
    cppfilename = line.rstrip() + ".cpp"
    print "building :"+ cppfilename
    buildExeFromFile(cppfilename)



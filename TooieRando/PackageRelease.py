import os
import sys
import shutil, errno
FilesList=[]
TargetFolder = os.path.join((os.getcwd()), sys.argv[1])

for file in os.listdir(os.getcwd()):
    if file.endswith(".txt"):
        if("UsedSeeds" in file or "SpoilerLog" in file):
            continue
        FilesList.append(os.path.join((os.getcwd()), file))
        continue
    if os.path.isdir(file) and ("xdelta" in file or "patch" in file or "Logic" in file):
        FilesList.append(os.path.join((os.getcwd()), file))
        
for file in os.listdir(os.path.join((os.getcwd()), "Release")):
    if file.endswith(".obj")or file.endswith(".exe")or file.endswith(".pdb") or file.endswith(".pch")or file.endswith(".EXE"):
        FilesList.append(os.path.join(os.path.join((os.getcwd()), "Release"),file))
os.makedirs(TargetFolder,exist_ok=True)
os.makedirs(os.path.join(TargetFolder, "output"),exist_ok=True)

for path in FilesList:
    if os.path.isdir(path):
        dest = os.path.join(TargetFolder, os.path.basename(path))
        shutil.copytree(path, dest, dirs_exist_ok=True)
    else:
        shutil.copy(path, TargetFolder)
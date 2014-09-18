d:
cd d:\appwre32\environ
ss get $/appware32/environ/*.* -r > %1
cd d:\appwre32\alms
ss get $/appware32/alms/*.* -r >> %1

cd d:\appwre32
sh d:\appwre32\scripts\dofiles d:


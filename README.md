TLT (Trunk Line And International Talks)
======================================

This software package was developed for automating of technological processes in communication's branches and hotels connected to automatic long-distance telephone station quasi-electronic system type "*Quartz*" of *UkrTelecom*, *Ukraine*, *Zhitomir* (some modified version, *TLT-H*, has been deployed to *Kazakhstan*, *Karaganda*), and was written in *C* and *Assembler* programming languages.

This application was successfully running 24x7 starting from 1992 till 2001 (at least as far I know, but may be later).

Conversion from original *cp866* encoding (Russian) to *utf-8* has been executed as below (except binary files):
```
for f in ./*; do iconv --from-code cp866 --to-code utf-8 $f -o $f.new && mv $f.new $f; done
```

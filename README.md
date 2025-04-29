# millefiori
Sample code of using inflection with MF2

Need to first build and install ICU and inflection


in inflection library
```
make install DESTDIR=~/inflection
```

In this code

Build
```
export ICU_ROOT=~/icu/icu/icu4c/
export INFLECTION_ROOT=~/inflection
make
```

Run
```
LD_LIBRARY_PATH=$ICU_ROOT/source/lib:$INFLECTION_ROOT/usr/local/lib ./main > out.txt
```

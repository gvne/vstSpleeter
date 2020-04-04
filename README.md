# vst Spleeter: A VST interface to Spleeter
# Note: This project has only been tested on OSx 10.14 and 10.15

This project is uses [JUCE](https://juce.com/) and
[spleeterpp](https://github.com/gvne/spleeterpp) to make a simple application
that runs [spleeter](https://github.com/deezer/spleeter) in c++.

## Build:

Clone spleeterpp and build the install target in Release mode.  
To generate the solution do:
```
SPLEETERPP_INSTALL_DIR=...
cmake -GXcode -DCMAKE_INSTALL_PREFIX=$SPLEETERPP_INSTALL_DIR -Drtff_use_mkl=ON ..
```

In spleeter-vst project root, create a symlink to the install folder:
```
bash configure.sh $SPLEETERPP_INSTALL_DIR
```

Then, build it with JUCE !

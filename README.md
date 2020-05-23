# vst Spleeter: A VST interface to Spleeter
# Note: This project has only been tested on OSx 10.14 and 10.15

This project is uses [JUCE](https://juce.com/) and
[spleeterpp](https://github.com/gvne/spleeterpp) to make a simple application
that runs [spleeter](https://github.com/deezer/spleeter) in c++.

# Build:

## OSX

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
xcopy "external/spleeterpp/build/install/models/filter" "C:\Users\guillaume\taff\perso\vstSpleeter\Builds\VisualStudio2019\x64\Release\Shared Code\models" /s /e
if not exist "test" mkdir "test"
## Windows

* Clone Eigen in the external folder
* Build spleeterpp in the external folder
* Install the external/spleeterpp/build/install/lib/tensorflow.dll in the host application search path. The easiest is to copy it in the system directory (%SystemRoot%\\system32)
* Install the [VST3](https://www.steinberg.net/vst3sdk) sdk in C:/SDKs
* Open the spleeter-vst.jucer project and start the visual studio generator
* Build with visual studio in Release mode

For the first two steps, you can run the following script:
```bash
mkdir external && cd external
git clone https://github.com/eigenteam/eigen-git-mirror.git eigen
cd eigen && git checkout 3.3.7 && cd -

git clone https://github.com/gvne/spleeterpp.git && cd spleeterpp
git checkout 586a5dc503c73fc4975aca675432a280d6ea860f
mkdir build && cd build
cmake -Drtff_use_mkl=ON -DCMAKE_INSTALL_PREFIX=$(pwd)/install ..
cmake --build . --config Release --target INSTALL
cd ../..

cd ..
```

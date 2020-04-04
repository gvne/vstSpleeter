spleeterpp_install_dir=$1

mkdir -p external
ln -s $spleeterpp_install_dir external/spleeterpp
git clone https://github.com/eigenteam/eigen-git-mirror.git external/eigen
cd external/eigen && git checkout 3.3.7 && cd -

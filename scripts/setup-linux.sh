git submodule update --init --recursive
git submodule foreach git checkout master
cd ext/zorx/ext/orx
./setup.sh
cd code/build/linux/gmake
make config=core_debug64 -j 16
make config=core_release64 -j 16
make config=debug64 -j 16
make config=release64 -j 16

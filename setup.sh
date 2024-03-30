git submodule init
git submodule update
cd orx
./setup.sh
cd code/build/linux/gmake/
make
cd libgff
make
cd ..

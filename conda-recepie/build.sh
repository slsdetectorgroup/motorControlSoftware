mkdir build
mkdir install
cd build
cmake .. \
      -DCMAKE_PREFIX_PATH=$CONDA_PREFIX \
      -DCMAKE_INSTALL_PREFIX=install \
      -DUSE_CLIENT=ON \
      -DUSE_BIG_XRAY=ON \
      -DUSE_LASER=ON \
      -DUSE_VACUUM=ON \
      -DUSE_GUI=ON \
     

cmake --build . -- -j10
cmake --build . --target install
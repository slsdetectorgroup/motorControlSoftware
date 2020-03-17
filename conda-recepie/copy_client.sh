mkdir $PREFIX/bin

cp build/bin/xrayClient $PREFIX/bin/.
cp build/bin/vacuumClient $PREFIX/bin/.
cp build/bin/laserClient $PREFIX/bin/.

cd python
${PYTHON} setup.py install
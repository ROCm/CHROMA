Quick installations instructions for CHROMA
=================================
To build CHROMA we need to install some dependencies like qmp, qdpxx, quda

Build steps for qmp:
1. git clone --recursive https://github.com/usqcd-software/qmp.git
2. cd qmp
3. git checkout qmp2-5-2
4. autoreconf -ivf
5. cd ..
6. mkdir build_qmp
7. cd build_qmp
8. ../qmp/configure --prefix=/home/projects/Chroma/qmp --with-qmp-comms-type=MPI CXX=mpicxx CC=mpicc \
  CFLAGS=-O3 CXXFLAGS="-O3 -std=c++11"
9. make -j4
10. make install 

Build steps for qdpxx:

1. git clone --recursive https://github.com/usqcd-software/qdpxx.git
2. cd qdpxx
3. autoreconf -ifv
4. cd ..
5. mkdir build_qdpxx
6. cd build_qdpxx
7. ../qdpxx/configure  --prefix=/home/projects/Chroma/qdp --with-qmp=/home/projects/Chroma/qmp --enable-parallel-arch=parscalar \
   CC=mpicc CXX=mpicxx \
   CXXFLAGS="-O3 -std=c++11" CFLAGS="-O3"
8. make -j
9. make install

Build steps for quda:

1. mkdir build_qudaChromaeigen
2. cd build_qudaChromaeigen
3. MPICXX=mpicxx MPICC=mpicc CC=gcc CXX=hipcc cmake ../quda-amd -DQUDA_GPU_ARCH=sm_70 -DQUDA_DIRAC_STAGGERED=ON -DQUDA_DIRAC_DOMAIN_WALL=OFF \
      -DQUDA_DIRAC_TWISTED_MASS=OFF -DQUDA_DOWNLOAD_EIGEN=OFF -DQUDA_DIRAC_WILSON=ON -DQUDA_DIRAC_CLOVER=ON \
      -DQUDA_DIRAC_TWISTED_CLOVER=ON -DQUDA_DYNAMIC_CLOVER=ON -DQUDA_DIRAC_CLOVER_HASENBUSCH=ON -DQUDA_LINK_HISQ=OFF \
      -DQUDA_MULTIGRID=ON -DQUDA_DOWNLOAD_USQCD=ON -DQUDA_MPI=OFF -DQUDA_INTERFACE_MILC=OFF \
      -DQUDA_QMP=ON  -DQUDA_QIO=ON \
      -DQUDA_BUILD_SHAREDLIB=ON -DQUDA_BUILD_ALL_TESTS=ON -DQUDA_TEX=OFF -DCMAKE_BUILD_TYPE=DEVEL \
      -DCMAKE_INSTALL_PREFIX=/home/projects/Chroma/qudaeigen \
      -DCMAKE_PREFIX_PATH=/opt/rocm \
      -DCMAKE_CXX_FLAGS="-DEIGEN_USE_HIP -DEIGEN_USE_GPU -I/opt/rocm/hiprand/include -I/opt/rocm/rocrand/include -I/opt/rocm/hipcub/include -I/opt/rocm/rocprim/include"
4. make -j
5. make install

Build steps for CHROMA:
1. git clone --recursive https://github.com/ROCmSoftwarePlatform/CHROMA.git -b develop-hip
2. cd CHROMA
3. autoreconf -i
4. mkdir build_chromanew
5. cd build_chromanew
6. ../CHROMA/configure \
 --prefix=/home/projects/Chroma/chromanew \
 --with-quda=/home/projects/Chroma/qudaeigen \
 --with-qmp=/home/projects/Chroma/qmp \
 --with-qdp=/home/projects/Chroma/qdp \
 CC=mpicc \
 CXX=mpicxx\
 CXXFLAGS="-O3 -std=c++11"
7. make -j
8. make install

To run tests on SingleGPU(CHROMA/build/mainprogs/tests)
***********
mpirun -np 1  --allow-run-as-root t_mesplq -geom 1 1 1 1

To run tests on MultiGPU (CHROMA/build/mainprogs/tests)
*************************
mpirun -np 2 --allow-run-as-root t_mesplq -geom 1 1 1 2

/home/linuxbrew/.linuxbrew/Cellar/cmake/3.11.1/bin/cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" /home/jakemanning/meet-in-middle/
/home/linuxbrew/.linuxbrew/Cellar/cmake/3.11.1/bin/cmake --build ./ --target middle -- -j 4
./middle "$@"
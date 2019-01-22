# CHANGE THE NUMBER OF CORES HERE
core_num=8

sudo="sudo"


# Installing deps....
arch="$(command -v apt)" # Testing see what dist we're on
if [[ $arch != "" ]]; then
    $sudo apt -y install $(cat deps_deb.txt)
else
    $sudo pacman -S --needed $(cat deps_arch.txt)
fi

# Installing OpenCV
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cd opencv
mkdir -p build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D OPENCV_EXTRA_MODULES_PATH=`pwd`/opencv_contrib/modules -D CMAKE_INSTALL_PREFIX=/usr/local ..

make -j$core_num
$sudo make install

cd ../..

# Installing Fann
git clone https://github.com/libfann/fann
cd fann/bin
cmake ..

make -j$core_num
$sudo make install

cd ../..

echo -n "Do you want to remove source files? yes/no >"
read remove

if [[ $remove == "yes" ]]; then
    rm -rf opencv opencv_contrib fann
fi
exit


#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-CLA-applies
#
# MuseScore
# Music Composition & Notation
#
# Copyright (C) 2023 MuseScore BVBA and others
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# For maximum AppImage compatibility, build on the oldest Linux distribution
# that still receives security updates from its manufacturer.

echo "############################## Setup Linux build environment ##############################"
trap 'echo Setup failed; exit 1' ERR

df -h .

BUILD_TOOLS=$HOME/build_tools
ENV_FILE=$BUILD_TOOLS/environment.sh
PACKARCH="x86_64" # call from check_visual_tests.yml without --arch

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --arch) PACKARCH="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

mkdir -p $BUILD_TOOLS

# Let's remove the file with environment variables to recreate it
rm -f $ENV_FILE

if [[ "$PACKARCH" == "armv7l" ]] || [[ "$PACKARCH" == "aarch64" ]]; then
  export DEBIAN_FRONTEND="noninteractive" TZ="Europe/London"
fi

echo "echo 'Setup MuseScore build environment'" >> $ENV_FILE

echo "############################## GET DEPENDENCIES ##############################"

apt_packages=(
# musescore 3 working linux x64 https://github.com/Jojo-Schmitz/MuseScore/blob/919eccce/build/ci/linux/setup.sh
  # apt_packages_basic=(
  file
  git
  # pkg-config # https://github.com/musescore/MuseScore/pull/25609
  software-properties-common # installs `add-apt-repository`
  unzip
  p7zip-full
  # apt_packages_standard=(
  curl
  libasound2-dev 
  libfontconfig1-dev
  libfreetype6-dev
  libfreetype6
  libgl1-mesa-dev
  libjack-dev
  libmp3lame-dev # removed in musescore 4
  libnss3-dev
  libportmidi-dev
  libpulse-dev
  libsndfile1-dev
  make
  portaudio19-dev # removed in musescore 4
  wget
# adapt musescore 4
# https://github.com/musescore/MuseScore/blob/b02a3fc49e37ae5d7a41892add56d36d3ee689d9/buildscripts/ci/linux/setup.sh
# comment out duplicates from musescore 3
  coreutils
  # curl
  # desktop-file-utils # installs `desktop-file-validate` for appimagetool. appimagetool not compiled anymore
  gawk
  # file
  # git
  gpg
  libboost-dev
  libboost-filesystem-dev
  libboost-regex-dev
  libcairo2-dev
  libfuse-dev
  libtool
  libssl-dev
  # pkg-config # https://github.com/musescore/MuseScore/pull/25609
  xxd
  # p7zip-full
  # libasound2-dev 
  # libfontconfig1-dev
  # libfreetype6
  # libfreetype6-dev
  libgcrypt20-dev
  # libgl1-mesa-dev
  libglib2.0-dev
  # libgpgme-dev # install for appimagetool. appimagetool not compiled anymore
  # libjack-dev
  # libnss3-dev
  # libportmidi-dev
  # libpulse-dev
  librsvg2-dev
  # libsndfile1-dev
  # libssl-dev
  # libtool
  # make
  # p7zip-full
  sed
  # software-properties-common # installs `add-apt-repository`
  # unzip
  # wget
  # zsync # installs `zsyncmake` for appimagetool. appimagetool not compiled anymore
  )

# MuseScore compiles without these but won't run without them
apt_packages_runtime=(
# musescore 3 working linux x64 https://github.com/Jojo-Schmitz/MuseScore/blob/919eccce/build/ci/linux/setup.sh
  libcups2
  libdbus-1-3
  libegl1-mesa-dev
  libodbc1
  libpq-dev
  libxcomposite-dev
  libxcursor-dev
  libxi-dev
  libxkbcommon-x11-0
  libxrandr2
  libxtst-dev
  libdrm-dev
# adapt musescore 4
# https://github.com/musescore/MuseScore/blob/b02a3fc49e37ae5d7a41892add56d36d3ee689d9/buildscripts/ci/linux/setup.sh
# comment out duplicates from musescore 3
  # libcups2
  # libdbus-1-3
  # libegl1-mesa-dev
  libgles2-mesa-dev
  # libodbc1
  # libpq-dev
  # libssl-dev
  # libxcomposite-dev
  # libxcursor-dev
  # libxi-dev
  # libxkbcommon-x11-0
  # libxrandr2
  # libxtst-dev
  # libdrm-dev
  libxcb-icccm4
  libxcb-image0
  libxcb-keysyms1
  libxcb-randr0
  libxcb-render-util0
  libxcb-xinerama0
  
  libxcb-xkb-dev
  libxkbcommon-dev
  libopengl-dev
  libvulkan-dev
  )

# backported msuescore 4 arm, unsure if needed in musescore 3
apt_packages_ffmpeg=(
  ffmpeg
  libavcodec-dev 
  libavformat-dev 
  libswscale-dev
  )

apt-get update # no package lists in Docker image
apt-get install -y --no-install-recommends \
  "${apt_packages[@]}" \
  "${apt_packages_runtime[@]}" \
  "${apt_packages_ffmpeg[@]}"

echo "############################## GET QT ##############################"

case "$PACKARCH" in
  x86_64)
    # 5152 second try
      # https://github.com/Jojo-Schmitz/MuseScore/commit/1e58c6a9f6d3971801d530098f813475a1b104fc
      # 5.9 to 5.15
      # Mainly to fix https://musescore.org/en/node/317323 for macOS. but
      # updating the builds for Linux and Windows too.
      # https://github.com/Jojo-Schmitz/MuseScore/commit/3a8e9017f13ad456ca2fa36f71d90bca53482b38
      # 5.15 revert back to 5.9
      # Fix vtests build using a similar method as the mtest.
      # Doesn't work, so back to 5.9.8, for Linux, for now...
    qt_version="5152"
    qt_dir="$BUILD_TOOLS/Qt/${qt_version}"
    if [[ ! -d "${qt_dir}" ]]; then
      mkdir -p "${qt_dir}"
      qt_url="https://s3.amazonaws.com/utils.musescore.org/Qt${qt_version}_gcc64.7z"
      wget -q --show-progress -O qt5.7z "${qt_url}"
      7z x -y qt5.7z -o"${qt_dir}"
    fi
    ;;
  armv7l | aarch64)
    # kitware is the cmake company
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ bionic main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null

    add-apt-repository --yes ppa:theofficialgman/opt-qt-5.15.2-focal-arm
    echo "using ppa:theofficialgman/opt-qt-5.15.2-focal-arm"
    qt_version="5152"
    qt_dir="/opt/qt515"
    apt-get update

    apt_packages_qt=(
      qt515base
      qt515declarative
      qt515quickcontrols
      qt515quickcontrols2
      qt515graphicaleffects
      qt515imageformats
      qt515networkauth-no-lgpl
      qt515remoteobjects
      qt515svg
      qt515tools
      qt515translations
      qt515wayland
      qt515x11extras
      qt515xmlpatterns
      qt515webengine # main/cmakelists.txt L232 QtWebEngineProcess  ? needed ? cause bug ?
      )
    apt-get install -y \
      "${apt_packages_qt[@]}"
    ;;
esac

echo export PATH="${qt_dir}/bin:\${PATH}" >> ${ENV_FILE}
echo export LD_LIBRARY_PATH="${qt_dir}/lib:\${LD_LIBRARY_PATH}" >> ${ENV_FILE}
echo export QT_PATH="${qt_dir}" >> ${ENV_FILE}
echo export QT_PLUGIN_PATH="${qt_dir}/plugins" >> ${ENV_FILE}
echo export QML2_IMPORT_PATH="${qt_dir}/qml" >> ${ENV_FILE}

echo "############################## GET COMPILERS ##############################"

apt-get install -y --no-install-recommends automake

case "$PACKARCH" in
  x86_64)
    gcc_version="9"
    apt-get install -y --no-install-recommends "g++-${gcc_version}"
    update-alternatives \
      --install /usr/bin/gcc gcc "/usr/bin/gcc-${gcc_version}" 40 \
      --slave /usr/bin/g++ g++ "/usr/bin/g++-${gcc_version}"
    echo export CC="/usr/bin/gcc-${gcc_version}" >> ${ENV_FILE}
    echo export CXX="/usr/bin/g++-${gcc_version}" >> ${ENV_FILE}
    gcc-${gcc_version} --version
    g++-${gcc_version} --version
    ;;
  armv7l | aarch64)
    apt-get install -y --no-install-recommends gcc
    apt-get install -y --no-install-recommends g++
    update-alternatives \
      --install /usr/bin/gcc gcc "$(readlink -f "$(which gcc)")" 40 \
      --slave /usr/bin/g++ g++ "$(readlink -f "$(which g++)")"
    echo export CC="$(readlink -f "$(which gcc)")" >> ${ENV_FILE}
    echo export CXX="$(readlink -f "$(which g++)")" >> ${ENV_FILE}
    ;;
esac

case "$PACKARCH" in
  x86_64)
    cmake_version="3.16.0"
    cmake_dir="$BUILD_TOOLS/cmake/${cmake_version}"
    if [[ ! -d "$cmake_dir" ]]; then
      mkdir -p "$cmake_dir"
      cmake_url="https://cmake.org/files/v${cmake_version%.*}/cmake-${cmake_version}-linux-x86_64.tar.gz" 
      wget -q --show-progress --no-check-certificate -O - "${cmake_url}" | tar --strip-components=1 -xz -C "${cmake_dir}"
    fi
    export PATH="$cmake_dir/bin:$PATH"
    echo export PATH="$cmake_dir/bin:\${PATH}" >> ${ENV_FILE}
    ;;
  armv7l | aarch64)
    apt-get install -y --no-install-recommends cmake
    ;;
esac
cmake --version

echo export CFLAGS="-Wno-psabi" >> ${ENV_FILE}
echo export CXXFLAGS="-Wno-psabi" >> ${ENV_FILE}

echo "############################## POST INSTALL ##############################"

chmod +x "$ENV_FILE"

if [[ "$PACKARCH" == "armv7l" ]]; then
  # add an exception for the "detected dubious ownership in repository" (only seen inside a Docker image)
  git config --global --add safe.directory /MuseScore
fi

df -h .
echo "Setup script done"

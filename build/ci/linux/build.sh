# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
echo "Build Linux MuseScore AppImage"

#set -x
trap 'echo Build failed; exit 1' ERR

df -h .

BUILD_TOOLS=$HOME/build_tools
ARTIFACTS_DIR=build.artifacts
CRASH_REPORT_URL=""
BUILD_MODE=""
SUFFIX="" # appended to `mscore` command name to avoid conflicts (e.g. `mscoredev`)

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -n|--number) BUILD_NUMBER="$2"; shift ;;
        --crash_log_url) CRASH_REPORT_URL="$2"; shift ;;
        --build_mode) BUILD_MODE="$2"; shift ;;
        --arch) PACKARCH="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ -z "$BUILD_NUMBER" ]; then echo "error: not set BUILD_NUMBER"; exit 1; fi
if [ -z "$BUILD_MODE" ]; then BUILD_MODE=$(cat $ARTIFACTS_DIR/env/build_mode.env); fi

MUSE_APP_BUILD_MODE=dev

case "${BUILD_MODE}" in
"devel")   MUSE_APP_BUILD_MODE=dev; SUFFIX=dev;;
"nightly") MUSE_APP_BUILD_MODE=dev; SUFFIX=nightly;;
"testing") MUSE_APP_BUILD_MODE=testing; SUFFIX=testing;;
"stable")  MUSE_APP_BUILD_MODE=release; SUFFIX="";;
"mtests")  MUSESCORE_BUILD_CONFIG=dev; BUILDTYPE=installdebug; OPTIONS="USE_SYSTEM_FREETYPE=ON UPDATE_CACHE=FALSE PREFIX=$ARTIFACTS_DIR/software";;
esac

echo "MUSE_APP_BUILD_MODE: $MUSE_APP_BUILD_MODE"
echo "BUILD_NUMBER: $BUILD_NUMBER"
echo "CRASH_REPORT_URL: $CRASH_REPORT_URL"
echo "BUILD_MODE: $BUILD_MODE"

echo "=== ENVIRONMENT === "

cat $BUILD_TOOLS/environment.sh
source $BUILD_TOOLS/environment.sh

# disable update module due to current broken functionality
if [ "$PACKARCH" == "aarch64" ] || [ "$PACKARCH" == "armv7l" ]; then
  MUSESCORE_BUILD_UPDATE_MODULE="OFF"
fi

# TODO: https://github.com/musescore/MuseScore/issues/11689
BUILD_VST=OFF

echo "=== BUILD ==="

MUSESCORE_REVISION=$(git rev-parse --short=7 HEAD)

# Build portable AppImage
MUSE_APP_BUILD_MODE=$MUSE_APP_BUILD_MODE \
MUSE_APP_INSTALL_SUFFIX=$SUFFIX \
MUSESCORE_BUILD_NUMBER=$BUILD_NUMBER \
MUSESCORE_REVISION=$MUSESCORE_REVISION \
MUSESCORE_CRASHREPORT_URL=$CRASH_REPORT_URL \
MUSESCORE_BUILD_VST_MODULE=$BUILD_VST \
MUSESCORE_BUILD_CRASHPAD_CLIENT=${MUSESCORE_BUILD_CRASHPAD_CLIENT:-"ON"} \
MUSESCORE_BUILD_UPDATE_MODULE=${MUSESCORE_BUILD_UPDATE_MODULE:-"ON"} \
MUSESCORE_BUILD_WEBSOCKET="ON" \

# bash ./ninja_build.sh -t appimage
#   
  JOBS=4
  TARGET=release

  MUSESCORE_INSTALL_DIR=${MUSESCORE_INSTALL_DIR:-"../build.install"}
  MUSESCORE_INSTALL_SUFFIX=${MUSESCORE_INSTALL_SUFFIX:-""}
  MUSESCORE_LABEL=${MUSESCORE_LABEL:-""}
  MUSESCORE_BUILD_CONFIG=${MUSESCORE_BUILD_CONFIG:-"dev"}
  MUSESCORE_BUILD_NUMBER=${MUSESCORE_BUILD_NUMBER:-"12345678"}
  MUSESCORE_REVISION=${MUSESCORE_REVISION:-"abc123456"}
  MUSESCORE_RUN_LRELEASE=${MUSESCORE_RUN_LRELEASE:-"OFF"}
  MUSESCORE_BUILD_PORTABLEAPPS=${MUSESCORE_BUILD_PORTABLEAPPS:-"OFF"}
  MUSESCORE_CRASHREPORT_URL=${MUSESCORE_CRASHREPORT_URL:-""}
  MUSESCORE_BUILD_CRASHPAD_CLIENT=${MUSESCORE_BUILD_CRASHPAD_CLIENT:-"ON"}
  MUSESCORE_BUILD_UPDATE_MODULE=${MUSESCORE_BUILD_UPDATE_MODULE:-"ON"}
  MUSESCORE_DEBUGLEVEL_ENABLED="OFF"
  MUSESCORE_BUILD_JACK=${MUSESCORE_BUILD_JACK:-"OFF"}
  MUSESCORE_BUILD_VST=${MUSESCORE_BUILD_VST:-"OFF"}
  MUSESCORE_VST3_SDK_PATH=${MUSESCORE_VST3_SDK_PATH:-""}
  MUSESCORE_DOWNLOAD_SOUNDFONT=${MUSESCORE_DOWNLOAD_SOUNDFONT:-"ON"}
  MUSESCORE_BUILD_UNIT_TESTS=${MUSESCORE_BUILD_UNIT_TESTS:-"OFF"}
  MUSESCORE_NO_RPATH=${MUSESCORE_NO_RPATH:-"OFF"}
  MUSESCORE_YOUTUBE_API_KEY=${MUSESCORE_YOUTUBE_API_KEY:-""} 
  MUSESCORE_BUILD_VIDEOEXPORT=${MUSESCORE_BUILD_VIDEOEXPORT:-"OFF"}

  SHOW_HELP=0
  while [[ "$#" -gt 0 ]]; do
      case $1 in
                                          -t|--target) TARGET="$2"; shift;;
          -j|--jobs) JOBS="$2"; shift;;
                                          -h|--help) SHOW_HELP=1; shift;;
          *) echo "Unknown parameter passed: $1"; exit 1 ;;
      esac
      shift
  done

  if [ $SHOW_HELP -eq 1 ]; then
                      echo "TODO..."
                      exit 0
  fi

  cmake --version
  echo "ninja version $(ninja --version)"

  function do_build() {

    BUILD_TYPE=$1

    cmake .. -GNinja \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_INSTALL_PREFIX="${MUSESCORE_INSTALL_DIR}" \
        -DMSCORE_INSTALL_SUFFIX="${MUSESCORE_INSTALL_SUFFIX}" \
        -DMUSESCORE_LABEL="${MUSESCORE_LABEL}" \
        -DMUSESCORE_BUILD_CONFIG="${MUSESCORE_BUILD_CONFIG}" \
        -DCMAKE_BUILD_NUMBER="${MUSESCORE_BUILD_NUMBER}" \
        -DMUSESCORE_REVISION="${MUSESCORE_REVISION}" \
        -DMUE_RUN_LRELEASE="${MUSESCORE_RUN_LRELEASE}" \
        -DBUILD_PORTABLEAPPS="${MUSESCORE_BUILD_PORTABLEAPPS}" \
        -DCRASH_REPORT_URL="${MUSESCORE_CRASHREPORT_URL}" \
        -DBUILD_CRASHPAD_CLIENT="${MUSESCORE_BUILD_CRASHPAD_CLIENT}" \
        -DBUILD_UPDATE_MODULE="${MUSESCORE_BUILD_UPDATE_MODULE}" \
        -DLOGGER_DEBUGLEVEL_ENABLED="${MUSESCORE_DEBUGLEVEL_ENABLED}" \
        -DBUILD_JACK="${MUSESCORE_BUILD_JACK}" \
        -DBUILD_VST="${MUSESCORE_BUILD_VST}" \
        -DVST3_SDK_PATH="${MUSESCORE_VST3_SDK_PATH}" \
        -DDOWNLOAD_SOUNDFONT="${MUSESCORE_DOWNLOAD_SOUNDFONT}" \
        -DBUILD_UNIT_TESTS="${MUSESCORE_BUILD_UNIT_TESTS}" \
        -DBUILD_VIDEOEXPORT_MODULE="${MUSESCORE_BUILD_VIDEOEXPORT}" \
        -DCMAKE_SKIP_RPATH="${MUSESCORE_NO_RPATH}" \
        -DYOUTUBE_API_KEY="${MUSESCORE_YOUTUBE_API_KEY}"

    ninja -j $JOBS 
  }
  # case $TARGET in appimage)
    MUSESCORE_INSTALL_DIR=../../../MuseScore 
    MUSESCORE_INSTALL_SUFFIX="3portable${MUSESCORE_INSTALL_SUFFIX}" # e.g. "4portable" or "4portablenightly"
    MUSESCORE_LABEL="Portable AppImage" 
    MUSESCORE_NO_RPATH=ON 

    mkdir -p build.release
    cd build.release
    do_build RELEASE
    ninja install

    build_dir="$(pwd)" 
    install_dir="$(cat $build_dir/PREFIX.txt)" 
    cd $install_dir

    #todo
    # ln -sf . usr # we installed into the root of our AppImage but some tools expect a "usr" subdirectory
    # mscore="mscore${MUSESCORE_INSTALL_SUFFIX}"
    # desktop="org.musescore.MuseScore${MUSESCORE_INSTALL_SUFFIX}.desktop"
    # icon="${mscore}.svg" 
    # mani="install_manifest.txt" 
    # # cp "share/applications/${desktop}" "${desktop}"
    # cp "share/icons/hicolor/scalable/apps/${icon}" "${icon}" 
    # <"$build_dir/${mani}" >"${mani}" sed -rn 's/.*(share\/)(applications|icons|man|metainfo|mime)(.*)/\1\2\3/p'
# end ./ninja_build.sh

bash ./build/ci/tools/make_release_channel_env.sh -c $MUSE_APP_BUILD_MODE
bash ./build/ci/tools/make_version_env.sh $BUILD_NUMBER
bash ./build/ci/tools/make_revision_env.sh $MUSESCORE_REVISION
bash ./build/ci/tools/make_branch_env.sh
bash ./build/ci/tools/make_datetime_env.sh
df -h .
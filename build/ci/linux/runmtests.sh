#!/usr/bin/env bash
echo "############################## Run MuseScore mtest (runmtests.sh) ##############################"
trap 'echo run_mtests.sh failed; exit 1' ERR

df -h .

# var bash and github
eval "$(./build/ci/tools/read_artifact_env.sh)"

# main
cd build.debug/mtest

# run the mtests in "minimal" platform for headless systems
# enable fonts handling
export QT_QPA_PLATFORM=minimal:enable_fonts
# if AddressSanitizer was used, disable leak detection
export ASAN_OPTIONS=detect_leaks=0:new_delete_type_mismatch=0

make -j2

df -h .

ctest -j2 --output-on-failure

df -h .

echo "run_mtests.sh ended"
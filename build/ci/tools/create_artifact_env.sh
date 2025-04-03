#!/usr/bin/env bash

# eval (requires sudo) + echo instead of bash, only to match read_artifact_env.sh code style

echo 'ARTIFACTS_DIR=build.artifacts'
echo 'ENV_FILE=$ARTIFACTS_DIR/environment.sh' # does not use $HOME or $BUILD_TOOLS(derived from $HOME), as bash $HOME != github action $HOME
echo 'echo "ENV_FILE at $ENV_FILE"'

echo 'mkdir -p $ARTIFACTS_DIR'
echo 'rm -f $ENV_FILE'
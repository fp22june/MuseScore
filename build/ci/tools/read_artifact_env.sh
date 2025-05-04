#!/usr/bin/env bash
# verbose (copy build.sh) for future backporter search and replace
# var bash and github
ARTIFACTS_DIR="build.artifacts"
ENV_FILE=$ARTIFACTS_DIR/environment.sh # does not use $HOME or $BUILD_TOOLS(derived from $HOME), as bash $HOME != github action $HOME
echo "ENV_FILE at $ENV_FILE"
source "$ENV_FILE"
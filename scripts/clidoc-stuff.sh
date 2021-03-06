#!/bin/bash
CLIDOC_SOURCE_DIR="$(dirname $0)/.."
CLIDOC_SOURCE_DIR="$(cd $CLIDOC_SOURCE_DIR; pwd)"
CLIDOC_BUILD_DIR="${CLIDOC_SOURCE_DIR}/build"
CLIDOC_MAIN_PATH="${CLIDOC_BUILD_DIR}/src/clidoc_main"


function single_file_cpplint {
  while read -r file; do
    cpplint --root="${CLIDOC_SOURCE_DIR}/src" \
            --filter=-legal/copyright,-build/c++11,-runtime/string $file
    printf "\n=============================================\n"
  done
}

function lint {
  find "${CLIDOC_SOURCE_DIR}/src" "${CLIDOC_SOURCE_DIR}/include" -type f  \
      \( -not -name "generated_*.cc" -and -not -name "generated_parser.h" \
         -and -not -name "FlexLexer.h" \)                                 \
      -and \( -name "*.h" -or -name "*.cc" \)                             \
  		| single_file_cpplint 
}

function build {
  if [[ ! -d $CLIDOC_BUILD_DIR ]]; then
    mkdir "$CLIDOC_BUILD_DIR"
  fi
  cached_pwd=$(pwd)
  cd "$CLIDOC_BUILD_DIR"
  cmake ..
  make
  make test
  cd $cached_pwd
}

function update_main_doc {
  MAIN_DOC_LIB_DIR="${CLIDOC_SOURCE_DIR}/lib/main_doc"
  MAIN_DOC_PATH="${CLIDOC_SOURCE_DIR}/scripts/main_doc"
  rm -rf "$MAIN_DOC_LIB_DIR"
  $CLIDOC_MAIN_PATH -m cpp11_cmake_project \
      "$MAIN_DOC_PATH" "$MAIN_DOC_LIB_DIR"
}


if [[ $1 = "lint" ]]; then
  lint
elif [[ $1 = "build" ]]; then
  build
elif [[ $1 = "rebuild" ]]; then
  rm -rf "$CLIDOC_BUILD_DIR"
  build
elif [[ $1 = "update_main_doc" ]]; then
  update_main_doc
elif [[ $1 = "clidoc_main" ]]; then
  echo "$CLIDOC_MAIN_PATH"
fi

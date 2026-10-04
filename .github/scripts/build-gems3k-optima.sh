#!/usr/bin/env bash
# Builds Optima and GEMS3K from the gemshub GitHub repositories and installs them into the active
# conda environment (the GEMSGUI environment has no gems3k conda package), so that the GEMSGUI build
# finds a GEMS3K with the Optima solver (USE_OPTIMA_SOLVER) through find_package(GEMS3K).
#
# Run it with the environment activated, before configuring GEMSGUI:
#     conda activate GEMSGUI && bash .github/scripts/build-gems3k-optima.sh
#
# Branches/tags are taken from the environment, so a test branch can be tried without editing the
# workflows (in a workflow: repository variables GEMS3K_REF / OPTIMA_REF):
#     OPTIMA_REPO  (default https://github.com/gemshub/optima.git)  OPTIMA_REF  (default optima_develop)
#     GEMS3K_REPO  (default https://github.com/gemshub/GEMS3K.git)  GEMS3K_REF  (default develop_optima)
# The resulting commits are appended to $GITHUB_ENV as OPTIMA_COMMIT / GEMS3K_COMMIT.
set -euo pipefail

OPTIMA_REPO="${OPTIMA_REPO:-https://github.com/gemshub/optima.git}"
OPTIMA_REF="${OPTIMA_REF:-optima_develop}"
GEMS3K_REPO="${GEMS3K_REPO:-https://github.com/gemshub/GEMS3K.git}"
GEMS3K_REF="${GEMS3K_REF:-develop_optima}"

: "${CONDA_PREFIX:?activate the conda environment first}"

GENERATOR=(-GNinja)
OPTIMA_SHARED=ON
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    # Windows: conda keeps headers/libraries under Library; Visual Studio generator as the GUI build
    PREFIX="$(cygpath -m "$CONDA_PREFIX")/Library"
    GENERATOR=(-A x64)
    OPTIMA_SHARED=OFF
    ;;
  Linux)
    PREFIX="$CONDA_PREFIX"
    # same compilers as the GUI build, so the libraries share one C++ runtime
    if [ -x "$CONDA_PREFIX/bin/x86_64-conda-linux-gnu-g++" ]; then
      export CC="$CONDA_PREFIX/bin/x86_64-conda-linux-gnu-gcc"
      export CXX="$CONDA_PREFIX/bin/x86_64-conda-linux-gnu-g++"
      export CONDA_BUILD_SYSROOT="$CONDA_PREFIX/x86_64-conda-linux-gnu/sysroot"
    fi
    ;;
  *)
    PREFIX="$CONDA_PREFIX"
    ;;
esac

WORK="${RUNNER_TEMP:-${TMPDIR:-/tmp}}/gems3k-deps"
rm -rf "$WORK"
mkdir -p "$WORK"
cd "$WORK"

echo "::group::Optima ($OPTIMA_REPO @ $OPTIMA_REF)"
git clone --depth 1 --branch "$OPTIMA_REF" "$OPTIMA_REPO" optima
cmake -S optima -B optima/build "${GENERATOR[@]}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DCMAKE_PREFIX_PATH="$PREFIX" \
  -DBUILD_SHARED_LIBS="$OPTIMA_SHARED" \
  -DOPTIMA_BUILD_ALL=OFF -DOPTIMA_BUILD_BENCH=OFF -DOPTIMA_BUILD_DEMOS=OFF \
  -DOPTIMA_BUILD_DOCS=OFF -DOPTIMA_BUILD_PYTHON=OFF
cmake --build optima/build --config Release --target install
echo "::endgroup::"

echo "::group::GEMS3K ($GEMS3K_REPO @ $GEMS3K_REF)"
git clone --depth 1 --branch "$GEMS3K_REF" "$GEMS3K_REPO" GEMS3K
cmake -S GEMS3K -B GEMS3K/build "${GENERATOR[@]}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DCMAKE_PREFIX_PATH="$PREFIX" \
  -DUSE_OPTIMA_SOLVER=ON \
  -DBUILD_TOOLS=OFF -DBUILD_SOLMOD=OFF -DBUILD_SOLMOD_PYTHON=OFF
cmake --build GEMS3K/build --config Release --target install
echo "::endgroup::"

OPTIMA_COMMIT="$(git -C optima rev-parse --short HEAD)"
GEMS3K_COMMIT="$(git -C GEMS3K rev-parse --short HEAD)"
echo "Optima $OPTIMA_REF @ $OPTIMA_COMMIT, GEMS3K $GEMS3K_REF @ $GEMS3K_COMMIT installed in $PREFIX"
if [ -n "${GITHUB_ENV:-}" ]; then
  {
    echo "OPTIMA_REF_USED=$OPTIMA_REF"
    echo "OPTIMA_COMMIT=$OPTIMA_COMMIT"
    echo "GEMS3K_REF_USED=$GEMS3K_REF"
    echo "GEMS3K_COMMIT=$GEMS3K_COMMIT"
  } >> "$GITHUB_ENV"
fi

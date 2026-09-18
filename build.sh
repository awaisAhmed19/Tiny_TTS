#!/usr/bin/env bash
# =============================================================================
# build.sh — TinyTTS build script
#
# Commands:
#   ./build.sh                  — Debug build (default)
#   ./build.sh debug            — Debug build
#   ./build.sh release          — Release build
#   ./build.sh relwithdebinfo   — RelWithDebInfo build
#   ./build.sh clean            — Remove build directory
#   ./build.sh run              — Build (Debug) and run
# =============================================================================

set -euo pipefail

# ── Formatting ────────────────────────────────────────────────────────────────

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

log_info() { echo -e "${CYAN}[build]${RESET} $*"; }
log_ok() { echo -e "${GREEN}[build]${RESET} $*"; }
log_warn() { echo -e "${YELLOW}[build]${RESET} $*"; }
log_error() { echo -e "${RED}[build]${RESET} $*" >&2; }
log_section() {
	echo -e "\n${BOLD}>>> $* <<<${RESET}"
}

die() {
	log_error "$*"
	exit 1
}

# ── Configuration ─────────────────────────────────────────────────────────────

BUILD_DIR="build"
BINARY_NAME="tinytts"

# Detect CPU core count portably.
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# ── Sanity checks ─────────────────────────────────────────────────────────────

require_cmake() {
	command -v cmake &>/dev/null ||
		die "cmake not found."

	command -v ninja &>/dev/null ||
		die "ninja not found."
}

require_project_root() {
	[[ -f CMakeLists.txt ]] ||
		die "CMakeLists.txt not found. Run this script from the project root."
}

# ── Build ─────────────────────────────────────────────────────────────────────

do_build() {
	local build_type="$1"
	local build_subdir="${BUILD_DIR}/${build_type,,}"

	log_section "Configuring: ${build_type}"

	cmake \
		-B "$build_subdir" \
		-G Ninja \
		-DCMAKE_BUILD_TYPE="$build_type" \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_COLOR_DIAGNOSTICS=ON

	# Keep compile_commands.json in project root for clangd.
	if [[ -f "${build_subdir}/compile_commands.json" ]]; then
		ln -sf "${build_subdir}/compile_commands.json" compile_commands.json
	fi

	log_section "Building: ${build_type} (${CORES} cores)"

	local start_time
	start_time=$(date +%s)

	cmake --build "$build_subdir" -j "$CORES"

	local end_time elapsed
	end_time=$(date +%s)
	elapsed=$((end_time - start_time))

	log_ok "Build complete in ${elapsed}s → ${build_subdir}/"

	# Print binary information.
	local binary="${build_subdir}/${BINARY_NAME}"

	if [[ -f "$binary" ]]; then
		local size
		size=$(du -sh "$binary" | cut -f1)

		log_info "Binary: ${binary} (${size})"
	fi
}

# ── Clean ─────────────────────────────────────────────────────────────────────

do_clean() {
	log_section "Cleaning build directory"

	if [[ -d "$BUILD_DIR" ]]; then
		rm -rf "$BUILD_DIR"
		log_ok "Removed: ${BUILD_DIR}/"
	else
		log_info "Nothing to clean (${BUILD_DIR}/ does not exist)."
	fi

	if [[ -L compile_commands.json ]]; then
		rm compile_commands.json
		log_info "Removed: compile_commands.json"
	fi
}

# ── Run ───────────────────────────────────────────────────────────────────────

do_run() {
	do_build "Debug"

	local build_subdir="${BUILD_DIR}/debug"
	local binary="${build_subdir}/${BINARY_NAME}"

	[[ -x "$binary" ]] ||
		die "Binary not found: ${binary}"

	log_section "Running ${BINARY_NAME}"

	exec "$binary"
}

do_test() {
	do_build "Debug"

	log_section "Running tests"

	ctest \
		--test-dir "${BUILD_DIR}/debug" \
		--output-on-failure
}

# ── Entry point ───────────────────────────────────────────────────────────────

main() {
	require_project_root

	local cmd="${1:-debug}"

	case "${cmd,,}" in
	debug)
		require_cmake
		do_build "Debug"
		;;

	release)
		require_cmake
		do_build "Release"
		;;

	relwithdebinfo | reldbg)
		require_cmake
		do_build "RelWithDebInfo"
		;;

	clean)
		do_clean
		;;

	run)
		require_cmake
		do_run
		;;

	test)
		require_cmake
		do_test
		;;

	help | --help | -h)
		echo "Usage: $0 [command]"
		echo ""
		echo "Commands:"
		echo "  (none) / debug       Debug build              [default]"
		echo "  release              Release build"
		echo "  relwithdebinfo       RelWithDebInfo build"
		echo "  clean                Remove build directory"
		echo "  run                  Debug build + run"
		echo ""
		;;

	*)
		die "Unknown command: ${cmd}. Run '$0 help' for usage."
		;;
	esac
}

main "$@"

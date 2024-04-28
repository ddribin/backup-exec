version := "1.0.2"

_default:
  @just --list --unsorted

# Build borg and restic for amd64
build-amd64: _borg-amd64 _restic-amd64
# Build borg and restic for arm64
build-arm64: _borg-arm64 _restic-arm64

# Build borg and restic for both amd64 and arm64
build-all: build-amd64 build-arm64

_borg-amd64: (_build "borg" "amd64")
_borg-arm64: (_build "borg" "arm64")

_restic-amd64: (_build "restic" "amd64")
_restic-arm64: (_build "restic" "arm64")

# basename in /usr/local/libexec/
_build path arch:
    docker build --platform linux/{{arch}} -t backup-exec .
    docker run \
        --platform linux/{{arch}} \
        --rm \
        -v "$PWD":/work \
        backup-exec \
        gcc \
        -o build/{{path}}-{{arch}} \
        -Os \
        -DEXEC_BIN='"/usr/local/libexec/{{path}}"' \
        -DVERSION='"{{version}}"' \
        backup-exec.c \
        -lcap-ng

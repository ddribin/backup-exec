version := "1.0.2"

# 2024-05-05 05:36:05+00:00
# Via date +%s
export SOURCE_DATE_EPOCH := "1714887365"
export GZIP := "--no-name"
# For reproducible builds. See:
# https://reproducible-builds.org/docs/archives/
# https://serverfault.com/questions/110208/different-md5sums-for-same-tar-contents

build := "build"

amd64_suffix := "-amd64"
arm64_suffix := "-arm64"

borg_dir_prefix := "backup-exec-borg-"
borg_amd64_dir := borg_dir_prefix + version + amd64_suffix
borg_arm64_dir := borg_dir_prefix + version + arm64_suffix

restic_dir_prefix := "backup-exec-restic-"
restic_amd64_dir := restic_dir_prefix + version + amd64_suffix
restic_arm64_dir := restic_dir_prefix + version + arm64_suffix

extra_tar_flags := if os() == "macos" {
    "--no-mac-metadata --no-xattrs"
} else {
    ""
}

_default:
  @just --list --unsorted

# Build borg and restic for amd64
build-amd64: _borg-amd64 _restic-amd64
# Build borg and restic for arm64
build-arm64: _borg-arm64 _restic-arm64

# Build borg and restic for both amd64 and arm64
build-all: build-amd64 build-arm64

# Clean the build director
clean:
    rm -rf "{{build}}"

# Print version info
build-info:
    @echo Version {{version}}
    @date -r "$SOURCE_DATE_EPOCH"

# Print SHA256 checksums
checksums:
    @cd "{{ build }}" && shasum -a256 *.tgz

_borg-amd64: (_build "borg" "amd64" borg_amd64_dir)
_borg-arm64: (_build "borg" "arm64" borg_arm64_dir)

_restic-amd64: (_build "restic" "amd64" restic_amd64_dir)
_restic-arm64: (_build "restic" "arm64" restic_arm64_dir)

_build exec_bin arch build_dir:
    mkdir -p "{{build}}/{{build_dir}}"
    docker build --platform linux/{{arch}} -t backup-exec .
    docker run \
        --platform linux/{{arch}} \
        --rm \
        -v "$PWD":/work \
        backup-exec \
        gcc \
        -o "{{build}}/{{build_dir}}/{{exec_bin}}" \
        -Os \
        -DEXEC_BIN='"/usr/local/libexec/{{exec_bin}}"' \
        -DVERSION='"{{version}}"' \
        backup-exec.c \
        -lcap-ng
    gtar \
        --sort=name \
        --mtime="@${SOURCE_DATE_EPOCH}" \
        --owner=0 --group=0 --numeric-owner \
        --pax-option=exthdr.name=%d/PaxHeaders/%f,delete=atime,delete=ctime \
        -zcvf "{{build}}/{{build_dir}}.tgz" \
        -C "{{build}}" "{{build_dir}}"

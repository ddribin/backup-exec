# Changelog

## [1.0.2] - 2023-05-05

### Changed

- Log the version when `BACKUP_EXEC_VERBOSE=1` is set, taken from the `VERSION` `#define`.
- Add a `justfile` to build binaries for both AMD64 and ARM64 Linux.

## [1.0.1] - 2023-03-18

### Changed

- Remove the `DEBUG` compile time macro in favor of checking the `BACKUP_EXEC_VERBOSE` environment variable at runtime.

## [1.0.0] - 2021-01-21

_Iintial Release._

[1.0.2]: https://github.com/ddribin/backup-exec/releases/tag/v1.0.2
[1.0.1]: https://github.com/ddribin/backup-exec/releases/tag/v1.0.1
[1.0.0]: https://github.com/ddribin/backup-exec/releases/tag/v1.0.0

# Overview

Creates wrapper executables intended for backing up files. The wrapper uses the `CAP_DAC_READ_SEARCH` capability to allow backup programs to read any file without having `root` privileges. See [`capabilities(7)`](https://man7.org/linux/man-pages/man7/capabilities.7.html) for more about this capability.

# Compiling and Running

This requires [`libcap-ng`](https://github.com/stevegrubb/libcap-ng), which can be installed with `apt`:

    apt install libcap-ng-dev

To compile, you should specify the absolute path to a binary by setting the `EXEC_BIN` macro:

    mkdir build
    gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng
 
 Then add the capabilities on the resulting binary:
 
     sudo setcap "cap_dac_read_search=+eip cap_setpcap=+ep" capsh

The `cap_setpcap` capability is required so that it can set [ambient capabilities](https://lwn.net/Articles/636533/). Ambient capabilities allows programs to inherit capabilities without running in ["secure-exection" mode](https://man7.org/linux/man-pages/man8/ld.so.8.html) (sometimes called "execure exec" mode). When run, this will run the program pointed to by `EXEC_BIN` with `CAP_DAC_READ_SEARCH`. You can verify it has the capability:

    > ./capsh --print
    Current: = cap_dac_read_search+eip
    ...

The `BACKUP_EXEC_VERBOSE` can be set to `1` to verify the execution of the correct `EXEC_BIN`:

    > BACKUP_EXEC_VERBOSE=1 ./capsh --print
    Original:
    0: <./capsh>
    1: <--print>
    Modified:
    0: </sbin/capsh>
    1: <--print>
    Current: = cap_dac_read_search+eip
    ...

Note that it does not matter what the executbale name is. It always re-`execs` as the compiled in `EXEC_BIN`:

    > BACKUP_EXEC_VERBOSE=1 ./foo --print
    Original:
    0: <./foo>
    1: <--print>
    Modified:
    0: </sbin/capsh>
    1: <--print>
    Current: = cap_dac_read_search+eip
    ...

# Compling with Docker

To compile using the Docker image, first build the image:

    docker build -t backup-exec .

Then compile:

    docker run --rm -v "$PWD":/work backup-exec gcc -o build/capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng

As a more practical example, here's how to create a wrapper for the for the [Borg backup](https://www.borgbackup.org) binary:

    docker run --rm -v "$PWD":/work backup-exec gcc -o build/borg -Os -DEXEC_BIN='"/usr/local/libexec/borg"' backup-exec.c -lcap-ng

And also for [Restic backup](https://restic.net) binary:

    docker run --rm -v "$PWD":/work backup-exec gcc -o build/restic -Os -DEXEC_BIN='"/usr/local/libexec/restic"' backup-exec.c -lcap-ng

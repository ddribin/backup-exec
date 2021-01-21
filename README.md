Creates wrapper executbles intended for backing up files. The wrapper uses the `CAP_DAC_READ_SEARCH` capability to allow backup programs to read any file without having `root` privileges. See [`capabilities(7)`](https://man7.org/linux/man-pages/man7/capabilities.7.html) for more about this capability.

This requires [`libcap-ng`](https://github.com/stevegrubb/libcap-ng), which can be installed:

    $ apt install libcap-ng-dev

To compile, you should specify a binary by setting the `EXEC_BIN` macro to the absolute path of the program:

    $ gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng
 
 Then add the capabilities on the resulting binary:
 
     $ sudo setcap "cap_dac_read_search=+eip cap_setpcap=+ep" capsh

The `cap_setpcap` capability is required so that it can set [ambient capabilities](https://lwn.net/Articles/636533/). Ambient capabilities allows programs to inherit capabilities without running in "secure exec" mode. When run, this will run the program pointed to by `EXEC_BIN` with `CAP_DAC_READ_SEARCH`. You can verify it has the capability:

    $ ./capsh --print
    Current: = cap_dac_read_search+eip
    ...

To compile using the Docker image, first build the image:

    $ docker build -t backup-exec .

Then compile:

    % docker run --rm -v "$PWD":/work backup-exec gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng

As a more practical example, here's how to create a wrapper for the for the [Borg backup](https://www.borgbackup.org) binary:

    $ docker run --rm -v "$PWD":/work backup-exec gcc -o build/borg -DDEBUG=1 -DEXEC_BIN='"/usr/local/libexec/borg"' backup-exec.c -lcap-ng

And also for [Restic backup](https://restic.net) binary:

    $ docker run --rm -v "$PWD":/work backup-exec gcc -o build/restic -DDEBUG=1 -DEXEC_BIN='"/usr/local/libexec/restic"' backup-exec.c -lcap-ng

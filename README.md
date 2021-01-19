Creates wrapper executbles intended for backing up files. The wrapper uses the `CAP_DAC_READ_SEARCH` capability to allow backup programs to read any file without having `root` privileges. See [`capabilities(7)`](https://man7.org/linux/man-pages/man7/capabilities.7.html) for more about this capability.

This requires [`libcap-ng`](https://github.com/stevegrubb/libcap-ng), which can be installed:

    $ apt install libcap-ng-dev

To compile:

    $ gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng
 
 Then add the capabilities on the resulting binary:
 
     $ sudo setcap "cap_dac_read_search=+eip cap_setpcap=+ep" capsh

The `cap_setpcap` capability is required so that it can set the ambient capability. The ambient capability allows programs to inherit capabilities without being a privileged binary. When run, you can verify it has the capability:

    $ ./capsh --print
    Current: = cap_dac_read_search+eip
    ...

To compile using the Docker image, first build the image:

    $ docker build -t backup-exec .

Then compile:

    % docker run --rm -v "$PWD":/work backup-exec gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng

As a more practical example, here's how to create a wrapper for the for the [Borg Backup](https://www.borgbackup.org) binary:

    $ docker run --rm -v "$PWD":/work backup-exec gcc -o build/borg -DDEBUG=1 -DEXEC_BIN='"/usr/local/libexec/borg"' backup-exec.c -lcap-ng

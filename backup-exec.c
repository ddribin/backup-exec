/*
 * gcc -DEXEC_BIN='"/sbin/capsh"' -o capsh backup-exec.c -lcap-ng
 * sudo setcap "cap_dac_read_search=+eip cap_setpcap=+ep" capsh
 *
 * Adapted from: https://gist.github.com/infinity0/596845b5eea3e1a02a018009b2931a39
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <linux/capability.h>
#include <cap-ng.h>

#ifndef EXEC_BIN
#define EXEC_BIN "/sbin/capsh"
#warning Defaulting to EXEC_BIN
#endif

static void set_ambient_cap(int cap)
{
    capng_get_caps_process();
    int rc = capng_update(CAPNG_ADD, CAPNG_INHERITABLE, cap);
    if (rc) {
        fprintf(stderr, "Cannot add inheritable cap\n");
        exit(EXIT_FAILURE);
    }
    capng_apply(CAPNG_SELECT_CAPS);

    /* Note the two 0s at the end. Kernel checks for these */
    if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0)) {
        perror("Cannot set cap");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    set_ambient_cap(CAP_DAC_READ_SEARCH);

    // Allocate argc+1 for trailing NULL
    char **new_argv = malloc(sizeof(char *) * argc+1);
    memcpy(new_argv, argv, sizeof(char *) * argc);
    new_argv[0] = EXEC_BIN;
    new_argv[argc] = NULL;

#if DEBUG
    for (int i = 0; i < argc; i++) {
      fprintf(stderr, "%d: <%s>\n", i, new_argv[i]);
    }
#endif
    
    execv(new_argv[0], new_argv);
    fprintf(stderr, "Cannot exec: %d %s\n", errno, strerror(errno));
    perror("Cannot exec");
    return EXIT_FAILURE;
}

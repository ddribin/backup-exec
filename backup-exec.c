/*
 * gcc -o capsh -DEXEC_BIN='"/sbin/capsh"' backup-exec.c -lcap-ng
 * sudo setcap "cap_dac_read_search=+eip cap_setpcap=+ep" capsh
 *
 * Adapted from: https://gist.github.com/infinity0/596845b5eea3e1a02a018009b2931a39
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/prctl.h>
#include <linux/capability.h>
#include <cap-ng.h>

#ifndef EXEC_BIN
#define EXEC_BIN "/sbin/capsh"
#warning Defaulting to EXEC_BIN
#endif

#ifndef VERSION
#define VERSION "HEAD (" __DATE__ " " __TIME__ ")"
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

static bool is_verbose_enabled(void)
{
    const char *backup_exec_verbose = getenv("BACKUP_EXEC_VERBOSE");
    if ((backup_exec_verbose != NULL) &&
        (strcmp(backup_exec_verbose, "1") == 0))
    {
        return true;
    } else {
        return false;
    }
}

static void verbose_print_arguments(const char *label, int argc, char **argv)
{
    // Output arguments if BACKUP_EXEC_VERBOSE=1
    if (is_verbose_enabled()) {
        fprintf(stderr, "%s\n", label);
        for (int i = 0; i < argc; i++) {
            fprintf(stderr, "%d: <%s>\n", i, argv[i]);
        }
    }
}

static void verbose_printf(const char *format, ...)
{
    if (is_verbose_enabled()) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}

int main(int argc, char **argv)
{
    verbose_printf("backup-exec version " VERSION "\n");
    verbose_print_arguments("Original:", argc, argv);

    set_ambient_cap(CAP_DAC_READ_SEARCH);

    // Allocate argc+1 for trailing NULL
    char **new_argv = malloc(sizeof(char *) * argc+1);
    memcpy(new_argv, argv, sizeof(char *) * argc);
    // Replace argv[0] with EXEC_BIN
    new_argv[0] = EXEC_BIN;
    new_argv[argc] = NULL;

    verbose_print_arguments("Modified:", argc, new_argv);

    execv(new_argv[0], new_argv);
    fprintf(stderr, "Cannot exec %s: (%d) %s\n", new_argv[0], errno, strerror(errno));
    return EXIT_FAILURE;
}

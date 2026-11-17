#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>

int main(void) {
    if (setgroups(0, NULL) != 0) {
        perror("setgroups failed");
        return 1;
    }

    if (setgid(0) != 0 || setuid(0) != 0) {
        perror("setuid/setgid failed");
        return 1;
    }

    clearenv();
    setenv("PATH", "/usr/bin:/bin", 1);

    execl("/usr/bin/xinit", "xinit",
          "/usr/local/libexec/xsleep-session",
          "--", ":1",
          NULL);

    perror("exec failed");
    return 1;
}

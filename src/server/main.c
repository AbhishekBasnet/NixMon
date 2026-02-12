#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

long read_uptime() {
    FILE *fp = fopen("/proc/uptime", "r");
    if (!fp) {
        perror("Failed to open /proc/uptime");
        return 0;
    }

    double uptime;
    fscanf(fp, "%lf", &uptime);
    fclose(fp);
    return (long)uptime;
}

int main() {
    printf("NixMon Server Starting...\n");

    long uptime = read_uptime();
    printf("System Uptime: %ld seconds\n", uptime);

    return 0;
}

#ifndef NIXMON_PROTOCOL_H
#define NIXMON_PROTOCOL_H

typedef struct {
    double cpu_pct;
    double cpu_cores[16];
    int core_count;

    long mem_total_kb;
    long mem_avail_kb;
    double mem_pct;

    double load_1m;
    double load_5m;
    double load_15m;

    long uptime_sec;
} SystemMetrics;

#define BUFFER_SIZE 4096
#define PORT 8080

#endif

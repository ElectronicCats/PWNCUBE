#ifndef __WORKERS_H
#define __WORKERS_H
#include <proto.h>

// Telemetry
void telemetryConfigureSensors(void);
void telemetryWorker(void);
void telemetryPingToGsWorker(void);
void telemetryIdleWorker(void);
#endif

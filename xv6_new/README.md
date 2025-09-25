# xv6 Modifications

This directory contains patches for the xv6 educational OS.

## Modifications

### System Calls
- `getreadcount()` - Track total bytes read across all processes

### Schedulers
Three scheduling implementations:
1. **FCFS** - First Come First Serve, non-preemptive
2. **CFS** - Completely Fair Scheduler, virtual runtime based
3. **MLFQ** - Multi-Level Feedback Queue, priority based

## Building

```bash
make qemu SCHEDULER=FCFS    # Build with FCFS scheduler
make qemu SCHEDULER=CFS     # Build with CFS scheduler
make qemu SCHEDULER=MLFQ    # Build with MLFQ scheduler
```

## Testing

Run scheduler tests to measure:
- Turnaround time
- Response time
- Fairness
- Throughput

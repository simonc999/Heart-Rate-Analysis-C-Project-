# Heart-Rate Analysis Project (C)

A small, self-contained C program that parses a file of heart-rate measurements and prints a concise report with six sections:

1. **[Measures]**   Count of readings below *HRinf*, between *HRinf* and *HRsup*, and ≥ *HRsup*  
2. **[Training]**   Total workout duration (min) and average sampling interval (s)  
3. **[Peak]**   Peak heart-rate value  
4. **[Out of range]**   Number of contiguous intervals with readings above *HRsup*  
5. **[Intense interval]**   First and last minute (relative to start) with intensity ≥ *HRinf*  
6. **[Print]**   Ten most recent readings in reverse chronological order  

The code is fully modular: every task lives in its own function so the `main()` simply orchestrates I/O and calls helpers.

---

## File format

HRinf HRsup # first line: two integers
hh:mm:ss value # one measurement per line

*Example*

120 160
09:30:00 85
09:30:20 122
09:30:40 155

---

## Building

```bash
gcc -std=c99 heart_rate_analysis.c -o hr_analysis
```
The program uses only the C standard library, so it builds on any modern compiler (GCC, Clang, MSVC, etc.).


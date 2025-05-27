/*
 * heart_rate_analysis.c
 * Author: simonc999
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_MEASURES 300

typedef struct {
    int hh, mm, ss;   /* ora, minuti, secondi */
    int value;        /* valore battiti */
    int t;            /* istante in secondi dal giorno inizio */
} Measure;

static int time_to_seconds(int h, int m, int s) {
    return h * 3600 + m * 60 + s;
}


static bool read_header(FILE *fp, int *HRinf, int *HRsup) {
    if (fscanf(fp, "%d %d", HRinf, HRsup) != 2)
        return false;
    int c;
    while ((c = fgetc(fp)) != '\n' && c != EOF) /* consuma fine riga */
        ;
    return true;
}


static int read_measures(FILE *fp, Measure measures[]) {
    char line[128];
    int n = 0;
    while (fgets(line, sizeof(line), fp) && n < MAX_MEASURES) {
        if (line[0] == '\n' || line[0] == '\0')
            continue; /* salta righe vuote */
        int h, m, s, v;
        if (sscanf(line, "%d:%d:%d %d", &h, &m, &s, &v) == 4) {
            measures[n].hh = h;
            measures[n].mm = m;
            measures[n].ss = s;
            measures[n].value = v;
            measures[n].t = time_to_seconds(h, m, s);
            ++n;
        }
    }
    return n;
}



static void count_zones(const Measure m[], int n, int HRinf, int HRsup,
                        int *a, int *b, int *c) {
    *a = *b = *c = 0;
    for (int i = 0; i < n; ++i) {
        int v = m[i].value;
        if (v < HRinf)
            ++*a;
        else if (v < HRsup)
            ++*b;
        else
            ++*c;
    }
}

static int training_duration_min(const Measure m[], int n) {
    if (n == 0) return 0;
    return (m[n - 1].t - m[0].t) / 60; /* tronca ai minuti */
}

static double average_interval(const Measure m[], int n) {
    if (n < 2) return -1.0; /* codice sentinella --> NA */
    return (double)(m[n - 1].t - m[0].t) / (n - 1);
}

static int peak_value(const Measure m[], int n) {
    int peak = 0;
    for (int i = 0; i < n; ++i)
        if (m[i].value > peak)
            peak = m[i].value;
    return peak;
}

static int intervals_out_of_range(const Measure m[], int n, int HRsup) {
    int intervals = 0;
    bool in_out = false;
    for (int i = 0; i < n; ++i) {
        if (m[i].value > HRsup) {
            if (!in_out) { ++intervals; in_out = true; }
        } else {
            in_out = false;
        }
    }
    return intervals;
}


static bool intense_interval(const Measure m[], int n, int HRinf,
                             int *start_min, int *end_min) {
    int start = -1, end = -1;
    for (int i = 0; i < n; ++i) {
        if (m[i].value >= HRinf) {
            if (start == -1) start = m[i].t;
            end = m[i].t;
        }
    }
    if (start == -1) return false;
    *start_min = (start - m[0].t) / 60;
    *end_min   = (end   - m[0].t) / 60;
    return true;
}

static void print_last_descending(const Measure m[], int n, int num) {
    int to_print = (n < num) ? n : num;
    for (int i = 0; i < to_print; ++i) {
        const Measure *mm = &m[n - 1 - i];
        printf("%02d:%02d:%02d %d\n", mm->hh, mm->mm, mm->ss, mm->value);
    }
}

// main

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Cannot open input file");
        return 1;
    }

    int HRinf, HRsup;
    if (!read_header(fp, &HRinf, &HRsup)) {
        fprintf(stderr, "Invalid header line\n");
        fclose(fp);
        return 1;
    }

    Measure measures[MAX_MEASURES];
    int n = read_measures(fp, measures);
    fclose(fp);

    // 1
    int countA, countB, countC;
    count_zones(measures, n, HRinf, HRsup, &countA, &countB, &countC);

    printf("[Measures]\n%d\n%d\n%d\n", countA, countB, countC);

    // 2
    printf("[Training]\n");
    int duration = training_duration_min(measures, n);
    printf("%d\n", duration);

    double avg_int = average_interval(measures, n);
    if (avg_int < 0)
        printf("NA\n");
    else
        printf("%.2lf\n", avg_int);

    // 3
    printf("[Peak]\n%d\n", peak_value(measures, n));

    // 4
    printf("[Out of range]\n%d\n", intervals_out_of_range(measures, n, HRsup));

    // 5
    printf("[Intense interval]\n");
    int s_min, e_min;
    if (intense_interval(measures, n, HRinf, &s_min, &e_min))
        printf("%d\n%d\n", s_min, e_min);
    else
        printf("NA\nNA\n");

    // 6
    printf("[Print]\n");
    print_last_descending(measures, n, 10);

    return 0;
}

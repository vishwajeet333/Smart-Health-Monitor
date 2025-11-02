#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_RECORDS 1000
#define MAX_LINE 512
#define MAX_NAME 100

// Health data structure
typedef struct {
    char date[20];
    int heart_rate;
    int systolic_bp;
    int diastolic_bp;
    int blood_sugar;
    float temperature;
    int oxygen_level;
    int steps;
} HealthRecord;

// Statistics structure
typedef struct {
    float avg_heart_rate;
    float avg_systolic;
    float avg_diastolic;
    float avg_blood_sugar;
    float avg_temperature;
    float avg_oxygen;
    int total_steps;
    int record_count;
} HealthStats;

// Alert structure
typedef struct {
    char message[256];
    int severity; // 1=Low, 2=Medium, 3=High, 4=Critical
} Alert;

// Function prototypes
void print_banner();
void print_menu();
int load_csv_data(const char *filename, HealthRecord records[], int *count);
int load_txt_data(const char *filename, HealthRecord records[], int *count);
void calculate_statistics(HealthRecord records[], int count, HealthStats *stats);
void analyze_health(HealthRecord records[], int count, HealthStats stats, Alert alerts[], int *alert_count);
void display_report(HealthStats stats, Alert alerts[], int alert_count);
void display_trends(HealthRecord records[], int count);
void generate_advice(Alert alerts[], int alert_count);
void export_report(HealthStats stats, Alert alerts[], int alert_count, const char *filename);
void add_manual_record(HealthRecord records[], int *count);
float calculate_bmi(float weight, float height);
int calculate_health_score(HealthStats stats);
void display_health_score(int score);
void create_sample_data();
void print_line(char c, int length);

int main() {
    HealthRecord records[MAX_RECORDS];
    Alert alerts[50];
    HealthStats stats = {0};
    int record_count = 0;
    int alert_count = 0;
    int choice;
    char filename[100];

    print_banner();

    while (1) {
        print_menu();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline

        switch (choice) {
            case 1:
                printf("\nEnter CSV filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;

                if (load_csv_data(filename, records, &record_count)) {
                    printf("[SUCCESS] Loaded %d records successfully!\n", record_count);
                } else {
                    printf("[ERROR] Failed to load data.\n");
                }
                break;

            case 2:
                printf("\nEnter TXT filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;

                if (load_txt_data(filename, records, &record_count)) {
                    printf("[SUCCESS] Loaded %d records successfully!\n", record_count);
                } else {
                    printf("[ERROR] Failed to load data.\n");
                }
                break;

            case 3:
                if (record_count == 0) {
                    printf("[WARNING] No data loaded. Please load data first.\n");
                } else {
                    calculate_statistics(records, record_count, &stats);
                    analyze_health(records, record_count, stats, alerts, &alert_count);
                    display_report(stats, alerts, alert_count);
                }
                break;

            case 4:
                if (record_count == 0) {
                    printf("[WARNING] No data loaded. Please load data first.\n");
                } else {
                    display_trends(records, record_count);
                }
                break;

            case 5:
                if (alert_count == 0) {
                    printf("[WARNING] No analysis performed yet. Please analyze data first (Option 3).\n");
                } else {
                    generate_advice(alerts, alert_count);
                }
                break;

            case 6:
                add_manual_record(records, &record_count);
                break;

            case 7:
                if (record_count == 0) {
                    printf("[WARNING] No data to export.\n");
                } else {
                    printf("\nEnter output filename: ");
                    fgets(filename, sizeof(filename), stdin);
                    filename[strcspn(filename, "\n")] = 0;
                    export_report(stats, alerts, alert_count, filename);
                }
                break;

            case 8:
                create_sample_data();
                break;

            case 9:
                print_line('=', 60);
                printf("    Thank you for using Smart Health Monitor!\n");
                printf("    Stay healthy!\n");
                print_line('=', 60);
                printf("\n");
                return 0;

            default:
                printf("[ERROR] Invalid choice. Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}

void print_line(char c, int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", c);
    }
    printf("\n");
}

void print_banner() {
    printf("\n");
    print_line('=', 60);
    printf("                                                            \n");
    printf("                   SMART HEALTH MONITOR                     \n");
    printf("                                                            \n");
    printf("          Your Personal Health Analytics System            \n");
    printf("                                                            \n");
    print_line('=', 60);
    printf("\n");
}

void print_menu() {
    printf("\n");
    print_line('-', 45);
    printf("               MAIN MENU                     \n");
    print_line('-', 45);
    printf(" 1. Load CSV Health Data                    \n");
    printf(" 2. Load TXT Health Data                    \n");
    printf(" 3. Analyze Health & Get Report             \n");
    printf(" 4. View Health Trends                      \n");
    printf(" 5. Get Personalized Advice                 \n");
    printf(" 6. Add Manual Health Record                \n");
    printf(" 7. Export Report                           \n");
    printf(" 8. Generate Sample Data File               \n");
    printf(" 9. Exit                                    \n");
    print_line('-', 45);
}

int load_csv_data(const char *filename, HealthRecord records[], int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    char line[MAX_LINE];
    *count = 0;

    // Skip header line
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) && *count < MAX_RECORDS) {
        // Remove trailing newline/carriage return
        line[strcspn(line, "\r\n")] = 0;

        // Skip empty lines
        if (strlen(line) < 5) continue;

        // Initialize record with safe defaults
        records[*count].heart_rate = 0;
        records[*count].systolic_bp = 0;
        records[*count].diastolic_bp = 0;
        records[*count].blood_sugar = 0;
        records[*count].temperature = 0.0;
        records[*count].oxygen_level = 0;
        records[*count].steps = 0;

        int items = sscanf(line, "%[^,],%d,%d,%d,%d,%f,%d,%d",
               records[*count].date,
               &records[*count].heart_rate,
               &records[*count].systolic_bp,
               &records[*count].diastolic_bp,
               &records[*count].blood_sugar,
               &records[*count].temperature,
               &records[*count].oxygen_level,
               &records[*count].steps);

        // Only count if we successfully read at least 5 fields
        if (items >= 5) {
            (*count)++;
        }
    }

    fclose(file);
    return (*count > 0);
}

int load_txt_data(const char *filename, HealthRecord records[], int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    char line[MAX_LINE];
    *count = 0;

    // Initialize current record
    HealthRecord current;
    int fields_read = 0;

    while (fgets(line, sizeof(line), file) && *count < MAX_RECORDS) {
        // Remove any trailing whitespace/newlines
        line[strcspn(line, "\r\n")] = 0;

        if (strstr(line, "Date:") && strlen(line) > 6) {
            if (fields_read == 7 && *count < MAX_RECORDS) {
                records[*count] = current;
                (*count)++;
                fields_read = 0;
            }
            sscanf(line, "Date: %s", current.date);
            fields_read = 1;
        }
        else if (strstr(line, "Heart Rate:") && fields_read >= 1) {
            sscanf(line, "Heart Rate: %d", &current.heart_rate);
            fields_read = 2;
        }
        else if (strstr(line, "Blood Pressure:") && fields_read >= 2) {
            sscanf(line, "Blood Pressure: %d/%d",
                   &current.systolic_bp, &current.diastolic_bp);
            fields_read = 3;
        }
        else if (strstr(line, "Blood Sugar:") && fields_read >= 3) {
            sscanf(line, "Blood Sugar: %d", &current.blood_sugar);
            fields_read = 4;
        }
        else if (strstr(line, "Temperature:") && fields_read >= 4) {
            sscanf(line, "Temperature: %f", &current.temperature);
            fields_read = 5;
        }
        else if (strstr(line, "Oxygen Level:") && fields_read >= 5) {
            sscanf(line, "Oxygen Level: %d", &current.oxygen_level);
            fields_read = 6;
        }
        else if (strstr(line, "Steps:") && fields_read >= 6) {
            sscanf(line, "Steps: %d", &current.steps);
            fields_read = 7;
        }
    }

    // Don't forget the last record
    if (fields_read == 7 && *count < MAX_RECORDS) {
        records[*count] = current;
        (*count)++;
    }

    fclose(file);
    return (*count > 0);
}

void calculate_statistics(HealthRecord records[], int count, HealthStats *stats) {
    float sum_hr = 0, sum_sys = 0, sum_dia = 0, sum_sugar = 0, sum_temp = 0, sum_oxy = 0;
    int total_steps = 0;

    for (int i = 0; i < count; i++) {
        sum_hr += records[i].heart_rate;
        sum_sys += records[i].systolic_bp;
        sum_dia += records[i].diastolic_bp;
        sum_sugar += records[i].blood_sugar;
        sum_temp += records[i].temperature;
        sum_oxy += records[i].oxygen_level;
        total_steps += records[i].steps;
    }

    stats->record_count = count;
    stats->avg_heart_rate = sum_hr / count;
    stats->avg_systolic = sum_sys / count;
    stats->avg_diastolic = sum_dia / count;
    stats->avg_blood_sugar = sum_sugar / count;
    stats->avg_temperature = sum_temp / count;
    stats->avg_oxygen = sum_oxy / count;
    stats->total_steps = total_steps;
}

void analyze_health(HealthRecord records[], int count, HealthStats stats, Alert alerts[], int *alert_count) {
    *alert_count = 0;

    // Heart Rate Analysis
    if (stats.avg_heart_rate > 100) {
        sprintf(alerts[*alert_count].message,
                "Average heart rate is %.0f BPM - Possible tachycardia detected",
                stats.avg_heart_rate);
        alerts[*alert_count].severity = (stats.avg_heart_rate > 120) ? 4 : 3;
        (*alert_count)++;
    } else if (stats.avg_heart_rate < 60) {
        sprintf(alerts[*alert_count].message,
                "Average heart rate is %.0f BPM - Bradycardia detected",
                stats.avg_heart_rate);
        alerts[*alert_count].severity = (stats.avg_heart_rate < 40) ? 4 : 2;
        (*alert_count)++;
    }

    // Blood Pressure Analysis
    if (stats.avg_systolic > 140 || stats.avg_diastolic > 90) {
        sprintf(alerts[*alert_count].message,
                "Average BP is %.0f/%.0f mmHg - Hypertension (Stage 2)",
                stats.avg_systolic, stats.avg_diastolic);
        alerts[*alert_count].severity = 4;
        (*alert_count)++;
    } else if (stats.avg_systolic > 130 || stats.avg_diastolic > 80) {
        sprintf(alerts[*alert_count].message,
                "Average BP is %.0f/%.0f mmHg - Hypertension (Stage 1)",
                stats.avg_systolic, stats.avg_diastolic);
        alerts[*alert_count].severity = 3;
        (*alert_count)++;
    } else if (stats.avg_systolic < 90 || stats.avg_diastolic < 60) {
        sprintf(alerts[*alert_count].message,
                "Average BP is %.0f/%.0f mmHg - Hypotension detected",
                stats.avg_systolic, stats.avg_diastolic);
        alerts[*alert_count].severity = 2;
        (*alert_count)++;
    }

    // Blood Sugar Analysis
    if (stats.avg_blood_sugar > 200) {
        sprintf(alerts[*alert_count].message,
                "Average blood sugar is %.0f mg/dL - Severe hyperglycemia",
                stats.avg_blood_sugar);
        alerts[*alert_count].severity = 4;
        (*alert_count)++;
    } else if (stats.avg_blood_sugar > 125) {
        sprintf(alerts[*alert_count].message,
                "Average blood sugar is %.0f mg/dL - Diabetes risk detected",
                stats.avg_blood_sugar);
        alerts[*alert_count].severity = 3;
        (*alert_count)++;
    } else if (stats.avg_blood_sugar < 70) {
        sprintf(alerts[*alert_count].message,
                "Average blood sugar is %.0f mg/dL - Hypoglycemia detected",
                stats.avg_blood_sugar);
        alerts[*alert_count].severity = 3;
        (*alert_count)++;
    }

    // Temperature Analysis
    if (stats.avg_temperature > 100.4) {
        sprintf(alerts[*alert_count].message,
                "Average temperature is %.1f F - Fever detected",
                stats.avg_temperature);
        alerts[*alert_count].severity = 3;
        (*alert_count)++;
    } else if (stats.avg_temperature < 95.0) {
        sprintf(alerts[*alert_count].message,
                "Average temperature is %.1f F - Hypothermia risk",
                stats.avg_temperature);
        alerts[*alert_count].severity = 4;
        (*alert_count)++;
    }

    // Oxygen Level Analysis
    if (stats.avg_oxygen < 90) {
        sprintf(alerts[*alert_count].message,
                "Average oxygen saturation is %.0f%% - Hypoxemia (Critical)",
                stats.avg_oxygen);
        alerts[*alert_count].severity = 4;
        (*alert_count)++;
    } else if (stats.avg_oxygen < 95) {
        sprintf(alerts[*alert_count].message,
                "Average oxygen saturation is %.0f%% - Low oxygen levels",
                stats.avg_oxygen);
        alerts[*alert_count].severity = 2;
        (*alert_count)++;
    }

    // Physical Activity Analysis
    int avg_steps = stats.total_steps / count;
    if (avg_steps < 5000) {
        sprintf(alerts[*alert_count].message,
                "Average daily steps: %d - Sedentary lifestyle detected",
                avg_steps);
        alerts[*alert_count].severity = 2;
        (*alert_count)++;
    }

    // Trend Analysis - Check last 3 records
    if (count >= 3) {
        int increasing_hr = 1, increasing_bp = 1;
        for (int i = count - 3; i < count - 1; i++) {
            if (records[i].heart_rate >= records[i + 1].heart_rate) increasing_hr = 0;
            if (records[i].systolic_bp >= records[i + 1].systolic_bp) increasing_bp = 0;
        }

        if (increasing_hr) {
            sprintf(alerts[*alert_count].message,
                    "Heart rate showing consistent upward trend");
            alerts[*alert_count].severity = 2;
            (*alert_count)++;
        }

        if (increasing_bp) {
            sprintf(alerts[*alert_count].message,
                    "Blood pressure showing consistent upward trend");
            alerts[*alert_count].severity = 2;
            (*alert_count)++;
        }
    }
}

void display_report(HealthStats stats, Alert alerts[], int alert_count) {
    printf("\n");
    print_line('=', 60);
    printf("              HEALTH ANALYSIS REPORT\n");
    print_line('=', 60);

    printf("\nVITAL STATISTICS (Based on %d records)\n", stats.record_count);
    print_line('-', 60);

    printf("  Heart Rate:      %.0f BPM", stats.avg_heart_rate);
    if (stats.avg_heart_rate >= 60 && stats.avg_heart_rate <= 100)
        printf(" [NORMAL]\n");
    else
        printf(" [ABNORMAL]\n");

    printf("  Blood Pressure:  %.0f/%.0f mmHg",
           stats.avg_systolic, stats.avg_diastolic);
    if (stats.avg_systolic < 120 && stats.avg_diastolic < 80)
        printf(" [NORMAL]\n");
    else
        printf(" [ELEVATED]\n");

    printf("  Blood Sugar:     %.0f mg/dL", stats.avg_blood_sugar);
    if (stats.avg_blood_sugar >= 70 && stats.avg_blood_sugar <= 125)
        printf(" [NORMAL]\n");
    else
        printf(" [ABNORMAL]\n");

    printf("  Temperature:     %.1f F", stats.avg_temperature);
    if (stats.avg_temperature >= 97.0 && stats.avg_temperature <= 99.0)
        printf(" [NORMAL]\n");
    else
        printf(" [ABNORMAL]\n");

    printf("  Oxygen Level:    %.0f%%", stats.avg_oxygen);
    if (stats.avg_oxygen >= 95)
        printf(" [NORMAL]\n");
    else
        printf(" [LOW]\n");

    printf("  Total Steps:     %d steps\n", stats.total_steps);
    printf("  Avg Daily Steps: %d steps/day\n",
           stats.total_steps / stats.record_count);

    // Health Score
    int health_score = calculate_health_score(stats);
    display_health_score(health_score);

    // Alerts
    if (alert_count > 0) {
        printf("\nHEALTH ALERTS\n");
        print_line('-', 60);

        for (int i = 0; i < alert_count; i++) {
            switch (alerts[i].severity) {
                case 4:
                    printf("  [CRITICAL] %s\n", alerts[i].message);
                    break;
                case 3:
                    printf("  [HIGH]     %s\n", alerts[i].message);
                    break;
                case 2:
                    printf("  [MEDIUM]   %s\n", alerts[i].message);
                    break;
                default:
                    printf("  [LOW]      %s\n", alerts[i].message);
            }
        }
    } else {
        printf("\n[SUCCESS] All vitals are within normal ranges! Keep up the good work!\n");
    }
    printf("\n");
}

int calculate_health_score(HealthStats stats) {
    int score = 100;

    // Heart rate
    if (stats.avg_heart_rate < 60 || stats.avg_heart_rate > 100) score -= 15;
    else if (stats.avg_heart_rate < 65 || stats.avg_heart_rate > 95) score -= 5;

    // Blood pressure
    if (stats.avg_systolic > 140 || stats.avg_diastolic > 90) score -= 20;
    else if (stats.avg_systolic > 130 || stats.avg_diastolic > 80) score -= 10;
    else if (stats.avg_systolic < 90 || stats.avg_diastolic < 60) score -= 15;

    // Blood sugar
    if (stats.avg_blood_sugar > 200 || stats.avg_blood_sugar < 70) score -= 25;
    else if (stats.avg_blood_sugar > 125) score -= 15;

    // Temperature
    if (stats.avg_temperature > 100.4 || stats.avg_temperature < 95.0) score -= 15;
    else if (stats.avg_temperature > 99.5 || stats.avg_temperature < 97.0) score -= 5;

    // Oxygen
    if (stats.avg_oxygen < 90) score -= 25;
    else if (stats.avg_oxygen < 95) score -= 10;

    // Steps
    int avg_steps = stats.total_steps / stats.record_count;
    if (avg_steps < 5000) score -= 10;
    else if (avg_steps > 10000) score += 5;

    if (score < 0) score = 0;
    if (score > 100) score = 100;

    return score;
}

void display_health_score(int score) {
    printf("\n");
    print_line('-', 50);
    printf("          OVERALL HEALTH SCORE\n");
    print_line('-', 50);

    // Score bar
    printf("  [");
    int bars = score / 5;
    for (int i = 0; i < 20; i++) {
        if (i < bars) printf("#");
        else printf("-");
    }
    printf("]\n");

    printf("          ");
    if (score >= 80) printf("%d/100 - EXCELLENT!\n", score);
    else if (score >= 60) printf("%d/100 - GOOD\n", score);
    else if (score >= 40) printf("%d/100 - FAIR\n", score);
    else printf("%d/100 - POOR\n", score);

    print_line('-', 50);
}

void display_trends(HealthRecord records[], int count) {
    printf("\n");
    print_line('=', 70);
    printf("                  HEALTH TRENDS\n");
    print_line('=', 70);

    int display_count = (count > 10) ? 10 : count;
    int start = count - display_count;

    printf("\nLast %d Records:\n", display_count);
    print_line('-', 70);
    printf("%-12s  HR   BP       Sugar  Temp   SpO2  Steps\n", "Date");
    print_line('-', 70);

    for (int i = start; i < count; i++) {
        printf("%-12s  %3d  %3d/%-3d  %3d    %.1f   %2d%%  %5d\n",
               records[i].date,
               records[i].heart_rate,
               records[i].systolic_bp,
               records[i].diastolic_bp,
               records[i].blood_sugar,
               records[i].temperature,
               records[i].oxygen_level,
               records[i].steps);
    }
    print_line('-', 70);
}

void generate_advice(Alert alerts[], int alert_count) {
    printf("\n");
    print_line('=', 60);
    printf("              PERSONALIZED HEALTH ADVICE\n");
    print_line('=', 60);

    int has_heart_issue = 0, has_bp_issue = 0, has_sugar_issue = 0;
    int has_oxygen_issue = 0, has_activity_issue = 0;

    for (int i = 0; i < alert_count; i++) {
        if (strstr(alerts[i].message, "heart rate")) has_heart_issue = 1;
        if (strstr(alerts[i].message, "pressure") || strstr(alerts[i].message, "BP")) has_bp_issue = 1;
        if (strstr(alerts[i].message, "sugar")) has_sugar_issue = 1;
        if (strstr(alerts[i].message, "oxygen")) has_oxygen_issue = 1;
        if (strstr(alerts[i].message, "steps")) has_activity_issue = 1;
    }

    if (alert_count == 0) {
        printf("\n[SUCCESS] Excellent! Your health metrics are optimal.\n\n");
        printf("General Wellness Tips:\n");
        printf("  * Maintain your current healthy lifestyle\n");
        printf("  * Stay hydrated (8 glasses of water daily)\n");
        printf("  * Continue regular physical activity\n");
        printf("  * Get 7-9 hours of quality sleep\n");
        printf("  * Practice stress management techniques\n");
        return;
    }

    printf("\nRECOMMENDED ACTIONS:\n");
    print_line('-', 60);

    if (has_heart_issue) {
        printf("\nFor Heart Rate Issues:\n");
        printf("  * Consult a cardiologist for proper evaluation\n");
        printf("  * Practice deep breathing exercises\n");
        printf("  * Reduce caffeine and stimulant intake\n");
        printf("  * Manage stress through meditation or yoga\n");
        printf("  * Ensure adequate sleep (7-9 hours)\n");
    }

    if (has_bp_issue) {
        printf("\nFor Blood Pressure Issues:\n");
        printf("  * Reduce sodium intake (<2300mg/day)\n");
        printf("  * Eat more fruits, vegetables, and whole grains\n");
        printf("  * Maintain healthy weight\n");
        printf("  * Limit alcohol consumption\n");
        printf("  * Exercise regularly (30 min/day, 5 days/week)\n");
        printf("  * Monitor BP daily and keep a log\n");
    }

    if (has_sugar_issue) {
        printf("\nFor Blood Sugar Issues:\n");
        printf("  * Consult an endocrinologist\n");
        printf("  * Follow a balanced, low-glycemic diet\n");
        printf("  * Eat smaller, frequent meals\n");
        printf("  * Increase fiber intake\n");
        printf("  * Exercise regularly to improve insulin sensitivity\n");
        printf("  * Monitor blood sugar levels consistently\n");
    }

    if (has_oxygen_issue) {
        printf("\nFor Oxygen Level Issues:\n");
        printf("  * [URGENT] SEEK IMMEDIATE MEDICAL ATTENTION if severe\n");
        printf("  * Practice breathing exercises\n");
        printf("  * Ensure proper ventilation in living spaces\n");
        printf("  * Avoid smoking and secondhand smoke\n");
        printf("  * Consider pulmonary function tests\n");
    }

    if (has_activity_issue) {
        printf("\nFor Low Physical Activity:\n");
        printf("  * Start with 10-minute walks, gradually increase\n");
        printf("  * Take stairs instead of elevators\n");
        printf("  * Set hourly reminders to stand and stretch\n");
        printf("  * Find activities you enjoy (dancing, sports, etc.)\n");
        printf("  * Use a fitness tracker for motivation\n");
        printf("  * Aim for 10,000 steps per day\n");
    }

    printf("\n[IMPORTANT DISCLAIMER]\n");
    printf("This is an automated analysis tool and NOT a substitute\n");
    printf("for professional medical advice. Please consult healthcare\n");
    printf("professionals for proper diagnosis and treatment.\n");
}

void add_manual_record(HealthRecord records[], int *count) {
    if (*count >= MAX_RECORDS) {
        printf("[ERROR] Maximum records reached!\n");
        return;
    }

    printf("\n");
    print_line('=', 60);
    printf("              ADD NEW HEALTH RECORD\n");
    print_line('=', 60);

    HealthRecord new_record;

    printf("\nEnter date (YYYY-MM-DD): ");
    scanf("%s", new_record.date);

    printf("Enter heart rate (BPM): ");
    scanf("%d", &new_record.heart_rate);

    printf("Enter systolic BP: ");
    scanf("%d", &new_record.systolic_bp);

    printf("Enter diastolic BP: ");
    scanf("%d", &new_record.diastolic_bp);

    printf("Enter blood sugar (mg/dL): ");
    scanf("%d", &new_record.blood_sugar);

    printf("Enter temperature (F): ");
    scanf("%f", &new_record.temperature);

    printf("Enter oxygen level (%%): ");
    scanf("%d", &new_record.oxygen_level);

    printf("Enter steps: ");
    scanf("%d", &new_record.steps);

    records[*count] = new_record;
    (*count)++;

    printf("\n[SUCCESS] Record added successfully!\n");
}

void export_report(HealthStats stats, Alert alerts[], int alert_count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("[ERROR] Failed to create report file.\n");
        return;
    }

    time_t now = time(NULL);
    char *timestamp = ctime(&now);

    fprintf(file, "============================================================\n");
    fprintf(file, "           SMART HEALTH MONITOR - ANALYSIS REPORT\n");
    fprintf(file, "============================================================\n");
    fprintf(file, "Generated: %s\n", timestamp);
    fprintf(file, "Based on %d health records\n\n", stats.record_count);

    fprintf(file, "VITAL STATISTICS SUMMARY\n");
    fprintf(file, "------------------------------------------------------------\n");
    fprintf(file, "Average Heart Rate:      %.0f BPM\n", stats.avg_heart_rate);
    fprintf(file, "Average Blood Pressure:  %.0f/%.0f mmHg\n", stats.avg_systolic, stats.avg_diastolic);
    fprintf(file, "Average Blood Sugar:     %.0f mg/dL\n", stats.avg_blood_sugar);
    fprintf(file, "Average Temperature:     %.1f F\n", stats.avg_temperature);
    fprintf(file, "Average Oxygen Level:    %.0f%%\n", stats.avg_oxygen);
    fprintf(file, "Total Steps:             %d steps\n", stats.total_steps);
    fprintf(file, "Average Daily Steps:     %d steps/day\n\n", stats.total_steps / stats.record_count);

    int health_score = calculate_health_score(stats);
    fprintf(file, "OVERALL HEALTH SCORE: %d/100\n", health_score);
    if (health_score >= 80) fprintf(file, "Status: EXCELLENT\n\n");
    else if (health_score >= 60) fprintf(file, "Status: GOOD\n\n");
    else if (health_score >= 40) fprintf(file, "Status: FAIR\n\n");
    else fprintf(file, "Status: POOR - NEEDS ATTENTION\n\n");

    if (alert_count > 0) {
        fprintf(file, "HEALTH ALERTS\n");
        fprintf(file, "------------------------------------------------------------\n");
        for (int i = 0; i < alert_count; i++) {
            switch (alerts[i].severity) {
                case 4: fprintf(file, "[CRITICAL] "); break;
                case 3: fprintf(file, "[HIGH]     "); break;
                case 2: fprintf(file, "[MEDIUM]   "); break;
                default: fprintf(file, "[LOW]      "); break;
            }
            fprintf(file, "%s\n", alerts[i].message);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "DISCLAIMER\n");
    fprintf(file, "------------------------------------------------------------\n");
    fprintf(file, "This report is generated by an automated analysis system\n");
    fprintf(file, "and is NOT a substitute for professional medical advice.\n");
    fprintf(file, "Please consult healthcare professionals for proper\n");
    fprintf(file, "diagnosis and treatment.\n");
    fprintf(file, "============================================================\n");

    fclose(file);
    printf("\n[SUCCESS] Report exported successfully to '%s'\n", filename);
}

void create_sample_data() {
    printf("\n");
    print_line('=', 60);
    printf("              CREATE SAMPLE DATA FILE\n");
    print_line('=', 60);

    printf("\nSelect format:\n");
    printf("1. CSV Format\n");
    printf("2. TXT Format\n");
    printf("\nEnter choice: ");

    int choice;
    scanf("%d", &choice);
    getchar();

    if (choice == 1) {
        FILE *file = fopen("sample_health_data.csv", "w");
        if (!file) {
            printf("[ERROR] Failed to create file.\n");
            return;
        }

        // Write header
        fprintf(file, "Date,HeartRate,SystolicBP,DiastolicBP,BloodSugar,Temperature,OxygenLevel,Steps\n");

        // Write 7 days of data
        fprintf(file, "2025-10-26,72,118,78,95,98.2,98,8500\n");
        fprintf(file, "2025-10-27,75,120,80,102,98.4,97,9200\n");
        fprintf(file, "2025-10-28,78,122,82,98,98.6,98,7800\n");
        fprintf(file, "2025-10-29,115,145,95,180,99.1,96,4500\n");
        fprintf(file, "2025-10-30,118,148,96,185,99.3,95,4200\n");
        fprintf(file, "2025-10-31,120,150,98,190,99.5,94,3800\n");
        fprintf(file, "2025-11-01,122,152,99,195,99.8,93,3500\n");

        fclose(file);
        printf("[SUCCESS] Sample CSV created: sample_health_data.csv\n");
        printf("File location: In your project's bin/Debug folder\n");
        printf("This file contains 7 days of health data.\n");

    } else if (choice == 2) {
        FILE *file = fopen("sample_health_data.txt", "w");
        if (!file) {
            printf("[ERROR] Failed to create file.\n");
            return;
        }

        // Record 1
        fprintf(file, "Date: 2025-10-26\n");
        fprintf(file, "Heart Rate: 72\n");
        fprintf(file, "Blood Pressure: 118/78\n");
        fprintf(file, "Blood Sugar: 95\n");
        fprintf(file, "Temperature: 98.2\n");
        fprintf(file, "Oxygen Level: 98\n");
        fprintf(file, "Steps: 8500\n");
        fprintf(file, "\n");

        // Record 2
        fprintf(file, "Date: 2025-10-27\n");
        fprintf(file, "Heart Rate: 75\n");
        fprintf(file, "Blood Pressure: 120/80\n");
        fprintf(file, "Blood Sugar: 102\n");
        fprintf(file, "Temperature: 98.4\n");
        fprintf(file, "Oxygen Level: 97\n");
        fprintf(file, "Steps: 9200\n");
        fprintf(file, "\n");

        // Record 3
        fprintf(file, "Date: 2025-10-28\n");
        fprintf(file, "Heart Rate: 78\n");
        fprintf(file, "Blood Pressure: 122/82\n");
        fprintf(file, "Blood Sugar: 98\n");
        fprintf(file, "Temperature: 98.6\n");
        fprintf(file, "Oxygen Level: 98\n");
        fprintf(file, "Steps: 7800\n");
        fprintf(file, "\n");

        // Record 4
        fprintf(file, "Date: 2025-10-29\n");
        fprintf(file, "Heart Rate: 115\n");
        fprintf(file, "Blood Pressure: 145/95\n");
        fprintf(file, "Blood Sugar: 180\n");
        fprintf(file, "Temperature: 99.1\n");
        fprintf(file, "Oxygen Level: 96\n");
        fprintf(file, "Steps: 4500\n");
        fprintf(file, "\n");

        // Record 5
        fprintf(file, "Date: 2025-10-30\n");
        fprintf(file, "Heart Rate: 118\n");
        fprintf(file, "Blood Pressure: 148/96\n");
        fprintf(file, "Blood Sugar: 185\n");
        fprintf(file, "Temperature: 99.3\n");
        fprintf(file, "Oxygen Level: 95\n");
        fprintf(file, "Steps: 4200\n");
        fprintf(file, "\n");

        // Record 6
        fprintf(file, "Date: 2025-10-31\n");
        fprintf(file, "Heart Rate: 120\n");
        fprintf(file, "Blood Pressure: 150/98\n");
        fprintf(file, "Blood Sugar: 190\n");
        fprintf(file, "Temperature: 99.5\n");
        fprintf(file, "Oxygen Level: 94\n");
        fprintf(file, "Steps: 3800\n");
        fprintf(file, "\n");

        // Record 7
        fprintf(file, "Date: 2025-11-01\n");
        fprintf(file, "Heart Rate: 122\n");
        fprintf(file, "Blood Pressure: 152/99\n");
        fprintf(file, "Blood Sugar: 195\n");
        fprintf(file, "Temperature: 99.8\n");
        fprintf(file, "Oxygen Level: 93\n");
        fprintf(file, "Steps: 3500\n");

        fclose(file);
        printf("[SUCCESS] Sample TXT created: sample_health_data.txt\n");
        printf("File location: In your project's bin/Debug folder\n");
        printf("This file contains 7 days of health data.\n");
    } else {
        printf("[ERROR] Invalid choice.\n");
    }
}

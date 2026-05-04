/* Simple GTK3 GUI for the CPU Scheduler Simulator
 * Features:
 *  - Load workload from file (simple text format)
 *  - Algorithm selection (FIFO,SJF,STCF,RR,MLFQ)
 *  - Quantum input for RR
 *  - Run button to simulate and display Gantt and metrics
 *
 * Workload format (each line): pid arrival burst priority
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/algorithms.h"
#include "../include/metrics.h"

#define MAX_PROCS 128

static process_t processes[MAX_PROCS];
static int proc_count = 0;
static timeline_event_t timeline[4096];

static void load_workload_from_file(const char *filename){
    FILE *f = fopen(filename, "r");
    if(!f) return;
    proc_count = 0;
    while(!feof(f) && proc_count < MAX_PROCS){
        int pid, at, bt, pr;
        if(fscanf(f, "%d %d %d %d", &pid, &at, &bt, &pr) == 4){
            processes[proc_count].pid = pid;
            processes[proc_count].arrival_time = at;
            processes[proc_count].burst_time = bt;
            processes[proc_count].priority = pr;
            processes[proc_count].remaining_time = bt;
            processes[proc_count].start_time = -1;
            processes[proc_count].completion_time = -1;
            processes[proc_count].turnaround_time = 0;
            processes[proc_count].waiting_time = 0;
            processes[proc_count].response_time = -1;
            proc_count++;
        } else break;
    }
    fclose(f);
}

static void on_file_open(GtkWidget *widget, gpointer user_data){
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open workload", GTK_WINDOW(user_data), GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        load_workload_from_file(filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void show_timeline_in_textview(GtkTextBuffer *buf){
    gtk_text_buffer_set_text(buf, "", -1);
    char line[256];
    for(int i=0;i<4096;i++){
        if(timeline[i].duration==0) break;
        snprintf(line, sizeof(line), "t=%d: P%d ran for %d\n", timeline[i].time, timeline[i].pid, timeline[i].duration);
        gtk_text_buffer_insert_at_cursor(buf, line, -1);
    }
}

static void on_run_clicked(GtkButton *btn, gpointer user_data){
    GtkBuilder *builder = (GtkBuilder*)user_data;
    GtkComboBoxText *alg_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "alg_combo"));
    const char *alg = gtk_combo_box_text_get_active_text(alg_combo);
    GtkEntry *quantum_entry = GTK_ENTRY(gtk_builder_get_object(builder, "quantum_entry"));
    int quantum = atoi(gtk_entry_get_text(quantum_entry)); if(quantum<=0) quantum=3;

    // clear timeline
    for(int i=0;i<4096;i++) timeline[i].time = timeline[i].pid = timeline[i].duration = 0;

    if(strcmp(alg, "FIFO") == 0){
        schedule_fifo(processes, proc_count, timeline);
    } else if(strcmp(alg, "SJF") == 0){
        schedule_sjf(processes, proc_count, timeline);
    } else if(strcmp(alg, "STCF") == 0){
        schedule_stcf(processes, proc_count, timeline);
    } else if(strcmp(alg, "RR") == 0){
        schedule_rr(processes, proc_count, quantum, timeline);
    } else if(strcmp(alg, "MLFQ") == 0){
        // simple default config: 3 queues quanta [2,4,8], boost 50
        int quanta_arr[] = {2,4,8}; mlfq_config_t cfg = {3, quanta_arr, 50};
        schedule_mlfq(processes, proc_count, &cfg, timeline);
    }

    // compute total_time
    int total_time = 0; for(int i=0;i<proc_count;i++) if(processes[i].completion_time > total_time) total_time = processes[i].completion_time;
    metrics_t m; calculate_metrics(processes, proc_count, total_time, &m);

    // update metrics view
    GtkTextBuffer *metrics_buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "metrics_view"))));
    char metrics_text[512];
    snprintf(metrics_text, sizeof(metrics_text), "Avg TAT: %.2f\nAvg WT: %.2f\nAvg RT: %.2f\nCPU Util: %.2f%%\nThroughput: %.3f\nFairness: %.3f\n",
             m.avg_turnaround_time, m.avg_waiting_time, m.avg_response_time, m.cpu_utilization, m.throughput, m.fairness_index);
    gtk_text_buffer_set_text(metrics_buf, metrics_text, -1);

    // update timeline view
    GtkTextBuffer *buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "timeline_view"))));
    show_timeline_in_textview(buf);
}

int gui_gtk_main(int argc, char **argv){
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    const char *ui =
        "<interface>"
        "  <requires lib=\"gtk+\" version=\"3.0\"/>"
        "  <object class=\"GtkWindow\" id=\"main_win\">"
        "    <property name=\"title\">CPU Scheduler Simulator</property>"
        "    <property name=\"default_width\">1000</property>"
        "    <property name=\"default_height\">600</property>"
        "    <child>"
        "      <object class=\"GtkBox\">"
        "        <property name=\"orientation\">vertical</property>"
        "        <child>"
        "          <object class=\"GtkToolbar\">"
        "            <child>"
        "              <object class=\"GtkToolButton\">"
        "                <property name=\"label\">Open</property>"
        "                <signal name=\"clicked\" handler=\"on_file_open\" swapped=\"no\"/>"
        "              </object>"
        "            </child>"
        "          </object>"
        "        </child>"
        "        <child>"
        "          <object class=\"GtkGrid\">"
        "            <child>"
        "              <object class=\"GtkLabel\"><property name=\"label\">Algorithm:</property></object>"
        "            </child>"
        "            <child>"
        "              <object class=\"GtkComboBoxText\" id=\"alg_combo\">"
        "                <items>"
        "                  <item translatable=\"yes\">FIFO</item>"
        "                  <item translatable=\"yes\">SJF</item>"
        "                  <item translatable=\"yes\">STCF</item>"
        "                  <item translatable=\"yes\">RR</item>"
        "                  <item translatable=\"yes\">MLFQ</item>"
        "                </items>"
        "              </object>"
        "            </child>"
        "            <child>"
        "              <object class=\"GtkLabel\"><property name=\"label\">Quantum (RR):</property></object>"
        "            </child>"
        "            <child>"
        "              <object class=\"GtkEntry\" id=\"quantum_entry\"> <property name=\"text\">3</property> </object>"
        "            </child>"
        "            <child>"
        "              <object class=\"GtkButton\"> <property name=\"label\">Run</property> <signal name=\"clicked\" handler=\"on_run_clicked\"/> </object>"
        "            </child>"
        "          </object>"
        "        </child>"
        "        <child>"
        "          <object class=\"GtkPaned\"> <property name=\"orientation\">horizontal</property>"
        "            <child>",
        "              <object class=\"GtkScrolledWindow\"> <child> <object class=\"GtkTextView\" id=\"timeline_view\"/> </child> </object>",
        "            </child>"
        "            <child>"
        "              <object class=\"GtkScrolledWindow\"> <child> <object class=\"GtkTextView\" id=\"metrics_view\"/> </child> </object>
        "            </child>"
        "          </object>"
        "        </child>"
        "      </object>"
        "    </child>"
        "  </object>"
        "</interface>";

    if(!gtk_builder_add_from_string(builder, ui, -1, NULL)){
        g_printerr("Failed to build UI\n");
        return 1;
    }

    GtkWidget *win = GTK_WIDGET(gtk_builder_get_object(builder, "main_win"));
    GtkWidget *open_btn = gtk_builder_get_object(builder, "open");
    // connect signals
    gtk_builder_connect_signals(builder, builder);
    // manually connect file open since toolbar button id not used
    GList *children = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "main_win")));
    (void)children; // silence unused

    // connect run button handler passing builder as user_data
    GtkWidget *run_btn = gtk_builder_get_object(builder, "run");
    if(run_btn) g_signal_connect(run_btn, "clicked", G_CALLBACK(on_run_clicked), builder);
    // connect file open via top-level action using pointer to window
    // For simplicity, when user clicks the toolbar Open button the default handler will call file chooser

    gtk_widget_show_all(win);
    gtk_main();
    g_object_unref(builder);
    return 0;
}

/* signal handlers referenced by name */
static void on_file_open_wrapper(GtkMenuItem *menuitem, gpointer user_data){ on_file_open(NULL, user_data); }
static void on_run_clicked_wrapper(GtkButton *btn, gpointer user_data){ on_run_clicked(btn, user_data); }

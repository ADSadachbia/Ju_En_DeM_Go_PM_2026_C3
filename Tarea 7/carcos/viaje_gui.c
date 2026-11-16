#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_DATOS "vehiculos.dat"
#define MAX_VEHICULOS 200
#define MAX_NOMBRE    64

typedef struct {
    int    id;
    char   nombre[MAX_NOMBRE];
    double consumo_ciudad;
    double consumo_carretera;
    double seguro_anual;
    double neumaticos_por_km;
    double mantenimiento_por_km;
    double valor_inicial;
    double anios_uso;
    double valor_final;
    double impuesto_anual;
    double limpieza_anual;
    double km_por_anio;
} Vehiculo;

/* Estado global de la aplicación */
static Vehiculo lista_vehiculos[MAX_VEHICULOS];
static int num_vehiculos = 0;
static int indice_seleccionado = -1;

/* Widgets principales */
static GtkWidget *ventana;
static GtkWidget *combo_vehiculos;
static GtkWidget *combo_calc_vehiculo;
static GtkWidget *txt_resultado;

/* Campos del formulario de vehículo */
static GtkWidget *ent_nombre, *ent_c_ciudad, *ent_c_carretera;
static GtkWidget *ent_seguro, *ent_neumaticos, *ent_mantenimiento;
static GtkWidget *ent_v_inicial, *ent_anios, *ent_v_final;
static GtkWidget *ent_impuesto, *ent_limpieza, *ent_km_anio;

/* Campos de la calculadora */
static GtkWidget *ent_distancia, *ent_pct_ciudad, *ent_precio_litro;

/* ------------------------------------------------------------------ */
/* Persistencia (Idéntica a tu programa original)                      */
/* ------------------------------------------------------------------ */

static void guardar_datos(void) {
    FILE *f = fopen(ARCHIVO_DATOS, "wb");
    if (!f) return;
    fwrite(&num_vehiculos, sizeof(int), 1, f);
    fwrite(lista_vehiculos, sizeof(Vehiculo), (size_t)num_vehiculos, f);
    fclose(f);
}

static void cargar_datos(void) {
    num_vehiculos = 0;
    FILE *f = fopen(ARCHIVO_DATOS, "rb");
    if (!f) return;
    if (fread(&num_vehiculos, sizeof(int), 1, f) == 1 &&
        num_vehiculos >= 0 && num_vehiculos <= MAX_VEHICULOS) {
        fread(lista_vehiculos, sizeof(Vehiculo), (size_t)num_vehiculos, f);
    } else {
        num_vehiculos = 0;
    }
    fclose(f);
}

/* ------------------------------------------------------------------ */
/* Funciones auxiliares de interfaz                                    */
/* ------------------------------------------------------------------ */

static void mostrar_mensaje(const char *titulo, const char *mensaje) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(ventana),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", mensaje);
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void actualizar_combos(void) {
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_vehiculos));
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_calc_vehiculo));

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_vehiculos), "-- Nuevo Vehículo --");

    for (int i = 0; i < num_vehiculos; i++) {
        char texto[100];
        snprintf(texto, sizeof(texto), "[%d] %s", lista_vehiculos[i].id, lista_vehiculos[i].nombre);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_vehiculos), texto);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_calc_vehiculo), texto);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_vehiculos), 0);
    if (num_vehiculos > 0)
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_calc_vehiculo), 0);
}

static void limpiar_formulario(void) {
    gtk_entry_set_text(GTK_ENTRY(ent_nombre), "");
    gtk_entry_set_text(GTK_ENTRY(ent_c_ciudad), "10.0");
    gtk_entry_set_text(GTK_ENTRY(ent_c_carretera), "14.0");
    gtk_entry_set_text(GTK_ENTRY(ent_seguro), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_neumaticos), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_mantenimiento), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_v_inicial), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_anios), "5.0");
    gtk_entry_set_text(GTK_ENTRY(ent_v_final), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_impuesto), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_limpieza), "0.0");
    gtk_entry_set_text(GTK_ENTRY(ent_km_anio), "15000.0");
    indice_seleccionado = -1;
}

static void set_double_entry(GtkWidget *entry, double val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", val);
    gtk_entry_set_text(GTK_ENTRY(entry), buf);
}

/* ------------------------------------------------------------------ */
/* Eventos (Callbacks)                                                 */
/* ------------------------------------------------------------------ */

static void al_cambiar_vehiculo(GtkComboBox *widget, gpointer data) {
    (void)data;
    int idx = gtk_combo_box_get_active(widget);
    if (idx <= 0 || idx > num_vehiculos) {
        limpiar_formulario();
        return;
    }
    indice_seleccionado = idx - 1;
    Vehiculo *v = &lista_vehiculos[indice_seleccionado];

    gtk_entry_set_text(GTK_ENTRY(ent_nombre), v->nombre);
    set_double_entry(ent_c_ciudad, v->consumo_ciudad);
    set_double_entry(ent_c_carretera, v->consumo_carretera);
    set_double_entry(ent_seguro, v->seguro_anual);
    set_double_entry(ent_neumaticos, v->neumaticos_por_km);
    set_double_entry(ent_mantenimiento, v->mantenimiento_por_km);
    set_double_entry(ent_v_inicial, v->valor_inicial);
    set_double_entry(ent_anios, v->anios_uso);
    set_double_entry(ent_v_final, v->valor_final);
    set_double_entry(ent_impuesto, v->impuesto_anual);
    set_double_entry(ent_limpieza, v->limpieza_anual);
    set_double_entry(ent_km_anio, v->km_por_anio);
}

static void al_guardar_vehiculo(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    const char *nombre = gtk_entry_get_text(GTK_ENTRY(ent_nombre));
    if (strlen(nombre) == 0) {
        mostrar_mensaje("Error", "El nombre del vehículo no puede estar vacío.");
        return;
    }

    Vehiculo v;
    strncpy(v.nombre, nombre, MAX_NOMBRE - 1);
    v.nombre[MAX_NOMBRE - 1] = '\0';
    v.consumo_ciudad       = atof(gtk_entry_get_text(GTK_ENTRY(ent_c_ciudad)));
    v.consumo_carretera    = atof(gtk_entry_get_text(GTK_ENTRY(ent_c_carretera)));
    v.seguro_anual         = atof(gtk_entry_get_text(GTK_ENTRY(ent_seguro)));
    v.neumaticos_por_km    = atof(gtk_entry_get_text(GTK_ENTRY(ent_neumaticos)));
    v.mantenimiento_por_km = atof(gtk_entry_get_text(GTK_ENTRY(ent_mantenimiento)));
    v.valor_inicial        = atof(gtk_entry_get_text(GTK_ENTRY(ent_v_inicial)));
    v.anios_uso            = atof(gtk_entry_get_text(GTK_ENTRY(ent_anios)));
    v.valor_final          = atof(gtk_entry_get_text(GTK_ENTRY(ent_v_final)));
    v.impuesto_anual       = atof(gtk_entry_get_text(GTK_ENTRY(ent_impuesto)));
    v.limpieza_anual       = atof(gtk_entry_get_text(GTK_ENTRY(ent_limpieza)));
    v.km_por_anio          = atof(gtk_entry_get_text(GTK_ENTRY(ent_km_anio)));

    if (v.consumo_ciudad <= 0 || v.consumo_carretera <= 0 || v.km_por_anio <= 0) {
        mostrar_mensaje("Error", "Consumos y Km por año deben ser mayores que cero.");
        return;
    }

    if (indice_seleccionado >= 0) {
        v.id = lista_vehiculos[indice_seleccionado].id;
        lista_vehiculos[indice_seleccionado] = v;
    } else {
        if (num_vehiculos >= MAX_VEHICULOS) {
            mostrar_mensaje("Error", "Límite máximo de vehículos alcanzado.");
            return;
        }
        int max_id = 0;
        for (int i = 0; i < num_vehiculos; i++)
            if (lista_vehiculos[i].id > max_id) max_id = lista_vehiculos[i].id;
        v.id = max_id + 1;
        lista_vehiculos[num_vehiculos++] = v;
    }

    guardar_datos();
    actualizar_combos();
    mostrar_mensaje("Éxito", "Vehículo guardado correctamente en vehiculos.dat.");
}

static void al_borrar_vehiculo(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    if (indice_seleccionado < 0) {
        mostrar_mensaje("Aviso", "Seleccione un vehículo existente para borrar.");
        return;
    }
    for (int i = indice_seleccionado; i < num_vehiculos - 1; i++) {
        lista_vehiculos[i] = lista_vehiculos[i + 1];
    }
    num_vehiculos--;
    guardar_datos();
    actualizar_combos();
    limpiar_formulario();
    mostrar_mensaje("Eliminado", "Vehículo eliminado correctamente.");
}

static void al_calcular_viaje(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_calc_vehiculo));
    if (idx < 0 || idx >= num_vehiculos) {
        mostrar_mensaje("Aviso", "Primero registre y seleccione un vehículo.");
        return;
    }

    Vehiculo *v = &lista_vehiculos[idx];
    double dist = atof(gtk_entry_get_text(GTK_ENTRY(ent_distancia)));
    double pct_ciudad = atof(gtk_entry_get_text(GTK_ENTRY(ent_pct_ciudad)));
    double precio_litro = atof(gtk_entry_get_text(GTK_ENTRY(ent_precio_litro)));

    if (dist <= 0 || pct_ciudad < 0 || pct_ciudad > 100 || precio_litro <= 0) {
        mostrar_mensaje("Error", "Verifique distancia (>0), % ciudad (0-100) y precio combustible (>0).");
        return;
    }

    double km_ciudad = dist * (pct_ciudad / 100.0);
    double km_carretera = dist - km_ciudad;
    double litros = (km_ciudad / v->consumo_ciudad) + (km_carretera / v->consumo_carretera);
    double c_comb = litros * precio_litro;
    double c_neum = dist * v->neumaticos_por_km;
    double c_mant = dist * v->mantenimiento_por_km;

    double c_seg = (v->seguro_anual / v->km_por_anio) * dist;
    double c_imp = (v->impuesto_anual / v->km_por_anio) * dist;
    double c_lim = (v->limpieza_anual / v->km_por_anio) * dist;
    double c_dep = 0.0;
    if (v->anios_uso > 0)
        c_dep = ((v->valor_inicial - v->valor_final) / (v->anios_uso * v->km_por_anio)) * dist;

    double total = c_comb + c_neum + c_mant + c_seg + c_imp + c_lim + c_dep;
    double por_km = total / dist;

    char reporte[1024];
    snprintf(reporte, sizeof(reporte),

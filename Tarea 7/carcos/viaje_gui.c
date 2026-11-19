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
static GtkWidget *ent_distancia, *ent_pct_carretera, *ent_pct_ciudad, *ent_precio_litro;

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
    int leidos = 0;
    if (fread(&num_vehiculos, sizeof(int), 1, f) == 1 &&
        num_vehiculos >= 0 && num_vehiculos <= MAX_VEHICULOS) {
        leidos = (int)fread(lista_vehiculos, sizeof(Vehiculo), (size_t)num_vehiculos, f);
    }
    if (leidos != num_vehiculos) num_vehiculos = 0;
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
    GtkWidget *confirmar = gtk_message_dialog_new(GTK_WINDOW(ventana),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "¿Borrar el vehículo «%s»?", lista_vehiculos[indice_seleccionado].nombre);
    gint respuesta = gtk_dialog_run(GTK_DIALOG(confirmar));
    gtk_widget_destroy(confirmar);
    if (respuesta != GTK_RESPONSE_YES) return;

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
    double pct_carretera = atof(gtk_entry_get_text(GTK_ENTRY(ent_pct_carretera)));
    double pct_ciudad = atof(gtk_entry_get_text(GTK_ENTRY(ent_pct_ciudad)));
    double precio_litro = atof(gtk_entry_get_text(GTK_ENTRY(ent_precio_litro)));

    if (dist <= 0 || pct_carretera < 0 || pct_ciudad < 0 ||
        (pct_carretera + pct_ciudad) <= 0 || precio_litro <= 0) {
        mostrar_mensaje("Error", "Verifique distancia (>0), porcentajes (>=0 y que no sumen 0) y precio (>0).");
        return;
    }

    double suma = pct_carretera + pct_ciudad;
    double km_carretera = dist * pct_carretera / suma;
    double km_ciudad = dist - km_carretera;
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

    char reporte[2048];
    char *p = reporte;
    size_t restante = sizeof(reporte);
    int n;
#define ADD(...) do {                                                        \
        n = snprintf(p, restante, __VA_ARGS__);                              \
        if (n > 0) {                                                         \
            size_t usado = (size_t)n;                                        \
            if (usado >= restante) usado = restante - 1;                     \
            p += usado; restante -= usado;                                   \
        }                                                                    \
    } while (0)

    ADD("================ RESULTADO DEL VIAJE ================\n");
    ADD("Vehículo.......: [%d] %s\n", v->id, v->nombre);
    ADD("Distancia......: %.2f km (carretera %.2f km, ciudad %.2f km)\n",
        dist, km_carretera, km_ciudad);
    ADD("Porcentajes....: %.2f%% carretera, %.2f%% ciudad\n", pct_carretera, pct_ciudad);
    ADD("Combustible....: %.2f litros a RD$ %.2f/l\n", litros, precio_litro);
    if (suma < 99.99 || suma > 100.01)
        ADD("Nota: los porcentajes suman %.2f%%, se normalizan a 100%%.\n", suma);
    ADD("\n  %-14s %14s   %13s   %6s\n", "Concepto", "Costo", "Por km", "% total");
    ADD("  ------------------------------------------------------------\n");
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Combustible", c_comb, c_comb / dist, 100.0 * c_comb / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Neumáticos", c_neum, c_neum / dist, 100.0 * c_neum / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Mantenimiento", c_mant, c_mant / dist, 100.0 * c_mant / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Seguro", c_seg, c_seg / dist, 100.0 * c_seg / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Impuesto", c_imp, c_imp / dist, 100.0 * c_imp / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Limpieza", c_lim, c_lim / dist, 100.0 * c_lim / total);
    ADD("  %-14s RD$%11.2f   RD$%8.4f/km   %5.1f%%\n", "Depreciación", c_dep, c_dep / dist, 100.0 * c_dep / total);
    ADD("  ------------------------------------------------------------\n");
    ADD("  COSTO TOTAL DEL VIAJE: RD$ %.2f\n", total);
    ADD("  COSTO POR KILÓMETRO.: RD$ %.4f/km\n", por_km);
    ADD("  Fijos prorrateados con %.0f km/año\n", v->km_por_anio);
    ADD("=====================================================\n");
#undef ADD

    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(txt_resultado)), reporte, -1);
}

static void al_nuevo_vehiculo(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_vehiculos), 0);
    limpiar_formulario();
    gtk_widget_grab_focus(ent_nombre);
}

static void al_limpiar_resultado(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(txt_resultado)), "", -1);
}

static void al_guardar_reporte(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(txt_resultado));
    GtkTextIter inicio, fin;
    gtk_text_buffer_get_bounds(buffer, &inicio, &fin);
    char *texto = gtk_text_buffer_get_text(buffer, &inicio, &fin, FALSE);
    if (texto == NULL || texto[0] == '\0') {
        mostrar_mensaje("Aviso", "Primero calcula un viaje para generar el reporte.");
        g_free(texto);
        return;
    }
    FILE *f = fopen("reporte_viaje.txt", "w");
    if (f == NULL) {
        mostrar_mensaje("Error", "No se pudo crear reporte_viaje.txt.");
        g_free(texto);
        return;
    }
    fputs(texto, f);
    fclose(f);
    g_free(texto);
    mostrar_mensaje("Éxito", "Reporte guardado en reporte_viaje.txt");
}

/* ------------------------------------------------------------------ */
/* Construcción de la interfaz                                         */
/* ------------------------------------------------------------------ */

static void agregar_campo(GtkGrid *grid, int fila, const char *etiqueta, GtkWidget *entry)
{
    GtkWidget *lbl = gtk_label_new(etiqueta);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(grid, lbl, 0, fila, 1, 1);
    gtk_grid_attach(grid, entry, 1, fila, 1, 1);
    gtk_entry_set_width_chars(GTK_ENTRY(entry), 18);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    cargar_datos();

    ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ventana), "carcos - Costo por kilómetro");
    gtk_window_set_default_size(GTK_WINDOW(ventana), 760, 640);
    gtk_window_set_position(GTK_WINDOW(ventana), GTK_WIN_POS_CENTER);
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(ventana), notebook);

    /* ----- Pestaña: Vehículos ----- */
    GtkWidget *tab_vehiculos = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(tab_vehiculos), 12);

    GtkWidget *barra = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    combo_vehiculos = gtk_combo_box_text_new();
    GtkWidget *btn_nuevo = gtk_button_new_with_label("Nuevo");
    GtkWidget *btn_guardar = gtk_button_new_with_label("Guardar");
    GtkWidget *btn_borrar = gtk_button_new_with_label("Borrar");
    gtk_box_pack_start(GTK_BOX(barra), gtk_label_new("Vehículo:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(barra), combo_vehiculos, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(barra), btn_nuevo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(barra), btn_guardar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(barra), btn_borrar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_vehiculos), barra, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    ent_nombre = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ent_nombre), "Ej: Toyota Corolla");
    agregar_campo(GTK_GRID(grid), 0, "Nombre:", ent_nombre);
    ent_c_ciudad = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 1, "Consumo ciudad (km/l):", ent_c_ciudad);
    ent_c_carretera = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 2, "Consumo carretera (km/l):", ent_c_carretera);
    ent_seguro = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 3, "Seguro anual (RD$):", ent_seguro);
    ent_neumaticos = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 4, "Neumáticos (RD$/km):", ent_neumaticos);
    ent_mantenimiento = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 5, "Mantenimiento (RD$/km):", ent_mantenimiento);
    ent_v_inicial = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 6, "Valor inicial (RD$):", ent_v_inicial);
    ent_anios = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 7, "Años de uso:", ent_anios);
    ent_v_final = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 8, "Valor final (RD$):", ent_v_final);
    ent_impuesto = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 9, "Impuesto anual (RD$):", ent_impuesto);
    ent_limpieza = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 10, "Limpieza anual (RD$):", ent_limpieza);
    ent_km_anio = gtk_entry_new();
    agregar_campo(GTK_GRID(grid), 11, "Km por año:", ent_km_anio);

    GtkWidget *scroll_veh = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_veh),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll_veh), grid);
    gtk_box_pack_start(GTK_BOX(tab_vehiculos), scroll_veh, TRUE, TRUE, 0);

    /* ----- Pestaña: Calcular viaje ----- */
    GtkWidget *tab_calculo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(tab_calculo), 12);

    GtkWidget *grid_calc = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_calc), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid_calc), 10);
    combo_calc_vehiculo = gtk_combo_box_text_new();
    GtkWidget *lbl_veh_calc = gtk_label_new("Vehículo:");
    gtk_widget_set_halign(lbl_veh_calc, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid_calc), lbl_veh_calc, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_calc), combo_calc_vehiculo, 1, 0, 1, 1);

    ent_distancia = gtk_entry_new();
    agregar_campo(GTK_GRID(grid_calc), 1, "Distancia total (km):", ent_distancia);
    ent_pct_carretera = gtk_entry_new();
    agregar_campo(GTK_GRID(grid_calc), 2, "% en carretera:", ent_pct_carretera);
    ent_pct_ciudad = gtk_entry_new();
    agregar_campo(GTK_GRID(grid_calc), 3, "% en ciudad:", ent_pct_ciudad);
    ent_precio_litro = gtk_entry_new();
    agregar_campo(GTK_GRID(grid_calc), 4, "Precio combustible (RD$/l):", ent_precio_litro);
    gtk_box_pack_start(GTK_BOX(tab_calculo), grid_calc, FALSE, FALSE, 0);

    GtkWidget *botones = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *btn_calcular = gtk_button_new_with_label("Calcular viaje");
    GtkWidget *btn_exportar = gtk_button_new_with_label("Guardar reporte");
    GtkWidget *btn_limpiar = gtk_button_new_with_label("Limpiar resultado");
    gtk_box_pack_start(GTK_BOX(botones), btn_calcular, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(botones), btn_exportar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(botones), btn_limpiar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_calculo), botones, FALSE, FALSE, 0);

    txt_resultado = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(txt_resultado), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txt_resultado), GTK_WRAP_NONE);
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        "textview { font-family: monospace; font-size: 11pt; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(txt_resultado),
        GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css);

    GtkWidget *scroll_resultado = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_resultado),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll_resultado), txt_resultado);
    gtk_box_pack_start(GTK_BOX(tab_calculo), scroll_resultado, TRUE, TRUE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_vehiculos,
                             gtk_label_new("Vehículos"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_calculo,
                             gtk_label_new("Calcular viaje"));

    g_signal_connect(combo_vehiculos, "changed", G_CALLBACK(al_cambiar_vehiculo), NULL);
    g_signal_connect(btn_nuevo, "clicked", G_CALLBACK(al_nuevo_vehiculo), NULL);
    g_signal_connect(btn_guardar, "clicked", G_CALLBACK(al_guardar_vehiculo), NULL);
    g_signal_connect(btn_borrar, "clicked", G_CALLBACK(al_borrar_vehiculo), NULL);
    g_signal_connect(btn_calcular, "clicked", G_CALLBACK(al_calcular_viaje), NULL);
    g_signal_connect(btn_exportar, "clicked", G_CALLBACK(al_guardar_reporte), NULL);
    g_signal_connect(btn_limpiar, "clicked", G_CALLBACK(al_limpiar_resultado), NULL);

    limpiar_formulario();
    actualizar_combos();
    gtk_entry_set_text(GTK_ENTRY(ent_distancia), "300");
    gtk_entry_set_text(GTK_ENTRY(ent_pct_carretera), "70");
    gtk_entry_set_text(GTK_ENTRY(ent_pct_ciudad), "30");
    gtk_entry_set_text(GTK_ENTRY(ent_precio_litro), "1.20");
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(txt_resultado)),
        "1) Registra un vehículo en la pestaña «Vehículos».\n"
        "2) Elige el vehículo, ajusta los datos del viaje y pulsa «Calcular viaje».\n", -1);

    gtk_widget_show_all(ventana);
    gtk_main();
    return 0;
}

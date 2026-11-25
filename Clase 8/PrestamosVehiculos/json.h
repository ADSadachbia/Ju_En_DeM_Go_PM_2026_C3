#ifndef JSON_H
#define JSON_H

#include <stddef.h>

/* ------------------------------------------------------------------
 * Implementación mínima de JSON para C (sin librerías externas).
 * Soporta: null, boolean, number, string, array y object.
 * ------------------------------------------------------------------ */

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

typedef struct Json Json;

typedef struct {
    char *key;
    Json *value;
} JsonMember;

struct Json {
    JsonType type;
    union {
        int boolean;
        double number;
        char *string;
        struct {
            Json **items;
            size_t count;
            size_t capacity;
        } array;
        struct {
            JsonMember *items;
            size_t count;
            size_t capacity;
        } object;
    } data;
};

/* Constructores */
Json *json_null(void);
Json *json_bool(int value);
Json *json_number(double value);
Json *json_string(const char *value);
Json *json_array(void);
Json *json_object(void);

/* Liberación (libera también los hijos) */
void json_free(Json *value);

/* Estas funciones toman posesión del valor entregado */
void json_object_set(Json *object, const char *key, Json *value);
void json_array_add(Json *array, Json *value);

/* Parseo. Devuelve NULL si hay error; json_error() da el detalle. */
Json *json_parse(const char *text);
const char *json_error(void);

/* Acceso cómodo (préstamo: no liberar) */
Json      *json_get(const Json *object, const char *key);
const char *json_as_string(const Json *value, const char *fallback);
double      json_as_number(const Json *value, double fallback);
int         json_as_int(const Json *value, int fallback);
int         json_as_bool(const Json *value, int fallback);
size_t      json_array_size(const Json *value);
Json       *json_array_at(const Json *value, size_t index);

/* Escritura a disco con indentación */
int json_write_file(const char *path, const Json *root);

#endif

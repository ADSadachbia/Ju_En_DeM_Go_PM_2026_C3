#include "json.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Utilidades internas                                                 */
/* ------------------------------------------------------------------ */

static char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *copy = (char *)malloc(n);
    if (copy) memcpy(copy, s, n);
    return copy;
}

static void *xrealloc(void *ptr, size_t size) {
    void *tmp = realloc(ptr, size);
    if (!tmp) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    return tmp;
}

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} Buffer;

static void buffer_init(Buffer *b) {
    b->capacity = 64;
    b->length = 0;
    b->data = (char *)malloc(b->capacity);
    if (!b->data) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    b->data[0] = '\0';
}

static void buffer_push(Buffer *b, char c) {
    if (b->length + 1 >= b->capacity) {
        b->capacity *= 2;
        b->data = (char *)xrealloc(b->data, b->capacity);
    }
    b->data[b->length++] = c;
    b->data[b->length] = '\0';
}

static char *buffer_take(Buffer *b) {
    char *result = b->data;
    b->data = NULL;
    b->length = b->capacity = 0;
    return result;
}

static char last_error[256] = "Sin error";

const char *json_error(void) {
    return last_error;
}

/* ------------------------------------------------------------------ */
/* Constructores y destructor                                          */
/* ------------------------------------------------------------------ */

static Json *json_new(JsonType type) {
    Json *j = (Json *)calloc(1, sizeof(Json));
    if (!j) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    j->type = type;
    return j;
}

Json *json_null(void)  { return json_new(JSON_NULL); }
Json *json_bool(int v) { Json *j = json_new(JSON_BOOL); j->data.boolean = v ? 1 : 0; return j; }
Json *json_number(double v) { Json *j = json_new(JSON_NUMBER); j->data.number = v; return j; }

Json *json_string(const char *v) {
    Json *j = json_new(JSON_STRING);
    j->data.string = xstrdup(v ? v : "");
    return j;
}

Json *json_array(void)  { return json_new(JSON_ARRAY); }
Json *json_object(void) { return json_new(JSON_OBJECT); }

void json_free(Json *value) {
    size_t i;
    if (!value) return;

    switch (value->type) {
        case JSON_STRING:
            free(value->data.string);
            break;
        case JSON_ARRAY:
            for (i = 0; i < value->data.array.count; i++)
                json_free(value->data.array.items[i]);
            free(value->data.array.items);
            break;
        case JSON_OBJECT:
            for (i = 0; i < value->data.object.count; i++) {
                free(value->data.object.items[i].key);
                json_free(value->data.object.items[i].value);
            }
            free(value->data.object.items);
            break;
        default:
            break;
    }
    free(value);
}

void json_object_set(Json *object, const char *key, Json *value) {
    size_t i;
    if (!object || object->type != JSON_OBJECT || !key || !value) return;

    for (i = 0; i < object->data.object.count; i++) {
        if (strcmp(object->data.object.items[i].key, key) == 0) {
            json_free(object->data.object.items[i].value);
            object->data.object.items[i].value = value;
            return;
        }
    }

    if (object->data.object.count == object->data.object.capacity) {
        object->data.object.capacity = object->data.object.capacity ? object->data.object.capacity * 2 : 8;
        object->data.object.items = (JsonMember *)xrealloc(
            object->data.object.items,
            object->data.object.capacity * sizeof(JsonMember));
    }
    object->data.object.items[object->data.object.count].key = xstrdup(key);
    object->data.object.items[object->data.object.count].value = value;
    object->data.object.count++;
}

void json_array_add(Json *array, Json *value) {
    if (!array || array->type != JSON_ARRAY || !value) return;

    if (array->data.array.count == array->data.array.capacity) {
        array->data.array.capacity = array->data.array.capacity ? array->data.array.capacity * 2 : 8;
        array->data.array.items = (Json **)xrealloc(
            array->data.array.items,
            array->data.array.capacity * sizeof(Json *));
    }
    array->data.array.items[array->data.array.count++] = value;
}

/* ------------------------------------------------------------------ */
/* Parseo recursivo                                                    */
/* ------------------------------------------------------------------ */

static const char *cursor;
static const char *input_start;

static void skip_whitespace(void) {
    while (*cursor && isspace((unsigned char)*cursor)) cursor++;
}

static long current_position(void) {
    if (!cursor || !input_start) return 0;
    return (long)(cursor - input_start) + 1;
}

static void set_error_at(const char *message, long position) {
    snprintf(last_error, sizeof(last_error), "%s (posición %ld)", message, position);
}

static int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int read_hex4(unsigned int *out) {
    int i;
    unsigned int value = 0;
    for (i = 0; i < 4; i++) {
        int h;
        if (!*cursor) return 0;
        h = hex_value(*cursor);
        if (h < 0) return 0;
        value = (value << 4) | (unsigned int)h;
        cursor++;
    }
    *out = value;
    return 1;
}

static void append_utf8(Buffer *b, unsigned int codepoint) {
    if (codepoint <= 0x7F) {
        buffer_push(b, (char)codepoint);
    } else if (codepoint <= 0x7FF) {
        buffer_push(b, (char)(0xC0 | (codepoint >> 6)));
        buffer_push(b, (char)(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        buffer_push(b, (char)(0xE0 | (codepoint >> 12)));
        buffer_push(b, (char)(0x80 | ((codepoint >> 6) & 0x3F)));
        buffer_push(b, (char)(0x80 | (codepoint & 0x3F)));
    } else {
        buffer_push(b, (char)(0xF0 | (codepoint >> 18)));
        buffer_push(b, (char)(0x80 | ((codepoint >> 12) & 0x3F)));
        buffer_push(b, (char)(0x80 | ((codepoint >> 6) & 0x3F)));
        buffer_push(b, (char)(0x80 | (codepoint & 0x3F)));
    }
}

/* El cursor apunta a la comilla de apertura. */
static char *parse_string_raw(void) {
    Buffer b;
    buffer_init(&b);

    if (*cursor != '"') {
        set_error_at("Se esperaba una cadena", current_position());
        free(b.data);
        return NULL;
    }
    cursor++;

    while (*cursor && *cursor != '"') {
        if (*cursor == '\\') {
            cursor++;
            switch (*cursor) {
                case '"':  buffer_push(&b, '"');  break;
                case '\\': buffer_push(&b, '\\'); break;
                case '/':  buffer_push(&b, '/');  break;
                case 'b':  buffer_push(&b, '\b'); break;
                case 'f':  buffer_push(&b, '\f'); break;
                case 'n':  buffer_push(&b, '\n'); break;
                case 'r':  buffer_push(&b, '\r'); break;
                case 't':  buffer_push(&b, '\t'); break;
                case 'u': {
                    unsigned int cp;
                    cursor++;
                    if (!read_hex4(&cp)) {
                        free(b.data);
                        return NULL;
                    }
                    if (cp >= 0xD800 && cp <= 0xDBFF && cursor[0] == '\\' && cursor[1] == 'u') {
                        unsigned int low;
                        cursor += 2;
                        if (!read_hex4(&low) || low < 0xDC00 || low > 0xDFFF) {
                            free(b.data);
                            return NULL;
                        }
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                    }
                    append_utf8(&b, cp);
                    continue;
                }
                default:
                    free(b.data);
                    return NULL;
            }
            cursor++;
        } else {
            buffer_push(&b, *cursor++);
        }
    }

    if (*cursor != '"') {
        free(b.data);
        return NULL;
    }
    cursor++;
    return buffer_take(&b);
}

static int parse_value(Json **out);

static int parse_object(Json **out) {
    Json *object = json_object();
    long start = current_position();

    cursor++; /* '{' */
    skip_whitespace();
    if (*cursor == '}') {
        cursor++;
        *out = object;
        return 1;
    }

    for (;;) {
        char *key;
        Json *value = NULL;

        if (*cursor != '"') {
            set_error_at("Se esperaba la clave del objeto", start);
            json_free(object);
            return 0;
        }
        key = parse_string_raw();
        if (!key) {
            set_error_at("Cadena de clave inválida", start);
            json_free(object);
            return 0;
        }

        skip_whitespace();
        if (*cursor != ':') {
            set_error_at("Se esperaba ':'", current_position());
            free(key);
            json_free(object);
            return 0;
        }
        cursor++;
        skip_whitespace();

        if (!parse_value(&value)) {
            free(key);
            json_free(object);
            return 0;
        }

        json_object_set(object, key, value);
        free(key);

        skip_whitespace();
        if (*cursor == ',') {
            cursor++;
            skip_whitespace();
            continue;
        }
        if (*cursor == '}') {
            cursor++;
            *out = object;
            return 1;
        }
        set_error_at("Se esperaba ',' o '}'", current_position());
        json_free(object);
        return 0;
    }
}

static int parse_array(Json **out) {
    Json *array = json_array();

    cursor++; /* '[' */
    skip_whitespace();
    if (*cursor == ']') {
        cursor++;
        *out = array;
        return 1;
    }

    for (;;) {
        Json *value = NULL;
        if (!parse_value(&value)) {
            json_free(array);
            return 0;
        }
        json_array_add(array, value);

        skip_whitespace();
        if (*cursor == ',') {
            cursor++;
            skip_whitespace();
            continue;
        }
        if (*cursor == ']') {
            cursor++;
            *out = array;
            return 1;
        }
        set_error_at("Se esperaba ',' o ']'", current_position());
        json_free(array);
        return 0;
    }
}

static int parse_literal(Json **out, const char *word, JsonType type, int boolean) {
    size_t n = strlen(word);
    if (strncmp(cursor, word, n) != 0) return 0;
    cursor += n;
    if (type == JSON_BOOL) *out = json_bool(boolean);
    else *out = json_null();
    return 1;
}

static int parse_value(Json **out) {
    skip_whitespace();

    if (*cursor == '{') return parse_object(out);
    if (*cursor == '[') return parse_array(out);
    if (*cursor == '"') {
        char *s = parse_string_raw();
        if (!s) {
            set_error_at("Cadena inválida", current_position());
            return 0;
        }
        *out = json_string(s);
        free(s);
        return 1;
    }
    if (*cursor == 't') return parse_literal(out, "true", JSON_BOOL, 1);
    if (*cursor == 'f') return parse_literal(out, "false", JSON_BOOL, 0);
    if (*cursor == 'n') return parse_literal(out, "null", JSON_NULL, 0);
    if (*cursor == '-' || isdigit((unsigned char)*cursor)) {
        char *end;
        double number = strtod(cursor, &end);
        if (end == cursor) {
            set_error_at("Número inválido", current_position());
            return 0;
        }
        cursor = end;
        *out = json_number(number);
        return 1;
    }

    set_error_at("Valor JSON inesperado", current_position());
    return 0;
}

Json *json_parse(const char *text) {
    Json *root = NULL;
    if (!text) {
        snprintf(last_error, sizeof(last_error), "Texto nulo");
        return NULL;
    }
    strcpy(last_error, "Sin error");
    input_start = text;
    cursor = text;
    if (!parse_value(&root)) return NULL;
    skip_whitespace();
    if (*cursor != '\0') {
        set_error_at("Sobran caracteres después del JSON", current_position());
        json_free(root);
        return NULL;
    }
    return root;
}

/* ------------------------------------------------------------------ */
/* Acceso cómodo                                                       */
/* ------------------------------------------------------------------ */

Json *json_get(const Json *object, const char *key) {
    size_t i;
    if (!object || object->type != JSON_OBJECT || !key) return NULL;
    for (i = 0; i < object->data.object.count; i++) {
        if (strcmp(object->data.object.items[i].key, key) == 0)
            return object->data.object.items[i].value;
    }
    return NULL;
}

const char *json_as_string(const Json *value, const char *fallback) {
    if (value && value->type == JSON_STRING) return value->data.string;
    return fallback;
}

double json_as_number(const Json *value, double fallback) {
    if (value && value->type == JSON_NUMBER) return value->data.number;
    return fallback;
}

int json_as_int(const Json *value, int fallback) {
    if (value && value->type == JSON_NUMBER) return (int)value->data.number;
    return fallback;
}

int json_as_bool(const Json *value, int fallback) {
    if (value && value->type == JSON_BOOL) return value->data.boolean;
    return fallback;
}

size_t json_array_size(const Json *value) {
    if (value && value->type == JSON_ARRAY) return value->data.array.count;
    return 0;
}

Json *json_array_at(const Json *value, size_t index) {
    if (!value || value->type != JSON_ARRAY || index >= value->data.array.count) return NULL;
    return value->data.array.items[index];
}

/* ------------------------------------------------------------------ */
/* Escritura                                                           */
/* ------------------------------------------------------------------ */

static void write_indent(FILE *file, int level) {
    int i;
    for (i = 0; i < level; i++) fputs("  ", file);
}

static void write_string(FILE *file, const char *s) {
    fputc('"', file);
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        switch (c) {
            case '"':  fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\b': fputs("\\b", file);  break;
            case '\f': fputs("\\f", file);  break;
            case '\n': fputs("\\n", file);  break;
            case '\r': fputs("\\r", file);  break;
            case '\t': fputs("\\t", file);  break;
            default:
                if (c < 0x20) fprintf(file, "\\u%04x", c);
                else fputc(c, file);
        }
    }
    fputc('"', file);
}

static void write_json(FILE *file, const Json *value, int level) {
    size_t i;
    if (!value) { fputs("null", file); return; }

    switch (value->type) {
        case JSON_NULL:
            fputs("null", file);
            break;
        case JSON_BOOL:
            fputs(value->data.boolean ? "true" : "false", file);
            break;
        case JSON_NUMBER:
            fprintf(file, "%.10g", value->data.number);
            break;
        case JSON_STRING:
            write_string(file, value->data.string);
            break;
        case JSON_ARRAY:
            if (value->data.array.count == 0) {
                fputs("[]", file);
                break;
            }
            fputs("[\n", file);
            for (i = 0; i < value->data.array.count; i++) {
                write_indent(file, level + 1);
                write_json(file, value->data.array.items[i], level + 1);
                if (i + 1 < value->data.array.count) fputc(',', file);
                fputc('\n', file);
            }
            write_indent(file, level);
            fputc(']', file);
            break;
        case JSON_OBJECT:
            if (value->data.object.count == 0) {
                fputs("{}", file);
                break;
            }
            fputs("{\n", file);
            for (i = 0; i < value->data.object.count; i++) {
                write_indent(file, level + 1);
                write_string(file, value->data.object.items[i].key);
                fputs(": ", file);
                write_json(file, value->data.object.items[i].value, level + 1);
                if (i + 1 < value->data.object.count) fputc(',', file);
                fputc('\n', file);
            }
            write_indent(file, level);
            fputc('}', file);
            break;
    }
}

int json_write_file(const char *path, const Json *root) {
    FILE *file = fopen(path, "w");
    if (!file) return 0;
    write_json(file, root, 0);
    fputc('\n', file);
    return fclose(file) == 0;
}

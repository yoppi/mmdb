#include <ruby.h>

#include <maxminddb.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

static VALUE rb_cMaxMindDB;

static VALUE rb_sym_city;
static VALUE rb_sym_country;
static VALUE rb_sym_country_code;
static VALUE rb_sym_continent;
static VALUE rb_sym_latitude;
static VALUE rb_sym_longitude;
static VALUE rb_sym_postcode;

const static char *en = "en";
const static char *names = "names";
const static char *city = "city";
const static char *country = "country";
const static char *country_code = "country_code";
const static char *continent = "continent";
const static char *location = "location";
const static char *latitude = "latitude";
const static char *longitude = "longitude";
const static char *postal = "postal";
const static char *code = "code";
const static char *iso_code = "iso_code";
const static char *postcode = "postcode";

struct MaxMindDB {
    MMDB_s *mmdb;
};

static size_t
maxminddb_memsize(const void *p) {
    const struct MaxMindDB *ptr = p;
    size_t size = 0;

    if (ptr) {
        size += sizeof(ptr);
        if (ptr->mmdb) {
            size += sizeof(MMDB_s);
        }
    }

    return size;
}

void
maxminddb_free(void *p) {
    struct MaxMindDB *ptr = p;

    if (ptr) {
        if (ptr->mmdb) {
            MMDB_close(ptr->mmdb);
        }
        xfree(ptr);
    }
}

static const rb_data_type_t mmdb_data_type = {
    "mmdb",
    {0, maxminddb_free, maxminddb_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

#define check_maxminddb(self) ((struct MaxMindDB*)rb_check_typeddata((self), &mmdb_data_type))

static struct MaxMindDB*
get_maxminddb(VALUE self) {
    struct MaxMindDB *ptr = check_maxminddb(self);

    if (!ptr) {
        rb_raise(rb_eStandardError, "uninitialized");
    }

    return ptr;
}

#define MaxMindDB(self) get_maxminddb(self)

static VALUE
maxminddb_alloc(VALUE klass) {
    return TypedData_Wrap_Struct(klass, &mmdb_data_type, 0);
}

static void
maxminddb_set_result(VALUE hash, VALUE key, MMDB_entry_data_s *data) {
    if (data->has_data) {
        switch (data->type) {
        case MMDB_DATA_TYPE_UTF8_STRING:
            rb_hash_aset(hash, key, rb_utf8_str_new(data->utf8_string, data->data_size));
            break;
        case MMDB_DATA_TYPE_DOUBLE:
            rb_hash_aset(hash, key, rb_float_new(data->double_value));
            break;
        default:
            rb_hash_aset(hash, key, Qnil);
        }
    } else {
        rb_hash_aset(hash, key, Qnil);
    }
}

VALUE
maxminddb_initialize(int argc, VALUE* argv, VALUE self) {
    VALUE db_path;
    MMDB_s *mmdb = ALLOC(MMDB_s);
    struct MaxMindDB *ptr;

    rb_scan_args(argc, argv, "1", &db_path);

    FilePathValue(db_path);

    if (MMDB_open(StringValuePtr(db_path), MMDB_MODE_MMAP, mmdb) != MMDB_SUCCESS) {
        rb_sys_fail_str(db_path);
    }

    ptr = ALLOC(struct MaxMindDB);
    DATA_PTR(self) = ptr;
    ptr->mmdb = mmdb;

    return self;
}

/**
 *
 * mmdb.lookup("123.45.67.89") -> Hash
 *
 * TODO:
 *   To receive `opts` argument that have language code and respond specified language data.
 */
VALUE
maxminddb_lookup(VALUE self, VALUE ip) {
    VALUE ret;
    int gai_error, mmdb_error;
    struct MaxMindDB *ptr = MaxMindDB(self);
    MMDB_s *mmdb = ptr->mmdb;

    if (NIL_P(ip)) {
        return Qnil;
    }

    MMDB_lookup_result_s lookuped = MMDB_lookup_string(mmdb, StringValuePtr(ip), &gai_error, &mmdb_error);
    if (gai_error) {
        rb_sys_fail(gai_strerror(gai_error));
    }
    if (mmdb_error) {
        rb_sys_fail(MMDB_strerror(mmdb_error));
    }

    if (lookuped.found_entry) {
        ret = rb_hash_new();
        MMDB_entry_data_s data;
        MMDB_entry_s *entry = &lookuped.entry;
        MMDB_get_value(entry, &data, city, names, en, NULL);
        maxminddb_set_result(ret, rb_sym_city, &data);
        MMDB_get_value(entry, &data, country, names, en, NULL);
        maxminddb_set_result(ret, rb_sym_country, &data);
        MMDB_get_value(entry, &data, country, iso_code, NULL);
        maxminddb_set_result(ret, rb_sym_country_code, &data);
        MMDB_get_value(entry, &data, continent, names, en, NULL);
        maxminddb_set_result(ret, rb_sym_continent, &data);
        MMDB_get_value(entry, &data, location, latitude, NULL);
        maxminddb_set_result(ret, rb_sym_latitude, &data);
        MMDB_get_value(entry, &data, location, longitude, NULL);
        maxminddb_set_result(ret, rb_sym_longitude, &data);
        MMDB_get_value(entry, &data, postal, code, NULL);
        maxminddb_set_result(ret, rb_sym_postcode, &data);
    } else {
        ret = Qnil;
    }

    return ret;
}

VALUE
maxminddb_close(VALUE self) {
    struct MaxMindDB *ptr = MaxMindDB(self);

    if (ptr) {
        if (ptr->mmdb) {
            MMDB_close(ptr->mmdb);
            ptr->mmdb = NULL;
        }
    }

    return Qnil;
}

void
maxminddb_init_rb_variables() {
    rb_sym_city = ID2SYM(rb_intern(city));
    rb_sym_country = ID2SYM(rb_intern(country));
    rb_sym_country_code = ID2SYM(rb_intern(country_code));
    rb_sym_continent = ID2SYM(rb_intern(continent));
    rb_sym_latitude = ID2SYM(rb_intern(latitude));
    rb_sym_longitude = ID2SYM(rb_intern(longitude));
    rb_sym_postcode = ID2SYM(rb_intern(postcode));
}

void
Init_mmdb() {
    rb_cMaxMindDB = rb_define_class("MaxMindDB", rb_cObject);

    rb_define_alloc_func(rb_cMaxMindDB, maxminddb_alloc);

    rb_define_method(rb_cMaxMindDB, "initialize", maxminddb_initialize, -1);
    rb_define_method(rb_cMaxMindDB, "lookup", maxminddb_lookup, 1);
    rb_define_method(rb_cMaxMindDB, "close", maxminddb_close, 0);

    maxminddb_init_rb_variables();
}

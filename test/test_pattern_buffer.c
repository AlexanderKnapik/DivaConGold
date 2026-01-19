#include "common/util.h"
#include "slider/colour.h"
#include "slider/pattern_buffer.h"
#include "slider/pattern_buffer_pvt.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#define HANDLE_SIZE (sizeof(struct pattern_buffer_handle))

const struct colour default_colours[] = {
    {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0x00}, {0xFF, 0X00, 0xFF}, {0xFF, 0x00, 0x00},
    {0x00, 0xFF, 0xFF}, {0x00, 0xFF, 0x00}, {0x00, 0X00, 0xFF}, {0x00, 0x00, 0x00},
};

static void assert_colour_equal(const struct colour *a, const struct colour *b, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        assert_int_equal(a->r, b->r);
        assert_int_equal(a->g, b->g);
        assert_int_equal(a->b, b->b);

        a++;
        b++;
    }
}

static void test_pattern_buffer_open_null_colours(void **state)
{
    (void)state;

    const pattern_buffer_handle_t pattern = pattern_buffer_open(NULL, 10);

    assert_null(pattern);
}

static void test_pattern_buffer_open_zero_size(void **state)
{
    (void)state;

    const pattern_buffer_handle_t pattern = pattern_buffer_open(default_colours, 0);

    assert_null(pattern);
}

static void test_pattern_buffer_open_success(void **state)
{
    (void)state;

    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));

    assert_non_null(pattern);
    assert_ptr_equal(pattern->buf, default_colours);
    assert_int_equal(pattern->read, 0);
    assert_int_equal(pattern->size, ARRAY_SIZE(default_colours));
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_increment_normal(void **state)
{
    (void)state;

    const uint16_t offset = 1;
    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));

    const uint16_t val = pattern_buffer_increment(pattern, offset);

    assert_int_equal(val, 1);
    assert_int_equal(pattern->read, 1);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_increment_wrap(void **state)
{
    (void)state;

    const uint16_t offset = 8;
    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));

    const uint16_t val = pattern_buffer_increment(pattern, offset);

    assert_int_equal(val, 0);
    assert_int_equal(pattern->read, 0);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_increment_multiple_wrap(void **state)
{
    (void)state;

    const uint16_t offset = (2 * ARRAY_SIZE(default_colours)) + 4;
    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));

    const uint16_t val = pattern_buffer_increment(pattern, offset);

    assert_int_equal(val, 4);
    assert_int_equal(pattern->read, 4);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_read_uninit_handle(void **state)
{
    (void)state;

    struct colour dest[1] = {};
    const uint16_t size = ARRAY_SIZE(dest);

    const uint16_t bytes_read = pattern_buffer_read(NULL, dest, size);

    assert_int_equal(bytes_read, 0);
}

static void test_pattern_buffer_read_null_dest(void **state)
{
    (void)state;

    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));
    struct colour dest[1] = {};
    const uint16_t size = ARRAY_SIZE(dest);

    const uint16_t bytes_read = pattern_buffer_read(pattern, NULL, size);

    assert_int_equal(bytes_read, 0);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_read_zero_len(void **state)
{
    (void)state;

    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));
    struct colour dest[1] = {};
    const uint16_t size = 0;

    const uint16_t bytes_read = pattern_buffer_read(pattern, dest, size);

    assert_int_equal(bytes_read, 0);
    pattern_buffer_close(pattern);
}

void helper_pattern_buffer_read(const struct colour *src, uint16_t size)
{
    pattern_buffer_handle_t pattern = pattern_buffer_open(src, size);
    struct colour *dest = malloc(sizeof(*dest));

    const uint16_t bytes_read = pattern_buffer_read(pattern, dest, size);

    assert_int_equal(bytes_read, size);
    assert_colour_equal(dest, pattern->buf, size);

    pattern_buffer_close(pattern);
    free(dest);
}

static void test_pattern_buffer_read_byte(void **state)
{
    (void)state;

    helper_pattern_buffer_read(default_colours, 1);
}

static void test_pattern_buffer_read_zero_pos(void **state)
{
    (void)state;

    helper_pattern_buffer_read(default_colours, ARRAY_SIZE(default_colours));
}

static void test_pattern_buffer_read_offset_pos(void **state)
{
    (void)state;

    const uint16_t size = 4;
    struct colour dest[4] = {};
    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));
    pattern_buffer_increment(pattern, 4);

    const struct colour expected_colours[] = {
        default_colours[4],
        default_colours[5],
        default_colours[6],
        default_colours[7],
    };

    const uint16_t bytes_read = pattern_buffer_read(pattern, dest, size);

    assert_int_equal(bytes_read, size);
    assert_colour_equal(dest, expected_colours, size);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_read_wrap(void **state)
{
    (void)state;

    const uint16_t size = ARRAY_SIZE(default_colours);
    const uint16_t offset = 4;
    struct colour dest[size];

    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));
    pattern_buffer_increment(pattern, offset);

    const struct colour expected_colours[] = {
        default_colours[4], default_colours[5], default_colours[6], default_colours[7],
        default_colours[0], default_colours[1], default_colours[2], default_colours[3],
    };

    const uint16_t bytes_read = pattern_buffer_read(pattern, dest, size);

    assert_int_equal(bytes_read, size);
    assert_colour_equal(dest, expected_colours, size);
    pattern_buffer_close(pattern);
}

static void test_pattern_buffer_read_multiple_wrap(void **state)
{
    (void)state;

    (void)state;

    const uint16_t size = 2 * ARRAY_SIZE(default_colours);
    const uint16_t offset = 4;
    struct colour dest[size];

    pattern_buffer_handle_t pattern =
        pattern_buffer_open(default_colours, ARRAY_SIZE(default_colours));
    pattern_buffer_increment(pattern, offset);

    const struct colour expected_colours[] = {
        default_colours[4], default_colours[5], default_colours[6], default_colours[7],
        default_colours[0], default_colours[1], default_colours[2], default_colours[3],
        default_colours[4], default_colours[5], default_colours[6], default_colours[7],
        default_colours[0], default_colours[1], default_colours[2], default_colours[3],
    };

    const uint16_t bytes_read = pattern_buffer_read(pattern, dest, size);

    assert_int_equal(bytes_read, size);
    assert_colour_equal(dest, expected_colours, size);
    pattern_buffer_close(pattern);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pattern_buffer_open_null_colours),
        cmocka_unit_test(test_pattern_buffer_open_zero_size),
        cmocka_unit_test(test_pattern_buffer_open_success),
        cmocka_unit_test(test_pattern_buffer_increment_normal),
        cmocka_unit_test(test_pattern_buffer_increment_wrap),
        cmocka_unit_test(test_pattern_buffer_increment_multiple_wrap),
        cmocka_unit_test(test_pattern_buffer_read_uninit_handle),
        cmocka_unit_test(test_pattern_buffer_read_null_dest),
        cmocka_unit_test(test_pattern_buffer_read_zero_len),
        cmocka_unit_test(test_pattern_buffer_read_byte),
        cmocka_unit_test(test_pattern_buffer_read_zero_pos),
        cmocka_unit_test(test_pattern_buffer_read_offset_pos),
        cmocka_unit_test(test_pattern_buffer_read_wrap),
        cmocka_unit_test(test_pattern_buffer_read_multiple_wrap),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

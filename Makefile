SHELL := bash

ON_WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wredundant-decls
NO_WARNINGS := -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-override-init

OPTIMIZATION := -O2

DEFAULT_FLAGS := -g -lm -std=c11 -DDEBUG -DPROFILE ${ON_WARNINGS} ${NO_WARNINGS}
TEST_FLAGS := ${DEFAULT_FLAGS}
CFLAGS := ${DEFAULT_FLAGS} ${OPTIMIZATION}

# Lucky number
TRY_FOR_MIN_TIME := 7

bin-folder:
	mkdir -p bin

taco-rename: bin-folder
	gcc ${CFLAGS} src/taco_rename.c -o bin/taco_rename.x
	bin/taco_rename.x taco_test.txt

json: bin-folder
	gcc ${CFLAGS} src/make_haversine_json.c -lm -o bin/make.x
	bin/make.x uniform $$RANDOM 10000000

calc: bin-folder
	gcc ${CFLAGS} src/calc_haversine.c  -lm -o bin/calc.x
	bin/calc.x haversine_pairs.json solution_dump.data

address-anatomy: bin-folder
	gcc ${CFLAGS} src/address_anatomy.c -o bin/address_anatomy.x
	bin/address_anatomy.x

tests: test-common test-arguments test-c-tokenize test-linear-algebra

test-common: bin-folder
	gcc ${TEST_FLAGS} src/tests/test_common.c -o bin/test_common.x
	bin/test_common.x

test-arguments: bin-folder
	gcc ${TEST_FLAGS} src/tests/test_arguments.c -o bin/test_arguments.x
	bin/test_arguments.x positional --verbose -v --foo --bar --baz=foo,bar,boo positional2 --bunk=bip,bop,bam,

test-c-tokenize: bin-folder
	gcc ${TEST_FLAGS} src/tests/test_c_tokenize.c -o bin/test_c_tokenize.x
	bin/test_c_tokenize.x

test-c-parse: bin-folder
	gcc ${TEST_FLAGS} src/tests/test_c_parse.c -o bin/test_c_parse.x
	bin/test_c_parse.x

test-linear-algebra: bin-folder
	gcc ${TEST_FLAGS} src/tests/test_linear_algebra.c -o bin/test_linear_algebra.x
	bin/test_linear_algebra.x

reptest-file-apis: bin-folder
	head -c 1G /dev/urandom > gb_file.txt
	gcc ${CFLAGS} src/reptests/reptest_file_apis.c -o bin/reptest_file_apis.x
	bin/reptest_file_apis.x gb_file.txt $(TRY_FOR_MIN_TIME)

reptest-page-faults: bin-folder
	gcc ${CFLAGS} src/reptests/reptest_page_faults.c -o bin/reptest_page_faults.x
	bin/reptest_page_faults.x $(TRY_FOR_MIN_TIME)

reptest-loop-dependencies: bin-folder
	nasm -f elf64 -o bin/reptest_loop_dependencies.o src/reptests/reptest_loop_dependencies.asm
	ar rcs bin/reptest_loop_dependencies.a bin/loop_deps.o
	gcc ${CFLAGS} src/reptests/reptest_loop_dependencies.c bin/reptest_loop_dependencies.a -o bin/reptest_loop_dependencies.x
	bin/reptest_loop_dependencies.x $(TRY_FOR_MIN_TIME)

reptest-nops: bin-folder
	nasm -f elf64 -o bin/reptest_nops.o src/reptests/reptest_nops.asm
	ar rcs bin/reptest_nops.a bin/reptest_nops.o
	gcc ${CFLAGS} src/reptests/reptest_nops.c bin/reptest_nops.a -o bin/reptest_nops.x
	bin/reptest_nops.x $(TRY_FOR_MIN_TIME)

reptest-branches: bin-folder
	nasm -f elf64 -o bin/reptest_branches.o src/reptests/reptest_branches.asm
	ar rcs bin/reptest_branches.a bin/reptest_branches.o
	gcc ${CFLAGS} src/reptests/reptest_branches.c bin/reptest_branches.a -o bin/reptest_branches.x
	bin/reptest_branches.x $(TRY_FOR_MIN_TIME)

reptest-code-alignment: bin-folder
	nasm -f elf64 -o bin/reptest_code_alignment.o src/reptests/reptest_code_alignment.asm
	ar rcs bin/reptest_code_alignment.a bin/reptest_code_alignment.o
	gcc ${CFLAGS} src/reptests/reptest_code_alignment.c bin/reptest_code_alignment.a -o bin/reptest_code_alignment.x
	bin/reptest_code_alignment.x $(TRY_FOR_MIN_TIME)

reptest-rat: bin-folder
	nasm -f elf64 -o bin/reptest_rat.o src/reptests/reptest_rat.asm
	ar rcs bin/reptest_rat.a bin/reptest_rat.o
	gcc ${CFLAGS} src/reptests/reptest_rat.c bin/reptest_rat.a -o bin/reptest_rat.x
	bin/reptest_rat.x $(TRY_FOR_MIN_TIME)

reptest-read-ports: bin-folder
	nasm -f elf64 -o bin/reptest_read_ports.o src/reptests/reptest_read_ports.asm
	ar rcs bin/reptest_read_ports.a bin/reptest_read_ports.o
	gcc ${CFLAGS} src/reptests/reptest_read_ports.c bin/reptest_read_ports.a -o bin/reptest_read_ports.x
	bin/reptest_read_ports.x $(TRY_FOR_MIN_TIME)

reptest-write-ports: bin-folder
	nasm -f elf64 -o bin/reptest_write_ports.o src/reptests/reptest_write_ports.asm
	ar rcs bin/reptest_write_ports.a bin/reptest_write_ports.o
	gcc ${CFLAGS} src/reptests/reptest_write_ports.c bin/reptest_write_ports.a -o bin/reptest_write_ports.x
	bin/reptest_write_ports.x $(TRY_FOR_MIN_TIME)

reptest-wide-reads: bin-folder
	nasm -f elf64 -o bin/reptest_wide_reads.o src/reptests/reptest_wide_reads.asm
	ar rcs bin/reptest_wide_reads.a bin/reptest_wide_reads.o
	gcc ${CFLAGS} src/reptests/reptest_wide_reads.c bin/reptest_wide_reads.a -o bin/reptest_wide_reads.x
	bin/reptest_wide_reads.x $(TRY_FOR_MIN_TIME)

reptest-cache-bandwidth: bin-folder
	nasm -f elf64 -o bin/reptest_cache_bandwidth.o src/reptests/reptest_cache_bandwidth.asm
	ar rcs bin/reptest_cache_bandwidth.a bin/reptest_cache_bandwidth.o
	gcc ${CFLAGS} src/reptests/reptest_cache_bandwidth.c bin/reptest_cache_bandwidth.a -o bin/reptest_cache_bandwidth.x
	bin/reptest_cache_bandwidth.x $(TRY_FOR_MIN_TIME) granular

reptest-data-alignment: bin-folder
	nasm -f elf64 -o bin/reptest_cache_bandwidth.o src/reptests/reptest_cache_bandwidth.asm
	ar rcs bin/reptest_cache_bandwidth.a bin/reptest_cache_bandwidth.o
	gcc ${CFLAGS} src/reptests/reptest_data_alignment.c bin/reptest_cache_bandwidth.a -o bin/reptest_data_alignment.x
	bin/reptest_data_alignment.x $(TRY_FOR_MIN_TIME)

reptest-pathologic-cache: bin-folder
	nasm -f elf64 -o bin/reptest_pathologic_cache.o src/reptests/reptest_pathologic_cache.asm
	ar rcs bin/reptest_pathologic_cache.a bin/reptest_pathologic_cache.o
	gcc ${CFLAGS} src/reptests/reptest_pathologic_cache.c bin/reptest_pathologic_cache.a -o bin/reptest_pathologic_cache.x
	bin/reptest_pathologic_cache.x $(TRY_FOR_MIN_TIME)

reptest-non-temporal: bin-folder
	nasm -f elf64 -o bin/reptest_non_temporal.o src/reptests/reptest_non_temporal.asm
	ar rcs bin/reptest_non_temporal.a bin/reptest_non_temporal.o
	gcc ${CFLAGS} src/reptests/reptest_non_temporal.c bin/reptest_non_temporal.a -o bin/reptest_non_temporal.x
	bin/reptest_non_temporal.x $(TRY_FOR_MIN_TIME)

reptest-prefetch: bin-folder
	nasm -f elf64 -o bin/reptest_prefetch.o src/reptests/reptest_prefetch.asm
	ar rcs bin/reptest_prefetch.a bin/reptest_prefetch.o
	gcc ${CFLAGS} src/reptests/reptest_prefetch.c bin/reptest_prefetch.a -o bin/reptest_prefetch.x
	bin/reptest_prefetch.x $(TRY_FOR_MIN_TIME)

reptest-chunk-read: bin-folder
	gcc ${CFLAGS} src/reptests/reptest_chunk_read.c -o bin/reptest_chunk_read.x
	bin/reptest_chunk_read.x gb_file.txt $(TRY_FOR_MIN_TIME)

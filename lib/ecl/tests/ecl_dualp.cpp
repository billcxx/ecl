#include <stdlib.h>
#include <stdbool.h>

#include <ert/util/test_util.hpp>

#include <ert/ecl/ecl_grid.hpp>
#include <ert/ecl/ecl_file.hpp>
#include <ert/ecl/ecl_kw.hpp>
#include <ert/ecl/ecl_kw_magic.hpp>

int main(int argc, char **argv) {
    const char *case_path = argv[1];
    char *grid_file =
        ecl_util_alloc_filename(NULL, case_path, ECL_EGRID_FILE, false, 0);
    char *init_file =
        ecl_util_alloc_filename(NULL, case_path, ECL_INIT_FILE, false, 0);
    char *rst_file =
        ecl_util_alloc_filename(NULL, case_path, ECL_RESTART_FILE, false, 0);

    ecl_grid_type *ecl_grid = ecl_grid_alloc(grid_file);
    ecl_file_type *RST_file = ecl_file_open(rst_file, 0);
    ecl_file_type *INIT_file = ecl_file_open(init_file, 0);
    ecl_file_type *GRID_file = ecl_file_open(grid_file, 0);

    {
        ecl_kw_type *actnum = ecl_file_iget_named_kw(GRID_file, "ACTNUM", 0);
        ecl_kw_type *swat = ecl_file_iget_named_kw(RST_file, "SWAT", 0);
        ecl_kw_type *permx = ecl_file_iget_named_kw(INIT_file, "PERMX", 0);
        int fracture_size = ecl_grid_get_nactive_fracture(ecl_grid);
        int matrix_size = ecl_grid_get_nactive(ecl_grid);

        test_assert_int_equal(fracture_size + matrix_size,
                              ecl_kw_get_size(swat));
        test_assert_int_equal(fracture_size + matrix_size,
                              ecl_kw_get_size(permx));

        {
            int gi;
            int matrix_index = 0;
            int fracture_index = 0;

            for (gi = 0; gi < ecl_grid_get_global_size(ecl_grid); gi++) {
                if (ecl_kw_iget_int(actnum, gi) & CELL_ACTIVE_MATRIX) {
                    test_assert_int_equal(
                        ecl_grid_get_active_index1(ecl_grid, gi), matrix_index);
                    test_assert_int_equal(
                        ecl_grid_get_global_index1A(ecl_grid, matrix_index),
                        gi);
                    matrix_index++;
                }

                if (ecl_kw_iget_int(actnum, gi) & CELL_ACTIVE_FRACTURE) {
                    test_assert_int_equal(
                        ecl_grid_get_active_fracture_index1(ecl_grid, gi),
                        fracture_index);
                    test_assert_int_equal(
                        ecl_grid_get_global_index1F(ecl_grid, fracture_index),
                        gi);
                    fracture_index++;
                }
            }
        }
    }

    ecl_file_close(RST_file);
    ecl_file_close(INIT_file);
    ecl_grid_free(ecl_grid);

    exit(0);
}

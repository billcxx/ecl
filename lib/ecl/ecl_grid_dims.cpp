#include <stdlib.h>
#include <stdbool.h>

#include <ert/util/util.h>
#include <ert/util/vector.hpp>

#include <ert/ecl/ecl_grid_dims.hpp>
#include <ert/ecl/ecl_util.hpp>
#include <ert/ecl/fortio.h>
#include <ert/ecl/ecl_endian_flip.hpp>
#include <ert/ecl/ecl_kw_magic.hpp>
#include <ert/ecl/ecl_kw.hpp>

struct ecl_grid_dims_struct {
    vector_type *dims_list;
};

static void ecl_grid_dims_read_EGRID(ecl_grid_dims_type *grid_dims,
                                     fortio_type *grid_fortio,
                                     fortio_type *data_fortio) {
    while (ecl_kw_fseek_kw(GRIDHEAD_KW, false, false, grid_fortio)) {
        grid_dims_type *dims;
        {
            ecl_kw_type *gridhead_kw = ecl_kw_fread_alloc(grid_fortio);

            int nx = ecl_kw_iget_int(gridhead_kw, GRIDHEAD_NX_INDEX);
            int ny = ecl_kw_iget_int(gridhead_kw, GRIDHEAD_NY_INDEX);
            int nz = ecl_kw_iget_int(gridhead_kw, GRIDHEAD_NZ_INDEX);

            dims = grid_dims_alloc(nx, ny, nz, 0);
            ecl_kw_free(gridhead_kw);
        }

        if (data_fortio) {
            if (ecl_kw_fseek_kw(INTEHEAD_KW, false, false, data_fortio)) {
                ecl_kw_type *intehead_kw = ecl_kw_fread_alloc(data_fortio);
                dims->nactive =
                    ecl_kw_iget_int(intehead_kw, INTEHEAD_NACTIVE_INDEX);
                ecl_kw_free(intehead_kw);
            }
        }

        vector_append_owned_ref(grid_dims->dims_list, dims, grid_dims_free__);
    }
}

static void ecl_grid_dims_read_GRID(ecl_grid_dims_type *grid_dims,
                                    fortio_type *grid_fortio,
                                    fortio_type *data_fortio) {
    while (ecl_kw_fseek_kw(DIMENS_KW, false, false, grid_fortio)) {
        grid_dims_type *dims;
        {
            ecl_kw_type *dimens_kw = ecl_kw_fread_alloc(grid_fortio);

            int nx = ecl_kw_iget_int(dimens_kw, DIMENS_NX_INDEX);
            int ny = ecl_kw_iget_int(dimens_kw, DIMENS_NY_INDEX);
            int nz = ecl_kw_iget_int(dimens_kw, DIMENS_NZ_INDEX);

            dims = grid_dims_alloc(nx, ny, nz, 0);
            ecl_kw_free(dimens_kw);
        }

        if (data_fortio) {
            if (ecl_kw_fseek_kw(INTEHEAD_KW, false, false, data_fortio)) {
                ecl_kw_type *intehead_kw = ecl_kw_fread_alloc(data_fortio);
                dims->nactive =
                    ecl_kw_iget_int(intehead_kw, INTEHEAD_NACTIVE_INDEX);
                ecl_kw_free(intehead_kw);
            }
        }

        vector_append_owned_ref(grid_dims->dims_list, dims, grid_dims_free__);
    }
}

ecl_grid_dims_type *ecl_grid_dims_alloc(const char *grid_file,
                                        const char *data_file) {
    ecl_grid_dims_type *grid_dims = NULL;
    bool grid_fmt_file;
    ecl_file_enum grid_file_type =
        ecl_util_get_file_type(grid_file, &grid_fmt_file, NULL);

    if ((grid_file_type == ECL_GRID_FILE) ||
        (grid_file_type == ECL_EGRID_FILE)) {
        fortio_type *grid_fortio =
            fortio_open_reader(grid_file, grid_fmt_file, ECL_ENDIAN_FLIP);
        if (grid_fortio) {
            grid_dims = (ecl_grid_dims_type *)util_malloc(sizeof *grid_dims);
            grid_dims->dims_list = vector_alloc_new();

            {
                fortio_type *data_fortio = NULL;
                bool data_fmt_file;

                if (data_file) {
                    ecl_util_get_file_type(data_file, &data_fmt_file, NULL);
                    data_fortio = fortio_open_reader(data_file, data_fmt_file,
                                                     ECL_ENDIAN_FLIP);
                }

                if (grid_file_type == ECL_EGRID_FILE)
                    ecl_grid_dims_read_EGRID(grid_dims, grid_fortio,
                                             data_fortio);
                else
                    ecl_grid_dims_read_GRID(grid_dims, grid_fortio,
                                            data_fortio);

                if (data_fortio)
                    fortio_fclose(data_fortio);
            }
            fortio_fclose(grid_fortio);
        }
    }

    return grid_dims;
}

void ecl_grid_dims_free(ecl_grid_dims_type *grid_dims) {
    vector_free(grid_dims->dims_list);
    free(grid_dims);
}

int ecl_grid_dims_get_num_grids(const ecl_grid_dims_type *grid_dims) {
    return vector_get_size(grid_dims->dims_list);
}

const grid_dims_type *
ecl_grid_dims_iget_dims(const ecl_grid_dims_type *grid_dims, int grid_nr) {
    return (const grid_dims_type *)vector_iget_const(grid_dims->dims_list,
                                                     grid_nr);
}

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <ert/util/test_util.hpp>
#include <ert/util/stringlist.hpp>
#include <ert/util/util.h>

#include <ert/ecl/ecl_util.hpp>
#include <ert/ecl/ecl_file.hpp>
#include <ert/ecl/ecl_rsthead.hpp>
#include <ert/ecl/ecl_kw_magic.hpp>
#include <ert/ecl/ecl_grid.hpp>

#include <ert/ecl_well/well_conn_collection.hpp>
#include <ert/ecl_well/well_segment.hpp>
#include <ert/ecl_well/well_const.hpp>
#include <ert/ecl_well/well_segment_collection.hpp>
#include <ert/ecl_well/well_rseg_loader.hpp>

int main(int argc, char **argv) {
    const char *Xfile = argv[1];
    ecl_file_type *rst_file = ecl_file_open(Xfile, 0);
    ecl_file_view_type *rst_view = ecl_file_get_active_view(rst_file);
    ecl_rsthead_type *rst_head =
        ecl_rsthead_alloc(rst_view, ecl_util_filename_report_nr(Xfile));
    const ecl_kw_type *iwel_kw = ecl_file_iget_named_kw(rst_file, IWEL_KW, 0);
    const ecl_kw_type *iseg_kw = ecl_file_iget_named_kw(rst_file, ISEG_KW, 0);
    well_rseg_loader_type *rseg_loader = well_rseg_loader_alloc(rst_view);
    const ecl_kw_type *icon_kw = ecl_file_iget_named_kw(rst_file, ICON_KW, 0);
    const ecl_kw_type *scon_kw = NULL;
    const ecl_kw_type *xcon_kw = NULL;

    test_install_SIGNALS();
    test_assert_not_NULL(rst_file);
    test_assert_not_NULL(rst_head);
    {
        int well_nr;
        for (well_nr = 0; well_nr < rst_head->nwells; well_nr++) {
            well_conn_collection_type *connections =
                well_conn_collection_alloc();
            well_conn_collection_load_from_kw(connections, iwel_kw, icon_kw,
                                              scon_kw, xcon_kw, well_nr,
                                              rst_head);
            {
                well_segment_collection_type *segments =
                    well_segment_collection_alloc();
                bool load_segment_information = true;
                bool is_MSW_well = false;

                if (well_segment_collection_load_from_kw(
                        segments, well_nr, iwel_kw, iseg_kw, rseg_loader,
                        rst_head, load_segment_information, &is_MSW_well)) {
                    well_branch_collection_type *branches =
                        well_branch_collection_alloc();

                    test_assert_true(
                        well_segment_well_is_MSW(well_nr, iwel_kw, rst_head));
                    well_segment_collection_link(segments);
                    {
                        int is;
                        for (is = 0;
                             is < well_segment_collection_get_size(segments);
                             is++) {
                            well_segment_type *segment =
                                well_segment_collection_iget(segments, is);

                            if (well_segment_nearest_wellhead(segment))
                                test_assert_NULL(
                                    well_segment_get_outlet(segment));
                            else
                                test_assert_not_NULL(
                                    well_segment_get_outlet(segment));

                            test_assert_int_not_equal(
                                well_segment_get_id(segment),
                                well_segment_get_outlet_id(segment));
                            test_assert_ptr_not_equal(
                                segment, well_segment_get_outlet(segment));
                        }
                    }
                    well_segment_collection_add_branches(segments, branches);
                    {
                        int ib;
                        for (ib = 0;
                             ib < well_branch_collection_get_size(branches);
                             ib++) {
                            const well_segment_type *start_segment =
                                well_branch_collection_iget_start_segment(
                                    branches, ib);
                            const well_segment_type *segment = start_segment;

                            printf("Branch %d ", ib);
                            while (segment) {
                                printf("%d -> ", well_segment_get_id(segment));
                                segment = well_segment_get_outlet(segment);
                            }
                            printf(" X \n");
                        }
                    }
                    well_segment_collection_add_connections(
                        segments, ECL_GRID_GLOBAL_GRID, connections);
                    well_branch_collection_free(branches);
                } else
                    test_assert_false(
                        well_segment_well_is_MSW(well_nr, iwel_kw, rst_head));

                well_segment_collection_free(segments);
            }
            well_conn_collection_free(connections);
        }
    }

    ecl_file_close(rst_file);
    ecl_rsthead_free(rst_head);
    exit(0);
}

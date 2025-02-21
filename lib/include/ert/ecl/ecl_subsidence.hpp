#ifndef ERT_ECL_SUBSIDENCE_H
#define ERT_ECL_SUBSIDENCE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <ert/ecl/ecl_file.hpp>
#include <ert/ecl/ecl_file_view.hpp>
#include <ert/ecl/ecl_grid.hpp>
#include <ert/ecl/ecl_region.hpp>

typedef struct ecl_subsidence_struct ecl_subsidence_type;
typedef struct ecl_subsidence_survey_struct ecl_subsidence_survey_type;

void ecl_subsidence_free(ecl_subsidence_type *ecl_subsidence_config);
ecl_subsidence_type *ecl_subsidence_alloc(const ecl_grid_type *ecl_grid,
                                          const ecl_file_type *init_file);
ecl_subsidence_survey_type *
ecl_subsidence_add_survey_PRESSURE(ecl_subsidence_type *subsidence,
                                   const char *name,
                                   const ecl_file_view_type *restart_view);

bool ecl_subsidence_has_survey(const ecl_subsidence_type *subsidence,
                               const char *name);
double ecl_subsidence_eval(const ecl_subsidence_type *subsidence,
                           const char *base, const char *monitor,
                           ecl_region_type *region, double utm_x, double utm_y,
                           double depth, double compressibility,
                           double poisson_ratio);

double ecl_subsidence_eval_geertsma(const ecl_subsidence_type *subsidence,
                                    const char *base, const char *monitor,
                                    ecl_region_type *region, double utm_x,
                                    double utm_y, double depth,
                                    double youngs_modulus, double poisson_ratio,
                                    double seabed);

double ecl_subsidence_eval_geertsma_rporv(const ecl_subsidence_type *subsidence,
                                          const char *base, const char *monitor,
                                          ecl_region_type *region, double utm_x,
                                          double utm_y, double depth,
                                          double youngs_modulus,
                                          double poisson_ratio, double seabed);

#ifdef __cplusplus
}
#endif
#endif

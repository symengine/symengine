find_path(MPC_INCLUDE_DIR mpc.h)
find_library(MPC_LIBRARY mpc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPC DEFAULT_MSG MPC_INCLUDE_DIR MPC_LIBRARY)

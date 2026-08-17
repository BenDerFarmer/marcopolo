#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);

  if (!mkdir_if_not_exists(BUILD_FOLDER))
    return 1;

  Cmd cmd = {0};

  nob_cc(&cmd);
  nob_cc_flags(&cmd);
  nob_cc_output(&cmd, BUILD_FOLDER "marco");
  nob_cc_inputs(&cmd, SRC_FOLDER "marco.c");

  if (!nob_cmd_run(&cmd))
    return 1;

  nob_cc(&cmd);
  nob_cc_flags(&cmd);
  nob_cc_output(&cmd, BUILD_FOLDER "polo");
  nob_cc_inputs(&cmd, SRC_FOLDER "polo.c");

  if (!nob_cmd_run(&cmd))
    return 1;

  return 0;
}

#pragma once

typedef int ag_error;

enum ag_error {AG_ERR_SUCCESS = 0, AG_ERR_INVALID_PATH/*file doesnt exist*/, AG_ERR_INVALID_FILE/*file contents are wrong*/, AG_ERR_NOT_YET_SUPPORTED};

char* ag_error_readable(ag_error error);

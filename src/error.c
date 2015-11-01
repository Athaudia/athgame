#include "error.h"

char* ag_error__readable(ag_error error)
{
	switch(error)
	{
	case AG_ERR_SUCCESS:      return "Success!";
	case AG_ERR_INVALID_PATH: return "Invalid Path(File not Found)";
	case AG_ERR_INVALID_FILE: return "Invalid File(wrong format?)";
	case AG_ERR_NOT_YET_SUPPORTED: return "Not yet supported";
	default: return "Unknown error code";
	}
}

import os
from .cffi_helpers import get_lib_handle


_this_path = os.path.dirname(os.path.realpath(__file__))

_library_dir = os.getenv('PI_LIBRARY_DIR')
if _library_dir is None:
    _library_dir = os.path.join(_this_path, 'lib')

_include_dir = os.getenv('PI_INCLUDE_DIR')
if _include_dir is None:
    _include_dir = os.path.join(_this_path, 'include')

lraw_api, ffi_raw = get_lib_handle(
    ['-DPI_API='],
    'libcamera_raw_api.h',
    'camera_raw_api',
    _library_dir,
    _include_dir
)

lvid_api, ffi_vid = get_lib_handle(
    ['-DPI_API='],
    'libcamera_vid_api.h',
    'camera_vid_api',
    _library_dir,
    _include_dir
)


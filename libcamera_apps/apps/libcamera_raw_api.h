#ifndef LIBCAMERA_RAW_API_H
#define LIBCAMERA_RAW_API_H

#ifdef __cplusplus
extern "C" {
#endif

void *lraw_init();
int lraw_set_arg(void* inst, char* arg_name, char* arg_value);
int lraw_start_camera(void* inst);
int lraw_get_frame_size(void* inst);
int lraw_wait_for_frame(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms );
int lraw_wait_for_frame2(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms,
                                  unsigned int *rbuf, int rbuf_len, unsigned int *g1buf, int g1buf_len,
                                  unsigned int *g2buf, int g2buf_len,
                                  unsigned int *bbuf, int bbuf_len
                                   );
int lraw_stop_camera(void* inst);
int lraw_deinit(void* inst);
int lraw_print_parameters(void* inst);

#ifdef __cplusplus
}
#endif





#endif


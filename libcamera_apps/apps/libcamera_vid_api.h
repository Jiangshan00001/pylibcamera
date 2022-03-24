#ifndef LIBCAMERA_VID_API_H
#define LIBCAMERA_VID_API_H



#ifdef __cplusplus
extern "C" {
#endif

void* lvid_init();
int lvid_set_arg(void* inst, char* arg_name, char* arg_value);
int lvid_start_camera(void* inst);
int lvid_get_frame_size(void* inst);
int lvid_wait_for_frame(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms );
int lvid_wait_for_frame2(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms,
                                  unsigned int *rbuf, int rbuf_len, unsigned int *g1buf, int g1buf_len,
                                  unsigned int *g2buf, int g2buf_len,
                                  unsigned int *bbuf, int bbuf_len
                                   );
int lvid_stop_camera(void* inst);
int lvid_deinit(void* inst);
int lvid_print_parameters(void* inst);

#ifdef __cplusplus
}
#endif



#endif // LIBCAMERA_VID_API_H

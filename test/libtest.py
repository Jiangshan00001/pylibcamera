from pylibcamera import lraw_api as cam_raw
from pylibcamera import ffi_raw as ffi
import time
import numpy as np
import cv2
import sys



def imagesc_show(datab):
    print('datab shape:', datab.shape)
    height, width = datab.shape[0:2]

    mmax = np.max(datab)
    mmin = np.min(datab)
    print('max min:', mmax, mmin)

    hist1, bins = np.histogram(datab.ravel(), 256, [0, 4096])
    print(hist1)

    datab = datab.astype(np.float32)

    I_cv2_norm = (datab - mmin) * 255.0 / (mmax - mmin)

    I_cv2_norm = np.clip(I_cv2_norm, 0, 255)
    I_cv2_norm = I_cv2_norm.astype(np.uint8)

    img_uint16_data2 = cv2.resize(I_cv2_norm, (width // 4, height // 4), cv2.INTER_NEAREST)
    print('img shape:', img_uint16_data2.shape, img_uint16_data2.dtype)
    img_uint16_data2 = cv2.equalizeHist(img_uint16_data2)
    cv2.imshow('test', img_uint16_data2)
    cv2.waitKey(1)

if __name__=='__main__':

    width = 4056
    height = 3040

    #width = 1920
    #height = 1080


    print('libcamera_raw_init start')
    inst=cam_raw.lraw_init()
    cam_raw.lraw_print_parameters(inst)

    print('libcamera_raw_set_arg start')
    cam_raw.lraw_set_arg(inst, b'camera', b'0')
    cam_raw.lraw_set_arg(inst, b'transform', b'0')
    cam_raw.lraw_set_arg(inst, b'width', str(width).encode('utf-8'))
    cam_raw.lraw_set_arg(inst, b'height', str(height).encode('utf-8'))
    cam_raw.lraw_set_arg(inst, b'shutter', b'10.3')
    cam_raw.lraw_set_arg(inst, b'verbose', b'1')


    print('libcamera_raw_get_frame_size start')
    frame_size=cam_raw.lraw_get_frame_size(inst)
    print('frame size:', frame_size)


    #buffer=b'\x00'*frame_size
    print('libcamera_raw_start_camera start')
    cam_raw.lraw_start_camera(inst)



    print('libcamera_raw_wait_for_frame start')

    t1=time.time()
    cnt=0
    sec_cnt=0
    time_stamp = ffi.new("unsigned long [2]")

    #raw_buf = ffi.new("unsigned char [" + str(int(frame_size)) +']')

    size_of_one_color=int(width*height//2//2)
    #r_buf  = ffi.new("unsigned int [" + str(size_of_one_color) +']')
    #g1_buf  = ffi.new("unsigned int [" + str(size_of_one_color) +']')
    #g2_buf  = ffi.new("unsigned int [" + str(size_of_one_color) +']')
    #b_buf  = ffi.new("unsigned int [" + str(size_of_one_color) +']')
    r_buf_np=np.zeros((size_of_one_color), dtype=np.uint32)
    g1_buf_np=np.zeros((size_of_one_color), dtype=np.uint32)
    g2_buf_np=np.zeros((size_of_one_color), dtype=np.uint32)
    b_buf_np=np.zeros((size_of_one_color), dtype=np.uint32)

    r_buf=ffi.cast('unsigned int *', r_buf_np.ctypes.data)
    g1_buf=ffi.cast('unsigned int *', g1_buf_np.ctypes.data)
    g2_buf=ffi.cast('unsigned int *', g2_buf_np.ctypes.data)
    b_buf=ffi.cast('unsigned int *', b_buf_np.ctypes.data)

    buffer_np = np.zeros((frame_size), dtype=np.uint8)
    raw_buf = ffi.cast('unsigned char*', buffer_np.ctypes.data)



    while True:
        #cam_raw.libcamera_raw_wait_for_frame(inst, raw_buf,frame_size, time_stamp)
        cam_raw.lraw_wait_for_frame2(inst,raw_buf, frame_size, time_stamp,
                                              r_buf,size_of_one_color, g1_buf, size_of_one_color, g2_buf,size_of_one_color,
                                              b_buf, size_of_one_color)

        #imagesc_show(np.copy( g1_buf_np).reshape(height//2, width//2))

        cnt+=1
        if time.time()-t1>1.0:
            print('get frame:',cnt)
            cnt=0
            t1=time.time()
            sec_cnt+=1

        if sec_cnt>10.0:
            break


    print('libcamera_raw_stop_camera start')
    cam_raw.lraw_stop_camera(inst)

    print('libcamera_raw_deinit start')
    cam_raw.lraw_deinit(inst)


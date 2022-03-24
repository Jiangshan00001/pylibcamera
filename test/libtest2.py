from pylibcamera import lvid_api as cam_vid
from pylibcamera import ffi_vid as ffi
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


    print('cam_vid start')
    inst=cam_vid.lvid_init()

    print('cam_vid_set_arg start')
    cam_vid.lvid_set_arg(inst, b'camera', b'0')
    #cam_vid.lvid_set_arg(inst, b'transform', b'0')
    cam_vid.lvid_set_arg(inst, b'height', str(height).encode('utf-8'))
    cam_vid.lvid_set_arg(inst, b'width', str(width).encode('utf-8'))
    cam_vid.lvid_set_arg(inst, b'codec', b'mjpeg')
    cam_vid.lvid_set_arg(inst, b'quality', b'90')
    cam_vid.lvid_set_arg(inst, b'shutter', b'10000')
    cam_vid.lvid_set_arg(inst, b'verbose', b'1')

    cam_vid.lvid_print_parameters(inst)

    print('cam_vid_get_frame_size start')
    frame_size=cam_vid.lvid_get_frame_size(inst)
    print('frame size:', frame_size)


    #buffer=b'\x00'*frame_size
    print('cam_vid_start_camera start')
    cam_vid.lvid_start_camera(inst)



    print('cam_vid_wait_for_frame start')

    t1=time.time()
    cnt=0
    sec_cnt=0
    time_stamp = ffi.new("unsigned long [2]")

    #raw_buf = ffi.new("unsigned char [" + str(int(frame_size)) +']')

    size_of_one_color=int(width*height//2//2)

    buffer_np = np.zeros(frame_size, dtype=np.uint8)
    raw_buf = ffi.cast('unsigned char*', buffer_np.ctypes.data)



    file_index=0
    while True:
        siz = cam_vid.lvid_wait_for_frame(inst, raw_buf,frame_size, time_stamp)
        f=open('test_'+str(file_index)+'.jpg', 'wb')
        f.write(ffi.buffer(raw_buf, siz) )
        f.close()
        file_index+=1
        #print(siz)

        cnt+=1
        if time.time()-t1>1.0:
            print('get frame:',cnt)
            cnt=0
            t1=time.time()
            sec_cnt+=1

        if sec_cnt>3.0:
            break


    print('lvid_stop_camera start')
    cam_vid.lvid_stop_camera(inst)

    print('lvid_deinit start')
    cam_vid.lvid_deinit(inst)


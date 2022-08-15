# pylibcamera
a libcamera python bindings

their seems one offical port form:
https://github.com/kbingham
which you should use.
or

https://github.com/raspberrypi/picamera2


==========

this is used only on raspberry pi, maybe it could used on other platforms.

most of the c code is copyed from:
https://github.com/raspberrypi/libcamera-apps


# install(should only on raspberry pi)
```
pip3 install pylibcamera

```


python bindings for raw-image get:
```
from pylibcamera import lraw_api as cam_raw
from pylibcamera import ffi_raw as ffi
width = 4056
height = 3040
inst=cam_raw.lraw_init()

cam_raw.lraw_set_arg(inst, b'camera', b'0')
cam_raw.lraw_set_arg(inst, b'transform', b'0')
cam_raw.lraw_set_arg(inst, b'width', str(width).encode('utf-8'))
cam_raw.lraw_set_arg(inst, b'height', str(height).encode('utf-8'))
cam_raw.lraw_set_arg(inst, b'shutter', b'10.3')
cam_raw.lraw_set_arg(inst, b'verbose', b'1')

frame_size=cam_raw.lraw_get_frame_size(inst)

cam_raw.lraw_start_camera(inst)

t1=time.time()
cnt=0
sec_cnt=0
time_stamp = ffi.new("unsigned long [2]")
size_of_one_color=int(width*height//2//2)
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
	#cam_raw.libcamera_raw_wait_for_frame(inst, raw_buf,frame_size, time_stamp)--just get raw-frame
	cam_raw.lraw_wait_for_frame2(inst,raw_buf, 0, time_stamp,
										  r_buf,size_of_one_color, g1_buf, size_of_one_color, g2_buf,size_of_one_color,
										  b_buf, size_of_one_color) #get raw-splited frame

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

```

for mjpeg image get:

```


from pylibcamera import lvid_api as cam_vid
from pylibcamera import ffi_vid as ffi
import time
import numpy as np
import cv2
import sys


if __name__=='__main__':

    width = 4056
    height = 3040

    print('cam_vid start')
    inst=cam_vid.lvid_init()

    print('cam_vid_set_arg start')
    cam_vid.lvid_set_arg(inst, b'camera', b'0')
    cam_vid.lvid_set_arg(inst, b'height', str(height).encode('utf-8'))
    cam_vid.lvid_set_arg(inst, b'width', str(width).encode('utf-8'))
    cam_vid.lvid_set_arg(inst, b'codec', b'mjpeg')
    cam_vid.lvid_set_arg(inst, b'quality', b'90')
    cam_vid.lvid_set_arg(inst, b'shutter', b'10000')
    cam_vid.lvid_set_arg(inst, b'verbose', b'1')

    cam_vid.lvid_print_parameters(inst)

    frame_size=cam_vid.lvid_get_frame_size(inst)


    cam_vid.lvid_start_camera(inst)




    t1=time.time()
    cnt=0
    sec_cnt=0
    time_stamp = ffi.new("unsigned long [2]")

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


```



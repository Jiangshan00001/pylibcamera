/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * libcamera_raw.cpp - libcamera raw video record app.
 */

#include <chrono>
#include<string>

#include "core/libcamera_encoder.hpp"
#include "encoder/null_encoder.hpp"
#include "output/output.hpp"
#include "buffer_output.h"

using namespace std::placeholders;

class LibcameraRaw : public LibcameraEncoder
{
public:
    LibcameraRaw() : LibcameraEncoder() {
        VideoOptions* options = this->GetOptions();
        options->awb_gain_b=1;
        options->awb_gain_r=1;
        options->awb_index=0;
        options->bitrate=0;
        options->brightness=1.0;
        options->camera = 0;
        options->circular=1.0;
        options->lores_height=0;
        options->lores_width=0;
        options->transform=(libcamera::Transform)0;
        options->roi_x = options->roi_y = options->roi_width = options->roi_height = 0; // don't set digital zoom
        options->exposure="normal";
        options->exposure_index=0;
        options->ev=1.0;
        options->gain=1.0;
        options->metering="centre";
        options->metering_index=0;

        options->rawfull=0;


    }

    // std::unique_ptr<BufferOutput> output;
    std::unique_ptr<BufferOutput> output;

protected:
	// Force the use of "null" encoder.
	void createEncoder() { encoder_ = std::unique_ptr<Encoder>(new NullEncoder(GetOptions())); }
};



#if 1



unsigned int unpack_data(unsigned char b0,unsigned char  b1, unsigned char b2, int first_data )
{
    //data format:
    //  B0        B1        B2
    //#AAAAAAAA BBBBBBBB BBBBAAAA
    if(first_data)
    {
        return (((unsigned)b0)<<4)|(b2&0xf);
    }
    else
    {
        return (((unsigned)b1)<<4)|((b2>>4)&0xf);
    }
}

int set_data(unsigned int *buf, int buf_len, int x, int y, unsigned int dat, int width)
{
    int pos= y*width + x;
    if (pos>=buf_len)return -1;
    buf[pos] = dat;
    return 0;
}

#endif



#ifdef __cplusplus
extern "C" {
#endif


void* lraw_init()
{
	LibcameraRaw *app = new LibcameraRaw();

    VideoOptions *options = app->GetOptions();
    options->denoise = "cdn_off";
    options->nopreview = true;
    options->output="buffer";

    app->output =  std::unique_ptr<BufferOutput>(new BufferOutput(options));

    app->SetEncodeOutputReadyCallback(std::bind(&Output::OutputReady, app->output.get(), _1, _2, _3, _4));

	return app;
}
int lraw_set_arg(void* inst, char* arg_name, char* arg_value)
{
    LibcameraRaw *app = (LibcameraRaw *)inst;
	//(void*)app;
    VideoOptions *options = app->GetOptions();

    std::string name(arg_name);
    std::string val(arg_value);

    if(name=="width")
    {
        options->width = std::stol(val);
    }
    else if(name=="height")
    {
        options->height = std::stol(val);
    }
    else if(name=="shutter")
    {
        options->shutter = std::stof(val);
    }
    else if(name=="verbose")
    {
        options->verbose = std::stol(val);
    }
    else if(name=="transform")
    {
        options->transform = (libcamera::Transform)std::stol(val);
    }
    else if(name=="camera")
    {
        options->camera = std::stol(val);
    }


		//if (options->Parse(argc, argv))
	return 0;
}


int lraw_print_parameters(void* inst)
{
    LibcameraRaw *app = (LibcameraRaw *)inst;


    VideoOptions *options = app->GetOptions();
    {
        options->Print();
    }
    return 0;
}
int lraw_start_camera(void* inst)
{
	LibcameraRaw *app = (LibcameraRaw *)inst;


    VideoOptions *options = app->GetOptions();
    if (options->verbose)
    {
        options->Print();
    }


	app->OpenCamera();
	app->ConfigureVideo(LibcameraRaw::FLAG_VIDEO_RAW);
	app->StartEncoder();
	app->StartCamera();
	return 0;
}
int lraw_get_frame_size(void* inst)
{
    LibcameraRaw *app = (LibcameraRaw *)inst;


    VideoOptions *options = app->GetOptions();

    unsigned width = options->width;
    unsigned height = options->height;

    unsigned int bytes_per_line = width / 2 * 3;
    bytes_per_line = (bytes_per_line+31)/32*32;


    return bytes_per_line* height;
}


int lraw_wait_for_frame(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms)
{
		LibcameraRaw *app = (LibcameraRaw *)inst;
			LibcameraEncoder::Msg msg = app->Wait();
		if (msg.type == LibcameraEncoder::MsgType::Quit)
			return 0;
		else if (msg.type != LibcameraEncoder::MsgType::RequestComplete)
			throw std::runtime_error("unrecognised message!");

        CompletedRequestPtr &completed_request = std::get<CompletedRequestPtr>(msg.payload);
        app->EncodeBuffer(completed_request, app->RawStream());


        unsigned int flags;
        app->output->getBuffer(buf, buf_len,timestamp_ms, &flags);

		return 0;
}

int lraw_wait_for_frame2(void* inst, char*buf, int buf_len, unsigned long* timestamp_ms,
                                 unsigned int *rbuf, int rbuf_len, unsigned int *g1buf, int g1buf_len,
                                 unsigned int *g2buf, int g2buf_len,
                                 unsigned int *bbuf, int bbuf_len
                                  )
{
        LibcameraRaw *app = (LibcameraRaw *)inst;
            LibcameraEncoder::Msg msg = app->Wait();
        if (msg.type == LibcameraEncoder::MsgType::Quit)
            return 0;
        else if (msg.type != LibcameraEncoder::MsgType::RequestComplete)
            throw std::runtime_error("unrecognised message!");

        CompletedRequestPtr &completed_request = std::get<CompletedRequestPtr>(msg.payload);
        app->EncodeBuffer(completed_request, app->RawStream());


        //unsigned int flags;
        buf_t buf2= app->output->getBufferInner();

        //no buffer avaliable
        if (buf2.buf_size==0)return -1;
        buf = (char*)buf2.buf;
        buf_len = buf2.buf_size-8;
        memcpy(timestamp_ms, buf+buf_len, 4);


        if ((rbuf_len!=0 )||(g1buf_len!=0 )||(g2buf_len!=0 )||(bbuf_len!=0 ))
        {
            VideoOptions *options = app->GetOptions();

            unsigned width = options->width;
            //unsigned height = options->height;
            //B G1
            //G2 R
            unsigned int bytes_per_line = width / 2 * 3;
            bytes_per_line = (bytes_per_line+31)/32*32;


            for(int i=0;i<buf_len;i+=3)
            {
                //3bytes, 2-12bitdata

                unsigned x_2 = (i % bytes_per_line)/3 *2;
                if (x_2>=width)continue;

                unsigned y = i/bytes_per_line;

                unsigned int data1=unpack_data(buf[i],buf[i+1], buf[i+2], 0);
                unsigned int data2=unpack_data(buf[i],buf[i+1], buf[i+2], 1);
                if(y%2==0)
                {
                    set_data(bbuf,bbuf_len, x_2/2, y/2, data1,width/2);
                    set_data(g1buf,g1buf_len, x_2/2, y/2, data2,width/2);
                }
                else
                {
                    set_data(g2buf,g2buf_len, x_2/2, y/2, data1,width/2);
                    set_data(rbuf, rbuf_len, x_2/2, y/2, data2,width/2);
                }
            }
            //decode raw data to rgb
        }


        app->output->freeBufferInner(buf2);

        //app->ShowPreview(completed_request, app->VideoStream());
        return 0;
}

int lraw_stop_camera(void* inst)
{
    LibcameraRaw *app = (LibcameraRaw *)inst;
    app->StopCamera(); // stop complains if encoder very slow to close

    app->StopEncoder();
    return 0;
}
int lraw_deinit(void* inst)
{
	LibcameraRaw *app = new LibcameraRaw();
	delete app;
	return 0;
}




#ifdef __cplusplus
}
#endif






#if 0

// The main even loop for the application.

static void event_loop(LibcameraRaw &app)
{
    VideoOptions const *options = app.GetOptions();
    std::unique_ptr<Output> output = std::unique_ptr<Output>(Output::Create(options));
    app.SetEncodeOutputReadyCallback(std::bind(&Output::OutputReady, output.get(), _1, _2, _3, _4));

    app.OpenCamera();
    app.ConfigureVideo(LibcameraRaw::FLAG_VIDEO_RAW);
    app.StartEncoder();
    app.StartCamera();
    auto start_time = std::chrono::high_resolution_clock::now();

    for (unsigned int count = 0; ; count++)
    {
        LibcameraRaw::Msg msg = app.Wait();

        if (msg.type != LibcameraRaw::MsgType::RequestComplete)
            throw std::runtime_error("unrecognised message!");
        if (count == 0)
        {
            libcamera::StreamConfiguration const &cfg = app.RawStream()->configuration();
            std::cerr << "Raw stream: " << cfg.size.width << "x" << cfg.size.height << " stride " << cfg.stride
                      << " format " << cfg.pixelFormat.toString() << std::endl;
        }

        if (options->verbose)
            std::cerr << "Viewfinder frame " << count << std::endl;
        auto now = std::chrono::high_resolution_clock::now();
        if (options->timeout && now - start_time > std::chrono::milliseconds(options->timeout))
        {
            app.StopCamera();
            app.StopEncoder();
            return;
        }

        app.EncodeBuffer(std::get<CompletedRequestPtr>(msg.payload), app.RawStream());
    }
}


int main11(int argc, char *argv[])
{
    try
    {
        LibcameraRaw app;
        VideoOptions *options = app.GetOptions();
        if (options->Parse(argc, argv))
        {
            options->denoise = "cdn_off";
            options->nopreview = true;
            if (options->verbose)
                options->Print();

            event_loop(app);
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "ERROR: *** " << e.what() << " ***" << std::endl;
        return -1;
    }
    return 0;
}
#endif

#include "libcamera_vid_api.h"

#include <chrono>
#include <poll.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/stat.h>

#include "core/libcamera_encoder.hpp"
#include "output/output.hpp"
#include "buffer_output.h"


using namespace std::placeholders;





static int get_colourspace_flags(std::string const &codec)
{
    if (codec == "mjpeg" || codec == "yuv420")
        return LibcameraEncoder::FLAG_VIDEO_JPEG_COLOURSPACE;
    else
        return LibcameraEncoder::FLAG_VIDEO_NONE;
}


class LibcameraVid:public LibcameraEncoder
{
public:
    std::unique_ptr<BufferOutput> output;

};











#ifdef __cplusplus
extern "C" {
#endif

void *lvid_init()
{
    LibcameraVid *app = new LibcameraVid();
    VideoOptions *options = app->GetOptions();

    options->denoise = "cdn_off";
    options->nopreview = true;
    options->output="buffer";
    options->transform = (libcamera::Transform)0;
    options->awb_gain_r = 1.0;
    options->awb_gain_b = 1.0;
    options->awb="auto";
    options->awb_index=libcamera::controls::AwbAuto;

    options->bitrate=0;
    options->brightness=0.0;
    options->camera = 0;
    options->circular=1.0;
    options->lores_height=0;
    options->lores_width=0;
    options->transform=(libcamera::Transform)0;
    options->roi_x = options->roi_y = options->roi_width = options->roi_height = 0; // don't set digital zoom
    options->exposure="normal";
    options->exposure_index=libcamera::controls::ExposureNormal;
    options->ev=0.0;
    options->gain=1.0;
    options->metering="centre";
    options->metering_index=libcamera::controls::MeteringCentreWeighted;
    options->saturation = 1.0;
    options->sharpness = 1.0;
    options->contrast = 1.0;
    options->framerate = 30;
    options->denoise ="cdn_fast";

    options->circular = 0;
    options->split=0;
    options->segment = 0;
    options->initial = "record";
    options->signal = 0;
    options->keypress = 0;
    options->intra = 0;
    options->inline_headers = 0;
    options->viewfinder_height = 0;
    options->viewfinder_width = 0;
    options->pause = 0;
    options->timeout = 2000;



    options->rawfull=0;




    app->output =  std::unique_ptr<BufferOutput>(new BufferOutput(options));



    return (void*)app;


}


int lvid_set_arg(void *inst, char *arg_name, char *arg_value)
{
    LibcameraVid *app =  (LibcameraVid*)inst;

    VideoOptions *options = app->GetOptions();
#if 0
    std::vector<std::string> arg_list;
    arg_list.push_back("libcamera_vid");
    arg_list.push_back(std::string("--")+arg_name);
    arg_list.push_back(arg_value);

    //char *argv[3];
    //argv[0] = (char*)arg_list[0].c_str();
    //argv[1] = (char*)arg_list[1].c_str();
    //argv[2] = (char*)arg_list[2].c_str();
#endif
    std::string name = arg_name;
    std::string val = arg_value;

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
    else if(name=="gain")
    {
        options->gain = std::stof(val);
    }
    else if(name=="codec")
    {
        options->codec = val;
    }
    else if(name=="quality")
    {
        options->quality = std::stol(val);
    }
    //options->Parse(3, argv);
    return  0;

}


int lvid_start_camera(void *inst)
{
    LibcameraVid *app =  (LibcameraVid*)inst;

    VideoOptions const *options = app->GetOptions();
    app->output = std::unique_ptr<BufferOutput>(new BufferOutput(options));
    app->SetEncodeOutputReadyCallback(std::bind(&Output::OutputReady, app->output.get(), _1, _2, _3, _4));

    app->OpenCamera();
    app->ConfigureVideo(get_colourspace_flags(options->codec));
    app->StartEncoder();
    app->StartCamera();
    //auto start_time = std::chrono::high_resolution_clock::now();
    return 0;
}


int lvid_get_frame_size(void *inst)
{
    LibcameraVid *app =  (LibcameraVid*)inst;

    VideoOptions *options = app->GetOptions();

    unsigned width = options->width;
    unsigned height = options->height;

    unsigned int bytes_per_line = width  * 3;
    bytes_per_line = (bytes_per_line+31)/32*32;


    return bytes_per_line* height;
}


int lvid_wait_for_frame(void *inst, char *buf, int buf_len, unsigned long *timestamp_ms)
{
    LibcameraVid *app =  (LibcameraVid*)inst;


    LibcameraEncoder::Msg msg = app->Wait();
    if (msg.type == LibcameraEncoder::MsgType::Quit)
    {
        printf("lvid_wait_for_frame:msg quit\n");
        return 0;
    }
    else if (msg.type != LibcameraEncoder::MsgType::RequestComplete)
    {
        throw std::runtime_error("unrecognised message!");
        return 0;
    }

    CompletedRequestPtr &completed_request = std::get<CompletedRequestPtr>(msg.payload);
    app->EncodeBuffer(completed_request, app->VideoStream());
    //app->ShowPreview(completed_request, app->VideoStream());

    unsigned int flags;
    return (int )app->output->getBuffer(buf, buf_len,timestamp_ms, &flags);

    return 0;
}


int lvid_stop_camera(void *inst)
{
    LibcameraVid *app =  (LibcameraVid*)inst;

    app->StopCamera(); // stop complains if encoder very slow to close
    app->StopEncoder();
    return 0;
}


int lvid_deinit(void *inst)
{
    LibcameraVid *app =  (LibcameraVid*)inst;
    delete app;
    return 0;
}


int lvid_print_parameters(void *inst)
{
    LibcameraVid *app =  (LibcameraVid*)inst;

    VideoOptions *options = app->GetOptions();
    options->Print();
    return 0;
}


#ifdef __cplusplus
}
#endif





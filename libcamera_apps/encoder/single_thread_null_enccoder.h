#ifndef SINGLE_THREAD_NULL_ENCCODER_H
#define SINGLE_THREAD_NULL_ENCCODER_H


#include "core/video_options.hpp"
#include "encoder.hpp"

class SingleThreadNullEnccoder : public Encoder
{
public:
    SingleThreadNullEnccoder(VideoOptions const *options);

    ~SingleThreadNullEnccoder();
    void EncodeBuffer(int fd, size_t size, void *mem, StreamInfo const &info, int64_t timestamp_us) override;

    void get_buf();
    void free_buf();



    VideoOptions options_;
    struct OutputItem
    {
        void *mem;
        size_t length;
        int64_t timestamp_us;
    };
    std::queue<OutputItem> output_queue_;


};

#endif // SINGLE_THREAD_NULL_ENCCODER_H



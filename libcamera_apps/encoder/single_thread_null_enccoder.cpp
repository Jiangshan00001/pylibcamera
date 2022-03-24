#include "single_thread_null_enccoder.h"

SingleThreadNullEnccoder::SingleThreadNullEnccoder(const VideoOptions *options):
    Encoder(options)
{
    if (options->verbose)
        std::cerr << "Opened NullEncoder" << std::endl;

}

SingleThreadNullEnccoder::~SingleThreadNullEnccoder()
{
    if (options_.verbose)
        std::cerr << "NullEncoder closed" << std::endl;

}

void SingleThreadNullEnccoder::EncodeBuffer(int fd, size_t size, void *mem, const StreamInfo &info, int64_t timestamp_us)
{
    OutputItem item = { mem, size, timestamp_us };
    output_queue_.push(item);

}

void SingleThreadNullEnccoder::get_buf()
{
    OutputItem item;
    if (!output_queue_.empty())
    {
        item = output_queue_.front();
        output_queue_.pop();
    }
    output_ready_callback_(item.mem, item.length, item.timestamp_us, true);
}

void SingleThreadNullEnccoder::free_buf()
{
    input_done_callback_(nullptr);

}

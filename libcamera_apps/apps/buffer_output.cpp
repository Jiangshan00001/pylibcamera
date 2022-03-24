#include "buffer_output.h"

BufferOutput::BufferOutput(VideoOptions const *options)
: Output(options),  file_start_time_ms_(0)
{
}


BufferOutput::~BufferOutput()
{

}

int BufferOutput::getBuffer(char *buf, unsigned len, unsigned long *timestamp_ms, unsigned *flags)
{
    if(m_buf_vec.empty())
    {
        return -1;
    }
    buf_t src = getBufferInner();

    size_t copyed_len = src.buf_size-8>len?len:src.buf_size-8;
    //copy
    if(copyed_len>0){
        memcpy(buf, src.buf, copyed_len);
    }
    memcpy(&timestamp_ms, (unsigned char*)src.buf+src.buf_size-8, 4);
    memcpy(&flags, (unsigned char*)src.buf+src.buf_size-4, 4);
    //printf("getbuffer: %08x, %d. %d.  %d\n",(unsigned)src.buf, copyed_len,src.buf_size, (unsigned int)timestamp_ms );
    //free
    freeBufferInner(src);
    return (int)copyed_len;
}

buf_t BufferOutput::getBufferInner()
{    if(m_buf_vec.empty())
    {
        printf("BufferOutput::getBufferInner empty\n");

        return buf_t(0,0);
    }

    return m_buf_vec.front();
}

int BufferOutput::freeBufferInner(buf_t buf)
{
    m_buf_pool.free_buf(buf.buf);
    for(unsigned i=0;i<m_buf_vec.size();++i)
    {
        if (m_buf_vec[i].buf == buf.buf)
        {
            m_buf_vec.erase(m_buf_vec.begin()+i);
            return 0;
        }
    }
    printf("freeBufferInner error: no such buffer:%08x\n", (unsigned int )buf.buf);
    return -1;
}

int BufferOutput::getBufSize()
{
    if(m_buf_vec.empty()){return 0;}
    buf_t src = m_buf_vec.front();
    return src.buf_size-8;
}


void BufferOutput::outputBuffer(void *mem, size_t size, int64_t timestamp_us, uint32_t flags)
{


    void *dst = m_buf_pool.get_buf(size+8);
    if (dst==NULL)
    {
        //buffer full
        printf("BufferOutput::outputBuffer full\n");
        return;
    }

    timestamp_us/=1000;//ns->us->ms

    //printf("outputBuffer:%08x. %u\n", (unsigned)dst, size);
    memcpy(dst, mem, size);
    memcpy((unsigned char*)dst+size, &timestamp_us, 4);
    memcpy((unsigned char*)dst+size+4, &flags, 4);

    m_buf_vec.push_back(buf_t(dst, size+8));
    return;

}


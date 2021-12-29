#include "ByteStreamLiveSource.hh"  
#include "GroupsockHelper.hh"  
  
////////// ByteStreamLiveSource //////////  
#include <sys/stat.h>
#include <fcntl.h>
#include "LiveTSType.hh"  


ByteStreamLiveSource*  
ByteStreamLiveSource::createNew(UsageEnvironment& env,
                unsigned preferredFrameSize,
                unsigned playTimePerFrame)
{
    ByteStreamLiveSource* newSource
        = new ByteStreamLiveSource(env, preferredFrameSize, playTimePerFrame);
    return newSource;
}

ByteStreamLiveSource::ByteStreamLiveSource(UsageEnvironment& env,
                        unsigned preferredFrameSize,
                        unsigned playTimePerFrame)
  : FramedSource(env),fPreferredFrameSize(preferredFrameSize),
    fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
    fHaveStartedReading(False), fLimitNumBytesToStream(False), fNumBytesToStream(0)
{
    fLocalBuf = new QRingBuffer(REC_BUF_MAX_LEN);
    // buff = (char*)malloc(10*1024*1024);
}

ByteStreamLiveSource::~ByteStreamLiveSource()
{
    // free(buff);
}

void ByteStreamLiveSource::doGetNextFrame()
{
    if (fLimitNumBytesToStream && fNumBytesToStream == 0)
    {
        handleClosure();  
        return;  
    }

    doReadFromBuffer();  
}

void ByteStreamLiveSource::doStopGettingFrames()
{
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
}

int ByteStreamLiveSource::SubmitTsData(unsigned char* data, int size)
{
    printf("ByteStreamLiveSource SubmitTsData size = %d\n", size);
    if (fLocalBuf == NULL) {
        printf("[Error] fLocalBuf is NULL\n");
        return -1;
    }

    fLocalBuf->write((void*)data, size);
    // static int wAddr = 0;
    // memcpy(buff+wAddr, data, size);//cake
    // wAddr += size;

    return 0;
}

void ByteStreamLiveSource::doReadFromBuffer()
{
    printf("ByteStreamLiveSource doReadFromBuffer\n");
    if (fLocalBuf == NULL) {
        printf("[Error] fLocalBuf is NULL\n");
        return;
    }

    //初始化计数器
    unsigned int readLen = 0;
    unsigned int syncBytePosition = 0;

    // 没用，之前确定最多读取字节数的变量
    fMaxSize = fPreferredFrameSize;

    //初始化Frame Size
    unsigned int read_pkg_num = 0;
    //读同步字节
    // static char* pRead = buff;
    while(true){
        fLocalBuf->read(fTo, 1);
        if(fTo[0] == TRANSPORT_SYNC_BYTE) break;
        // memcpy(fTo, pRead, 1);
        // pRead += 1;
        // if(fTo[0] == TRANSPORT_SYNC_BYTE) break;
    }

    fLocalBuf->read(fTo + 1, 188 -1);
    // memcpy(fTo+1, pRead, 188-1);
    // pRead += 188-1;

    int pid = ((fTo[1] & 0x1f) << 8) + fTo[2];
    printf("ByteStreamLiveSource filter pid: 0x%x\n", pid);
    if(pid != 8191) read_pkg_num = 1;

    //
    for(; read_pkg_num < fPreferredFrameSize / 188;)
    {
        fLocalBuf->read(fTo + read_pkg_num * 188, 188);
        // memcpy(fTo + read_pkg_num * 188, pRead, 188);
        // pRead += 188;

        int pid = ((fTo[read_pkg_num * 188 + 1] & 0x1f) << 8) + fTo[read_pkg_num * 188 + 2];
        //printf("head = 0x%x read pid = %d  read_pkg_num = %d\n",fTo[read_pkg_num * 188], pid, read_pkg_num);
        if(pid != 8191) read_pkg_num++;
    }

    fFrameSize = fPreferredFrameSize;

   // int32_t bytes = write(mFd, fTo, fFrameSize);
    //printf("can Len = %d fFrameSize = %d \n", fLocalBuf->canRead(), fFrameSize);
    //for(int i = 0; i < 7; i++)
    //    printf("head = 0x%x\n", fTo[i * 188]);

    //for(int i = 0; i < 7; i++)
    //    printf("data = 0x%x\n", fTo[i * 5]);

    if (fFrameSize == 0)
    {
        handleClosure();
        return;
    }
    //fNumBytesToStream -= fFrameSize;

    // Set the 'presentation time':
    if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0)
    {  
        if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0)
        {
            // This is the first frame, so use the current time:
            gettimeofday(&fPresentationTime, NULL);
        }
        else
        {
            // Increment by the play time of the previous data:
            unsigned uSeconds   = fPresentationTime.tv_usec + fLastPlayTime;
            fPresentationTime.tv_sec += uSeconds/1000000;
            fPresentationTime.tv_usec = uSeconds%1000000;
        }

        // Remember the play time of this data:
        fLastPlayTime = (fPlayTimePerFrame*fFrameSize)/fPreferredFrameSize;
        fDurationInMicroseconds = fLastPlayTime;
    }
    else
    {
        // We don't know a specific play time duration for this data,
        // so just record the current time as being the 'presentation time':
        gettimeofday(&fPresentationTime, NULL);
    }
    //printf("time fPresentationTime.tv_sec = %lld,  fPresentationTime.tv_usec = %lld", fPresentationTime.tv_sec, fPresentationTime.tv_usec);
    // Inform the reader that he has data:
    // Because the file read was done from the event loop, we can call the
    // 'after getting' function directly, without risk of infinite recursion:
    FramedSource::afterGetting(this);
}

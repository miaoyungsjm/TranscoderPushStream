#include "MPEG2TransportLiveServerMediaSubsession.hh"
#include "SimpleRTPSink.hh"
#include "LiveTSType.hh"

MPEG2TransportLiveServerMediaSubsession*
MPEG2TransportLiveServerMediaSubsession::createNew(UsageEnvironment& env,
                Boolean reuseFirstSource)
{
    MPEG2TransportLiveServerMediaSubsession* newpSubsession = 
    new MPEG2TransportLiveServerMediaSubsession(env, reuseFirstSource);
    return newpSubsession;
}

MPEG2TransportLiveServerMediaSubsession  
::MPEG2TransportLiveServerMediaSubsession(UsageEnvironment& env,
                Boolean reuseFirstSource)
: OnDemandServerMediaSubsession(env, reuseFirstSource),
fDuration(0.0), fClientSessionHashTable(NULL), mpLiveSource(NULL)
{
}

MPEG2TransportLiveServerMediaSubsession
::~MPEG2TransportLiveServerMediaSubsession()
{
}

#define TRANSPORT_PACKET_SIZE 188
#define TRANSPORT_PACKETS_PER_NETWORK_PACKET 7
// The product of these two numbers must be enough to fit within a network packet

void MPEG2TransportLiveServerMediaSubsession
::startStream(unsigned clientSessionId, void* streamToken, TaskFunc* rtcpRRHandler,
          void* rtcpRRHandlerClientData, unsigned short& rtpSeqNum,
          unsigned& rtpTimestamp,
          ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
          void* serverRequestAlternativeByteHandlerClientData)
{
    // Call the original, default version of this routine:
    OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken,
                         rtcpRRHandler, rtcpRRHandlerClientData,
                         rtpSeqNum, rtpTimestamp,
                         serverRequestAlternativeByteHandler, serverRequestAlternativeByteHandlerClientData);  
}

void MPEG2TransportLiveServerMediaSubsession  
::pauseStream(unsigned clientSessionId, void* streamToken)   
{
    // Call the original, default version of this routine:  
    OnDemandServerMediaSubsession::pauseStream(clientSessionId, streamToken);  
}

void MPEG2TransportLiveServerMediaSubsession
::seekStream(unsigned clientSessionId, void* streamToken, double& seekNPT, double streamDuration, u_int64_t& numBytes)
{
    // Begin by calling the original, default version of this routine:  
    OnDemandServerMediaSubsession::seekStream(clientSessionId, streamToken, seekNPT, streamDuration, numBytes);
}

void MPEG2TransportLiveServerMediaSubsession
::setStreamScale(unsigned clientSessionId, void* streamToken, float scale)
{
    // Call the original, default version of this routine:
    OnDemandServerMediaSubsession::setStreamScale(clientSessionId, streamToken, scale);
}

void MPEG2TransportLiveServerMediaSubsession
::deleteStream(unsigned clientSessionId, void*& streamToken)
{
    // Call the original, default version of this routine:
    OnDemandServerMediaSubsession::deleteStream(clientSessionId, streamToken);
}

FramedSource* MPEG2TransportLiveServerMediaSubsession  
::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)   
{
    // Create the video source:
    unsigned const inputDataChunkSize
        = TRANSPORT_PACKETS_PER_NETWORK_PACKET * TRANSPORT_PACKET_SIZE;

    ByteStreamLiveSource* liveSource
        = ByteStreamLiveSource::createNew(envir(), inputDataChunkSize);
    printf("MPEG2TransportLiveServerMediaSubsession create liveSource = %p\n", liveSource);
    mpLiveSource = liveSource;
    estBitrate = 500; // kbps, estimate

    // Create a framer for the Transport Stream:
    MPEG2TransportStreamFramer* framer
        = MPEG2TransportStreamFramer::createNew(envir(), liveSource);

    return framer;
}

RTPSink* MPEG2TransportLiveServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
           unsigned char /*rtpPayloadTypeIfDynamic*/,
           FramedSource* /*inputSource*/)
{
    return SimpleRTPSink::createNew(envir(), rtpGroupsock,
                  33, 90000, "video", "MP2T",
                  1, True, False /*no 'M' bit*/);
}

void MPEG2TransportLiveServerMediaSubsession::testScaleFactor(float& scale)
{
    scale = 1.0f;
}

float MPEG2TransportLiveServerMediaSubsession::duration() const
{
    return fDuration;  
}

int MPEG2TransportLiveServerMediaSubsession::SubmitTsData(unsigned char* data, int size)
{
    // printf("MPEG2TransportLiveServerMediaSubsession SubmitTsData size = %d pkt = %d\n", size, size / 188);
    if (mpLiveSource) {
        mpLiveSource->SubmitTsData(data, size);
    }
    return 0;
}
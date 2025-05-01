/*********************************************\
 * 火山视窗PIV模块 - BASS音频                *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_BASS_HPP
#define _PIV_BASS_HPP

#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#ifndef PIV_BASS_FUNC_DEF
#define PIV_BASS_FUNC_DEF(_f) (WINAPI * p##_f)
#endif
#ifndef PIV_BASS_FUNC_VAR
#define PIV_BASS_FUNC_VAR(_f) p##_f _f = nullptr;
#endif
#ifndef PIV_BASS_FUNC_GET
#define PIV_BASS_FUNC_GET(_f)  \
    _f = reinterpret_cast<p##_f>(::GetProcAddress(m_hmodule, #_f));  \
    ASSERT(_f != nullptr);
#endif

// bass.h ---------------------------------------------------------------------
#ifndef BASS_H
#define BASS_H

#ifdef _WIN32
#ifdef WINAPI_FAMILY
#include <winapifamily.h>
#endif
#include <wtypes.h>
typedef unsigned __int64 QWORD;
#else
#include <stdint.h>
#define WINAPI
#define CALLBACK
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
#ifdef __OBJC__
typedef int BOOL32;
#define BOOL BOOL32 // override objc's BOOL
#else
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define LOBYTE(a) (BYTE)(a)
#define HIBYTE(a) (BYTE)((a) >> 8)
#define LOWORD(a) (WORD)(a)
#define HIWORD(a) (WORD)((a) >> 16)
#define MAKEWORD(a, b) (WORD)(((a) & 0xff) | ((b) << 8))
#define MAKELONG(a, b) (DWORD)(((a) & 0xffff) | ((b) << 16))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define BASSVERSION 0x204 // API version
#define BASSVERSIONTEXT "2.4"

#ifndef BASSDEF
#define BASSDEF(f) WINAPI f
#else
#define NOBASSOVERLOADS
#endif

typedef DWORD HMUSIC;   // MOD music handle
typedef DWORD HSAMPLE;  // sample handle
typedef DWORD HCHANNEL; // sample playback handle
typedef DWORD HSTREAM;  // sample stream handle
typedef DWORD HRECORD;  // recording handle
typedef DWORD HSYNC;    // synchronizer handle
typedef DWORD HDSP;     // DSP handle
typedef DWORD HFX;      // effect handle
typedef DWORD HPLUGIN;  // plugin handle

// Error codes returned by BASS_ErrorGetCode
#define BASS_OK 0                  // all is OK
#define BASS_ERROR_MEM 1           // memory error
#define BASS_ERROR_FILEOPEN 2      // can't open the file
#define BASS_ERROR_DRIVER 3        // can't find a free/valid driver
#define BASS_ERROR_BUFLOST 4       // the sample buffer was lost
#define BASS_ERROR_HANDLE 5        // invalid handle
#define BASS_ERROR_FORMAT 6        // unsupported sample format
#define BASS_ERROR_POSITION 7      // invalid position
#define BASS_ERROR_INIT 8          // BASS_Init has not been successfully called
#define BASS_ERROR_START 9         // BASS_Start has not been successfully called
#define BASS_ERROR_SSL 10          // SSL/HTTPS support isn't available
#define BASS_ERROR_REINIT 11       // device needs to be reinitialized
#define BASS_ERROR_ALREADY 14      // already initialized/paused/whatever
#define BASS_ERROR_NOTAUDIO 17     // file does not contain audio
#define BASS_ERROR_NOCHAN 18       // can't get a free channel
#define BASS_ERROR_ILLTYPE 19      // an illegal type was specified
#define BASS_ERROR_ILLPARAM 20     // an illegal parameter was specified
#define BASS_ERROR_NO3D 21         // no 3D support
#define BASS_ERROR_NOEAX 22        // no EAX support
#define BASS_ERROR_DEVICE 23       // illegal device number
#define BASS_ERROR_NOPLAY 24       // not playing
#define BASS_ERROR_FREQ 25         // illegal sample rate
#define BASS_ERROR_NOTFILE 27      // the stream is not a file stream
#define BASS_ERROR_NOHW 29         // no hardware voices available
#define BASS_ERROR_EMPTY 31        // the file has no sample data
#define BASS_ERROR_NONET 32        // no internet connection could be opened
#define BASS_ERROR_CREATE 33       // couldn't create the file
#define BASS_ERROR_NOFX 34         // effects are not available
#define BASS_ERROR_NOTAVAIL 37     // requested data/action is not available
#define BASS_ERROR_DECODE 38       // the channel is/isn't a "decoding channel"
#define BASS_ERROR_DX 39           // a sufficient DirectX version is not installed
#define BASS_ERROR_TIMEOUT 40      // connection timedout
#define BASS_ERROR_FILEFORM 41     // unsupported file format
#define BASS_ERROR_SPEAKER 42      // unavailable speaker
#define BASS_ERROR_VERSION 43      // invalid BASS version (used by add-ons)
#define BASS_ERROR_CODEC 44        // codec is not available/supported
#define BASS_ERROR_ENDED 45        // the channel/file has ended
#define BASS_ERROR_BUSY 46         // the device is busy
#define BASS_ERROR_UNSTREAMABLE 47 // unstreamable file
#define BASS_ERROR_PROTOCOL 48     // unsupported protocol
#define BASS_ERROR_DENIED 49       // access denied
#define BASS_ERROR_UNKNOWN -1      // some other mystery problem

// BASS_SetConfig options
#define BASS_CONFIG_BUFFER 0
#define BASS_CONFIG_UPDATEPERIOD 1
#define BASS_CONFIG_GVOL_SAMPLE 4
#define BASS_CONFIG_GVOL_STREAM 5
#define BASS_CONFIG_GVOL_MUSIC 6
#define BASS_CONFIG_CURVE_VOL 7
#define BASS_CONFIG_CURVE_PAN 8
#define BASS_CONFIG_FLOATDSP 9
#define BASS_CONFIG_3DALGORITHM 10
#define BASS_CONFIG_NET_TIMEOUT 11
#define BASS_CONFIG_NET_BUFFER 12
#define BASS_CONFIG_PAUSE_NOPLAY 13
#define BASS_CONFIG_NET_PREBUF 15
#define BASS_CONFIG_NET_PASSIVE 18
#define BASS_CONFIG_REC_BUFFER 19
#define BASS_CONFIG_NET_PLAYLIST 21
#define BASS_CONFIG_MUSIC_VIRTUAL 22
#define BASS_CONFIG_VERIFY 23
#define BASS_CONFIG_UPDATETHREADS 24
#define BASS_CONFIG_DEV_BUFFER 27
#define BASS_CONFIG_REC_LOOPBACK 28
#define BASS_CONFIG_VISTA_TRUEPOS 30
#define BASS_CONFIG_IOS_SESSION 34
#define BASS_CONFIG_IOS_MIXAUDIO 34
#define BASS_CONFIG_DEV_DEFAULT 36
#define BASS_CONFIG_NET_READTIMEOUT 37
#define BASS_CONFIG_VISTA_SPEAKERS 38
#define BASS_CONFIG_IOS_SPEAKER 39
#define BASS_CONFIG_MF_DISABLE 40
#define BASS_CONFIG_HANDLES 41
#define BASS_CONFIG_UNICODE 42
#define BASS_CONFIG_SRC 43
#define BASS_CONFIG_SRC_SAMPLE 44
#define BASS_CONFIG_ASYNCFILE_BUFFER 45
#define BASS_CONFIG_OGG_PRESCAN 47
#define BASS_CONFIG_MF_VIDEO 48
#define BASS_CONFIG_AIRPLAY 49
#define BASS_CONFIG_DEV_NONSTOP 50
#define BASS_CONFIG_IOS_NOCATEGORY 51
#define BASS_CONFIG_VERIFY_NET 52
#define BASS_CONFIG_DEV_PERIOD 53
#define BASS_CONFIG_FLOAT 54
#define BASS_CONFIG_NET_SEEK 56
#define BASS_CONFIG_AM_DISABLE 58
#define BASS_CONFIG_NET_PLAYLIST_DEPTH 59
#define BASS_CONFIG_NET_PREBUF_WAIT 60
#define BASS_CONFIG_ANDROID_SESSIONID 62
#define BASS_CONFIG_WASAPI_PERSIST 65
#define BASS_CONFIG_REC_WASAPI 66
#define BASS_CONFIG_ANDROID_AAUDIO 67
#define BASS_CONFIG_SAMPLE_ONEHANDLE 69
#define BASS_CONFIG_NET_META 71
#define BASS_CONFIG_NET_RESTRATE 72
#define BASS_CONFIG_REC_DEFAULT 73
#define BASS_CONFIG_NORAMP 74

// BASS_SetConfigPtr options
#define BASS_CONFIG_NET_AGENT 16
#define BASS_CONFIG_NET_PROXY 17
#define BASS_CONFIG_IOS_NOTIFY 46
#define BASS_CONFIG_ANDROID_JAVAVM 63
#define BASS_CONFIG_LIBSSL 64
#define BASS_CONFIG_FILENAME 75

#define BASS_CONFIG_THREAD 0x40000000 // flag: thread-specific setting

// BASS_CONFIG_IOS_SESSION flags
#define BASS_IOS_SESSION_MIX 1
#define BASS_IOS_SESSION_DUCK 2
#define BASS_IOS_SESSION_AMBIENT 4
#define BASS_IOS_SESSION_SPEAKER 8
#define BASS_IOS_SESSION_DISABLE 16
#define BASS_IOS_SESSION_DEACTIVATE 32
#define BASS_IOS_SESSION_AIRPLAY 64
#define BASS_IOS_SESSION_BTHFP 128
#define BASS_IOS_SESSION_BTA2DP 0x100

// BASS_Init flags
#define BASS_DEVICE_8BITS 1            // unused
#define BASS_DEVICE_MONO 2             // mono
#define BASS_DEVICE_3D 4               // unused
#define BASS_DEVICE_16BITS 8           // limit output to 16-bit
#define BASS_DEVICE_REINIT 128         // reinitialize
#define BASS_DEVICE_LATENCY 0x100      // unused
#define BASS_DEVICE_CPSPEAKERS 0x400   // unused
#define BASS_DEVICE_SPEAKERS 0x800     // force enabling of speaker assignment
#define BASS_DEVICE_NOSPEAKER 0x1000   // ignore speaker arrangement
#define BASS_DEVICE_DMIX 0x2000        // use ALSA "dmix" plugin
#define BASS_DEVICE_FREQ 0x4000        // set device sample rate
#define BASS_DEVICE_STEREO 0x8000      // limit output to stereo
#define BASS_DEVICE_HOG 0x10000        // hog/exclusive mode
#define BASS_DEVICE_AUDIOTRACK 0x20000 // use AudioTrack output
#define BASS_DEVICE_DSOUND 0x40000     // use DirectSound output
#define BASS_DEVICE_SOFTWARE 0x80000   // disable hardware/fastpath output

// DirectSound interfaces (for use with BASS_GetDSoundObject)
#define BASS_OBJECT_DS 1    // IDirectSound
#define BASS_OBJECT_DS3DL 2 // IDirectSound3DListener

// Device info structure
typedef struct
{
#if defined(_WIN32_WCE) || (defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
    const wchar_t *name;   // description
    const wchar_t *driver; // driver
#else
    const char *name;   // description
    const char *driver; // driver
#endif
    DWORD flags;
} BASS_DEVICEINFO;

// BASS_DEVICEINFO flags
#define BASS_DEVICE_ENABLED 1
#define BASS_DEVICE_DEFAULT 2
#define BASS_DEVICE_INIT 4
#define BASS_DEVICE_LOOPBACK 8
#define BASS_DEVICE_DEFAULTCOM 128

#define BASS_DEVICE_TYPE_MASK 0xff000000
#define BASS_DEVICE_TYPE_NETWORK 0x01000000
#define BASS_DEVICE_TYPE_SPEAKERS 0x02000000
#define BASS_DEVICE_TYPE_LINE 0x03000000
#define BASS_DEVICE_TYPE_HEADPHONES 0x04000000
#define BASS_DEVICE_TYPE_MICROPHONE 0x05000000
#define BASS_DEVICE_TYPE_HEADSET 0x06000000
#define BASS_DEVICE_TYPE_HANDSET 0x07000000
#define BASS_DEVICE_TYPE_DIGITAL 0x08000000
#define BASS_DEVICE_TYPE_SPDIF 0x09000000
#define BASS_DEVICE_TYPE_HDMI 0x0a000000
#define BASS_DEVICE_TYPE_DISPLAYPORT 0x40000000

// BASS_GetDeviceInfo flags
#define BASS_DEVICES_AIRPLAY 0x1000000

typedef struct
{
    DWORD flags;     // device capabilities (DSCAPS_xxx flags)
    DWORD hwsize;    // unused
    DWORD hwfree;    // unused
    DWORD freesam;   // unused
    DWORD free3d;    // unused
    DWORD minrate;   // unused
    DWORD maxrate;   // unused
    BOOL eax;        // unused
    DWORD minbuf;    // recommended minimum buffer length in ms
    DWORD dsver;     // DirectSound version
    DWORD latency;   // average delay (in ms) before start of playback
    DWORD initflags; // BASS_Init "flags" parameter
    DWORD speakers;  // number of speakers available
    DWORD freq;      // current output rate
} BASS_INFO;

// BASS_INFO flags (from DSOUND.H)
#define DSCAPS_EMULDRIVER 0x00000020 // device does not have hardware DirectSound support
#define DSCAPS_CERTIFIED 0x00000040  // device driver has been certified by Microsoft

#define DSCAPS_HARDWARE 0x80000000 // hardware mixed

// Recording device info structure
typedef struct
{
    DWORD flags;   // device capabilities (DSCCAPS_xxx flags)
    DWORD formats; // supported standard formats (WAVE_FORMAT_xxx flags)
    DWORD inputs;  // number of inputs
    BOOL singlein; // TRUE = only 1 input can be set at a time
    DWORD freq;    // current input rate
} BASS_RECORDINFO;

// BASS_RECORDINFO flags (from DSOUND.H)
#define DSCCAPS_EMULDRIVER DSCAPS_EMULDRIVER // device does not have hardware DirectSound recording support
#define DSCCAPS_CERTIFIED DSCAPS_CERTIFIED   // device driver has been certified by Microsoft

// defines for formats field of BASS_RECORDINFO (from MMSYSTEM.H)
#ifndef WAVE_FORMAT_1M08
#define WAVE_FORMAT_1M08 0x00000001 /* 11.025 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_1S08 0x00000002 /* 11.025 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_1M16 0x00000004 /* 11.025 kHz, Mono,   16-bit */
#define WAVE_FORMAT_1S16 0x00000008 /* 11.025 kHz, Stereo, 16-bit */
#define WAVE_FORMAT_2M08 0x00000010 /* 22.05  kHz, Mono,   8-bit  */
#define WAVE_FORMAT_2S08 0x00000020 /* 22.05  kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_2M16 0x00000040 /* 22.05  kHz, Mono,   16-bit */
#define WAVE_FORMAT_2S16 0x00000080 /* 22.05  kHz, Stereo, 16-bit */
#define WAVE_FORMAT_4M08 0x00000100 /* 44.1   kHz, Mono,   8-bit  */
#define WAVE_FORMAT_4S08 0x00000200 /* 44.1   kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_4M16 0x00000400 /* 44.1   kHz, Mono,   16-bit */
#define WAVE_FORMAT_4S16 0x00000800 /* 44.1   kHz, Stereo, 16-bit */
#endif

// Sample info structure
typedef struct
{
    DWORD freq;     // default playback rate
    float volume;   // default volume (0-1)
    float pan;      // default pan (-1=left, 0=middle, 1=right)
    DWORD flags;    // BASS_SAMPLE_xxx flags
    DWORD length;   // length (in bytes)
    DWORD max;      // maximum simultaneous playbacks
    DWORD origres;  // original resolution
    DWORD chans;    // number of channels
    DWORD mingap;   // minimum gap (ms) between creating channels
    DWORD mode3d;   // BASS_3DMODE_xxx mode
    float mindist;  // minimum distance
    float maxdist;  // maximum distance
    DWORD iangle;   // angle of inside projection cone
    DWORD oangle;   // angle of outside projection cone
    float outvol;   // delta-volume outside the projection cone
    DWORD vam;      // unused
    DWORD priority; // unused
} BASS_SAMPLE;

#define BASS_SAMPLE_8BITS 1           // 8 bit
#define BASS_SAMPLE_FLOAT 256         // 32 bit floating-point
#define BASS_SAMPLE_MONO 2            // mono
#define BASS_SAMPLE_LOOP 4            // looped
#define BASS_SAMPLE_3D 8              // 3D functionality
#define BASS_SAMPLE_SOFTWARE 16       // unused
#define BASS_SAMPLE_MUTEMAX 32        // mute at max distance (3D only)
#define BASS_SAMPLE_VAM 64            // unused
#define BASS_SAMPLE_FX 128            // unused
#define BASS_SAMPLE_OVER_VOL 0x10000  // override lowest volume
#define BASS_SAMPLE_OVER_POS 0x20000  // override longest playing
#define BASS_SAMPLE_OVER_DIST 0x30000 // override furthest from listener (3D only)

#define BASS_STREAM_PRESCAN 0x20000  // scan file for accurate seeking and length
#define BASS_STREAM_AUTOFREE 0x40000 // automatically free the stream when it stops/ends
#define BASS_STREAM_RESTRATE 0x80000 // restrict the download rate of internet file stream
#define BASS_STREAM_BLOCK 0x100000   // download internet file stream in small blocks
#define BASS_STREAM_DECODE 0x200000  // don't play the stream, only decode
#define BASS_STREAM_STATUS 0x800000  // give server status info (HTTP/ICY tags) in DOWNLOADPROC

#define BASS_MP3_IGNOREDELAY 0x200 // ignore LAME/Xing/VBRI/iTunes delay & padding info
#define BASS_MP3_SETPOS BASS_STREAM_PRESCAN

#define BASS_MUSIC_FLOAT BASS_SAMPLE_FLOAT
#define BASS_MUSIC_MONO BASS_SAMPLE_MONO
#define BASS_MUSIC_LOOP BASS_SAMPLE_LOOP
#define BASS_MUSIC_3D BASS_SAMPLE_3D
#define BASS_MUSIC_FX BASS_SAMPLE_FX
#define BASS_MUSIC_AUTOFREE BASS_STREAM_AUTOFREE
#define BASS_MUSIC_DECODE BASS_STREAM_DECODE
#define BASS_MUSIC_PRESCAN BASS_STREAM_PRESCAN // calculate playback length
#define BASS_MUSIC_CALCLEN BASS_MUSIC_PRESCAN
#define BASS_MUSIC_RAMP 0x200          // normal ramping
#define BASS_MUSIC_RAMPS 0x400         // sensitive ramping
#define BASS_MUSIC_SURROUND 0x800      // surround sound
#define BASS_MUSIC_SURROUND2 0x1000    // surround sound (mode 2)
#define BASS_MUSIC_FT2PAN 0x2000       // apply FastTracker 2 panning to XM files
#define BASS_MUSIC_FT2MOD 0x2000       // play .MOD as FastTracker 2 does
#define BASS_MUSIC_PT1MOD 0x4000       // play .MOD as ProTracker 1 does
#define BASS_MUSIC_NONINTER 0x10000    // non-interpolated sample mixing
#define BASS_MUSIC_SINCINTER 0x800000  // sinc interpolated sample mixing
#define BASS_MUSIC_POSRESET 0x8000     // stop all notes when moving position
#define BASS_MUSIC_POSRESETEX 0x400000 // stop all notes and reset bmp/etc when moving position
#define BASS_MUSIC_STOPBACK 0x80000    // stop the music on a backwards jump effect
#define BASS_MUSIC_NOSAMPLE 0x100000   // don't load the samples

// Speaker assignment flags
#define BASS_SPEAKER_FRONT 0x1000000  // front speakers
#define BASS_SPEAKER_REAR 0x2000000   // rear speakers
#define BASS_SPEAKER_CENLFE 0x3000000 // center & LFE speakers (5.1)
#define BASS_SPEAKER_SIDE 0x4000000   // side speakers (7.1)
#define BASS_SPEAKER_N(n) ((n) << 24) // n'th pair of speakers (max 15)
#define BASS_SPEAKER_LEFT 0x10000000  // modifier: left
#define BASS_SPEAKER_RIGHT 0x20000000 // modifier: right
#define BASS_SPEAKER_FRONTLEFT BASS_SPEAKER_FRONT | BASS_SPEAKER_LEFT
#define BASS_SPEAKER_FRONTRIGHT BASS_SPEAKER_FRONT | BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_REARLEFT BASS_SPEAKER_REAR | BASS_SPEAKER_LEFT
#define BASS_SPEAKER_REARRIGHT BASS_SPEAKER_REAR | BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_CENTER BASS_SPEAKER_CENLFE | BASS_SPEAKER_LEFT
#define BASS_SPEAKER_LFE BASS_SPEAKER_CENLFE | BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_SIDELEFT BASS_SPEAKER_SIDE | BASS_SPEAKER_LEFT
#define BASS_SPEAKER_SIDERIGHT BASS_SPEAKER_SIDE | BASS_SPEAKER_RIGHT
#define BASS_SPEAKER_REAR2 BASS_SPEAKER_SIDE
#define BASS_SPEAKER_REAR2LEFT BASS_SPEAKER_SIDELEFT
#define BASS_SPEAKER_REAR2RIGHT BASS_SPEAKER_SIDERIGHT

#define BASS_ASYNCFILE 0x40000000 // read file asynchronously
#define BASS_UNICODE 0x80000000   // UTF-16

#define BASS_RECORD_ECHOCANCEL 0x2000
#define BASS_RECORD_AGC 0x4000
#define BASS_RECORD_PAUSE 0x8000 // start recording paused

// DX7 voice allocation & management flags
#define BASS_VAM_HARDWARE 1
#define BASS_VAM_SOFTWARE 2
#define BASS_VAM_TERM_TIME 4
#define BASS_VAM_TERM_DIST 8
#define BASS_VAM_TERM_PRIO 16

// Channel info structure
typedef struct
{
    DWORD freq;  // default playback rate
    DWORD chans; // channels
    DWORD flags;
    DWORD ctype;   // type of channel
    DWORD origres; // original resolution
    HPLUGIN plugin;
    HSAMPLE sample;
    const char *filename;
} BASS_CHANNELINFO;

#define BASS_ORIGRES_FLOAT 0x10000

// BASS_CHANNELINFO types
#define BASS_CTYPE_SAMPLE 1
#define BASS_CTYPE_RECORD 2
#define BASS_CTYPE_STREAM 0x10000
#define BASS_CTYPE_STREAM_VORBIS 0x10002
#define BASS_CTYPE_STREAM_OGG 0x10002
#define BASS_CTYPE_STREAM_MP1 0x10003
#define BASS_CTYPE_STREAM_MP2 0x10004
#define BASS_CTYPE_STREAM_MP3 0x10005
#define BASS_CTYPE_STREAM_AIFF 0x10006
#define BASS_CTYPE_STREAM_CA 0x10007
#define BASS_CTYPE_STREAM_MF 0x10008
#define BASS_CTYPE_STREAM_AM 0x10009
#define BASS_CTYPE_STREAM_SAMPLE 0x1000a
#define BASS_CTYPE_STREAM_DUMMY 0x18000
#define BASS_CTYPE_STREAM_DEVICE 0x18001
#define BASS_CTYPE_STREAM_WAV 0x40000 // WAVE flag (LOWORD=codec)
#define BASS_CTYPE_STREAM_WAV_PCM 0x50001
#define BASS_CTYPE_STREAM_WAV_FLOAT 0x50003
#define BASS_CTYPE_MUSIC_MOD 0x20000
#define BASS_CTYPE_MUSIC_MTM 0x20001
#define BASS_CTYPE_MUSIC_S3M 0x20002
#define BASS_CTYPE_MUSIC_XM 0x20003
#define BASS_CTYPE_MUSIC_IT 0x20004
#define BASS_CTYPE_MUSIC_MO3 0x00100 // MO3 flag

// BASS_PluginLoad flags
#define BASS_PLUGIN_PROC 1

typedef struct
{
    DWORD ctype; // channel type
#if defined(_WIN32_WCE) || (defined(WINAPI_FAMILY) && WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP)
    const wchar_t *name; // format description
    const wchar_t *exts; // file extension filter (*.ext1;*.ext2;etc...)
#else
    const char *name; // format description
    const char *exts; // file extension filter (*.ext1;*.ext2;etc...)
#endif
} BASS_PLUGINFORM;

typedef struct
{
    DWORD version;                  // version (same form as BASS_GetVersion)
    DWORD formatc;                  // number of formats
    const BASS_PLUGINFORM *formats; // the array of formats
} BASS_PLUGININFO;

// 3D vector (for 3D positions/velocities/orientations)
typedef struct BASS_3DVECTOR
{
#ifdef __cplusplus
    BASS_3DVECTOR() {}
    BASS_3DVECTOR(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
#endif
    float x; // +=right, -=left
    float y; // +=up, -=down
    float z; // +=front, -=behind
} BASS_3DVECTOR;

// 3D channel modes
#define BASS_3DMODE_NORMAL 0   // normal 3D processing
#define BASS_3DMODE_RELATIVE 1 // position is relative to the listener
#define BASS_3DMODE_OFF 2      // no 3D processing

// software 3D mixing algorithms (used with BASS_CONFIG_3DALGORITHM)
#define BASS_3DALG_DEFAULT 0
#define BASS_3DALG_OFF 1
#define BASS_3DALG_FULL 2
#define BASS_3DALG_LIGHT 3

// BASS_SampleGetChannel flags
#define BASS_SAMCHAN_NEW 1    // get a new playback channel
#define BASS_SAMCHAN_STREAM 2 // create a stream

typedef DWORD(CALLBACK STREAMPROC)(HSTREAM handle, void *buffer, DWORD length, void *user);
/* User stream callback function.
handle : The stream that needs writing
buffer : Buffer to write the samples in
length : Number of bytes to write
user   : The 'user' parameter value given when calling BASS_StreamCreate
RETURN : Number of bytes written. Set the BASS_STREAMPROC_END flag to end the stream. */

#define BASS_STREAMPROC_END 0x80000000 // end of user stream flag

// Special STREAMPROCs
#define STREAMPROC_DUMMY (STREAMPROC *)0      // "dummy" stream
#define STREAMPROC_PUSH (STREAMPROC *)-1      // push stream
#define STREAMPROC_DEVICE (STREAMPROC *)-2    // device mix stream
#define STREAMPROC_DEVICE_3D (STREAMPROC *)-3 // device 3D mix stream

// BASS_StreamCreateFileUser file systems
#define STREAMFILE_NOBUFFER 0
#define STREAMFILE_BUFFER 1
#define STREAMFILE_BUFFERPUSH 2

// User file stream callback functions
typedef void(CALLBACK FILECLOSEPROC)(void *user);
typedef QWORD(CALLBACK FILELENPROC)(void *user);
typedef DWORD(CALLBACK FILEREADPROC)(void *buffer, DWORD length, void *user);
typedef BOOL(CALLBACK FILESEEKPROC)(QWORD offset, void *user);

typedef struct
{
    FILECLOSEPROC *close;
    FILELENPROC *length;
    FILEREADPROC *read;
    FILESEEKPROC *seek;
} BASS_FILEPROCS;

// BASS_StreamPutFileData options
#define BASS_FILEDATA_END 0 // end & close the file

// BASS_StreamGetFilePosition modes
#define BASS_FILEPOS_CURRENT 0
#define BASS_FILEPOS_DECODE BASS_FILEPOS_CURRENT
#define BASS_FILEPOS_DOWNLOAD 1
#define BASS_FILEPOS_END 2
#define BASS_FILEPOS_START 3
#define BASS_FILEPOS_CONNECTED 4
#define BASS_FILEPOS_BUFFER 5
#define BASS_FILEPOS_SOCKET 6
#define BASS_FILEPOS_ASYNCBUF 7
#define BASS_FILEPOS_SIZE 8
#define BASS_FILEPOS_BUFFERING 9
#define BASS_FILEPOS_AVAILABLE 10

typedef void(CALLBACK DOWNLOADPROC)(const void *buffer, DWORD length, void *user);
/* Internet stream download callback function.
buffer : Buffer containing the downloaded data... NULL=end of download
length : Number of bytes in the buffer
user   : The 'user' parameter value given when calling BASS_StreamCreateURL */

// BASS_ChannelSetSync types
#define BASS_SYNC_POS 0
#define BASS_SYNC_END 2
#define BASS_SYNC_META 4
#define BASS_SYNC_SLIDE 5
#define BASS_SYNC_STALL 6
#define BASS_SYNC_DOWNLOAD 7
#define BASS_SYNC_FREE 8
#define BASS_SYNC_SETPOS 11
#define BASS_SYNC_MUSICPOS 10
#define BASS_SYNC_MUSICINST 1
#define BASS_SYNC_MUSICFX 3
#define BASS_SYNC_OGG_CHANGE 12
#define BASS_SYNC_DEV_FAIL 14
#define BASS_SYNC_DEV_FORMAT 15
#define BASS_SYNC_THREAD 0x20000000  // flag: call sync in other thread
#define BASS_SYNC_MIXTIME 0x40000000 // flag: sync at mixtime, else at playtime
#define BASS_SYNC_ONETIME 0x80000000 // flag: sync only once, else continuously

typedef void(CALLBACK SYNCPROC)(HSYNC handle, DWORD channel, DWORD data, void *user);
/* Sync callback function.
handle : The sync that has occured
channel: Channel that the sync occured in
data   : Additional data associated with the sync's occurance
user   : The 'user' parameter given when calling BASS_ChannelSetSync */

typedef void(CALLBACK DSPPROC)(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user);
/* DSP callback function.
handle : The DSP handle
channel: Channel that the DSP is being applied to
buffer : Buffer to apply the DSP to
length : Number of bytes in the buffer
user   : The 'user' parameter given when calling BASS_ChannelSetDSP */

typedef BOOL(CALLBACK RECORDPROC)(HRECORD handle, const void *buffer, DWORD length, void *user);
/* Recording callback function.
handle : The recording handle
buffer : Buffer containing the recorded sample data
length : Number of bytes
user   : The 'user' parameter value given when calling BASS_RecordStart
RETURN : TRUE = continue recording, FALSE = stop */

// BASS_ChannelIsActive return values
#define BASS_ACTIVE_STOPPED 0
#define BASS_ACTIVE_PLAYING 1
#define BASS_ACTIVE_STALLED 2
#define BASS_ACTIVE_PAUSED 3
#define BASS_ACTIVE_PAUSED_DEVICE 4

// Channel attributes
#define BASS_ATTRIB_FREQ 1
#define BASS_ATTRIB_VOL 2
#define BASS_ATTRIB_PAN 3
#define BASS_ATTRIB_EAXMIX 4
#define BASS_ATTRIB_NOBUFFER 5
#define BASS_ATTRIB_VBR 6
#define BASS_ATTRIB_CPU 7
#define BASS_ATTRIB_SRC 8
#define BASS_ATTRIB_NET_RESUME 9
#define BASS_ATTRIB_SCANINFO 10
#define BASS_ATTRIB_NORAMP 11
#define BASS_ATTRIB_BITRATE 12
#define BASS_ATTRIB_BUFFER 13
#define BASS_ATTRIB_GRANULE 14
#define BASS_ATTRIB_USER 15
#define BASS_ATTRIB_TAIL 16
#define BASS_ATTRIB_PUSH_LIMIT 17
#define BASS_ATTRIB_DOWNLOADPROC 18
#define BASS_ATTRIB_VOLDSP 19
#define BASS_ATTRIB_VOLDSP_PRIORITY 20
#define BASS_ATTRIB_MUSIC_AMPLIFY 0x100
#define BASS_ATTRIB_MUSIC_PANSEP 0x101
#define BASS_ATTRIB_MUSIC_PSCALER 0x102
#define BASS_ATTRIB_MUSIC_BPM 0x103
#define BASS_ATTRIB_MUSIC_SPEED 0x104
#define BASS_ATTRIB_MUSIC_VOL_GLOBAL 0x105
#define BASS_ATTRIB_MUSIC_ACTIVE 0x106
#define BASS_ATTRIB_MUSIC_VOL_CHAN 0x200 // + channel #
#define BASS_ATTRIB_MUSIC_VOL_INST 0x300 // + instrument #

// BASS_ChannelSlideAttribute flags
#define BASS_SLIDE_LOG 0x1000000

// BASS_ChannelGetData flags
#define BASS_DATA_AVAILABLE 0         // query how much data is buffered
#define BASS_DATA_NOREMOVE 0x10000000 // flag: don't remove data from recording buffer
#define BASS_DATA_FIXED 0x20000000    // unused
#define BASS_DATA_FLOAT 0x40000000    // flag: return floating-point sample data
#define BASS_DATA_FFT256 0x80000000   // 256 sample FFT
#define BASS_DATA_FFT512 0x80000001   // 512 FFT
#define BASS_DATA_FFT1024 0x80000002  // 1024 FFT
#define BASS_DATA_FFT2048 0x80000003  // 2048 FFT
#define BASS_DATA_FFT4096 0x80000004  // 4096 FFT
#define BASS_DATA_FFT8192 0x80000005  // 8192 FFT
#define BASS_DATA_FFT16384 0x80000006 // 16384 FFT
#define BASS_DATA_FFT32768 0x80000007 // 32768 FFT
#define BASS_DATA_FFT_INDIVIDUAL 0x10 // FFT flag: FFT for each channel, else all combined
#define BASS_DATA_FFT_NOWINDOW 0x20   // FFT flag: no Hanning window
#define BASS_DATA_FFT_REMOVEDC 0x40   // FFT flag: pre-remove DC bias
#define BASS_DATA_FFT_COMPLEX 0x80    // FFT flag: return complex data
#define BASS_DATA_FFT_NYQUIST 0x100   // FFT flag: return extra Nyquist value

// BASS_ChannelGetLevelEx flags
#define BASS_LEVEL_MONO 1      // get mono level
#define BASS_LEVEL_STEREO 2    // get stereo level
#define BASS_LEVEL_RMS 4       // get RMS levels
#define BASS_LEVEL_VOLPAN 8    // apply VOL/PAN attributes to the levels
#define BASS_LEVEL_NOREMOVE 16 // don't remove data from recording buffer

// BASS_ChannelGetTags types : what's returned
#define BASS_TAG_ID3 0                 // ID3v1 tags : TAG_ID3 structure
#define BASS_TAG_ID3V2 1               // ID3v2 tags : variable length block
#define BASS_TAG_OGG 2                 // OGG comments : series of null-terminated UTF-8 strings
#define BASS_TAG_HTTP 3                // HTTP headers : series of null-terminated ASCII strings
#define BASS_TAG_ICY 4                 // ICY headers : series of null-terminated ANSI strings
#define BASS_TAG_META 5                // ICY metadata : ANSI string
#define BASS_TAG_APE 6                 // APE tags : series of null-terminated UTF-8 strings
#define BASS_TAG_MP4 7                 // MP4/iTunes metadata : series of null-terminated UTF-8 strings
#define BASS_TAG_WMA 8                 // WMA tags : series of null-terminated UTF-8 strings
#define BASS_TAG_VENDOR 9              // OGG encoder : UTF-8 string
#define BASS_TAG_LYRICS3 10            // Lyric3v2 tag : ASCII string
#define BASS_TAG_CA_CODEC 11           // CoreAudio codec info : TAG_CA_CODEC structure
#define BASS_TAG_MF 13                 // Media Foundation tags : series of null-terminated UTF-8 strings
#define BASS_TAG_WAVEFORMAT 14         // WAVE format : WAVEFORMATEEX structure
#define BASS_TAG_AM_NAME 16            // Android Media codec name : ASCII string
#define BASS_TAG_ID3V2_2 17            // ID3v2 tags (2nd block) : variable length block
#define BASS_TAG_AM_MIME 18            // Android Media MIME type : ASCII string
#define BASS_TAG_LOCATION 19           // redirected URL : ASCII string
#define BASS_TAG_RIFF_INFO 0x100       // RIFF "INFO" tags : series of null-terminated ANSI strings
#define BASS_TAG_RIFF_BEXT 0x101       // RIFF/BWF "bext" tags : TAG_BEXT structure
#define BASS_TAG_RIFF_CART 0x102       // RIFF/BWF "cart" tags : TAG_CART structure
#define BASS_TAG_RIFF_DISP 0x103       // RIFF "DISP" text tag : ANSI string
#define BASS_TAG_RIFF_CUE 0x104        // RIFF "cue " chunk : TAG_CUE structure
#define BASS_TAG_RIFF_SMPL 0x105       // RIFF "smpl" chunk : TAG_SMPL structure
#define BASS_TAG_APE_BINARY 0x1000     // + index #, binary APE tag : TAG_APE_BINARY structure
#define BASS_TAG_MUSIC_NAME 0x10000    // MOD music name : ANSI string
#define BASS_TAG_MUSIC_MESSAGE 0x10001 // MOD message : ANSI string
#define BASS_TAG_MUSIC_ORDERS 0x10002  // MOD order list : BYTE array of pattern numbers
#define BASS_TAG_MUSIC_AUTH 0x10003    // MOD author : UTF-8 string
#define BASS_TAG_MUSIC_INST 0x10100    // + instrument #, MOD instrument name : ANSI string
#define BASS_TAG_MUSIC_CHAN 0x10200    // + channel #, MOD channel name : ANSI string
#define BASS_TAG_MUSIC_SAMPLE 0x10300  // + sample #, MOD sample name : ANSI string

// ID3v1 tag structure
typedef struct
{
    char id[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    BYTE genre;
} TAG_ID3;

// Binary APE tag structure
typedef struct
{
    const char *key;
    const void *data;
    DWORD length;
} TAG_APE_BINARY;

// BWF "bext" tag structure
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200)
#endif
#pragma pack(push, 1)
typedef struct
{
    char Description[256];        // description
    char Originator[32];          // name of the originator
    char OriginatorReference[32]; // reference of the originator
    char OriginationDate[10];     // date of creation (yyyy-mm-dd)
    char OriginationTime[8];      // time of creation (hh-mm-ss)
    QWORD TimeReference;          // first sample count since midnight (little-endian)
    WORD Version;                 // BWF version (little-endian)
    BYTE UMID[64];                // SMPTE UMID
    BYTE Reserved[190];
#if defined(__GNUC__) && __GNUC__ < 3
    char CodingHistory[0]; // history
#elif 1                        // change to 0 if compiler fails the following line
    char CodingHistory[]; // history
#else
    char CodingHistory[1]; // history
#endif
} TAG_BEXT;
#pragma pack(pop)

// BWF "cart" tag structures
typedef struct
{
    DWORD dwUsage; // FOURCC timer usage ID
    DWORD dwValue; // timer value in samples from head
} TAG_CART_TIMER;

typedef struct
{
    char Version[4];             // version of the data structure
    char Title[64];              // title of cart audio sequence
    char Artist[64];             // artist or creator name
    char CutID[64];              // cut number identification
    char ClientID[64];           // client identification
    char Category[64];           // category ID, PSA, NEWS, etc
    char Classification[64];     // classification or auxiliary key
    char OutCue[64];             // out cue text
    char StartDate[10];          // yyyy-mm-dd
    char StartTime[8];           // hh:mm:ss
    char EndDate[10];            // yyyy-mm-dd
    char EndTime[8];             // hh:mm:ss
    char ProducerAppID[64];      // name of vendor or application
    char ProducerAppVersion[64]; // version of producer application
    char UserDef[64];            // user defined text
    DWORD dwLevelReference;      // sample value for 0 dB reference
    TAG_CART_TIMER PostTimer[8]; // 8 time markers after head
    char Reserved[276];
    char URL[1024]; // uniform resource locator
#if defined(__GNUC__) && __GNUC__ < 3
    char TagText[0]; // free form text for scripts or tags
#elif 1                  // change to 0 if compiler fails the following line
    char TagText[]; // free form text for scripts or tags
#else
    char TagText[1]; // free form text for scripts or tags
#endif
} TAG_CART;

// RIFF "cue " tag structures
typedef struct
{
    DWORD dwName;
    DWORD dwPosition;
    DWORD fccChunk;
    DWORD dwChunkStart;
    DWORD dwBlockStart;
    DWORD dwSampleOffset;
} TAG_CUE_POINT;

typedef struct
{
    DWORD dwCuePoints;
#if defined(__GNUC__) && __GNUC__ < 3
        TAG_CUE_POINT CuePoints[0];
#elif 1 // change to 0 if compiler fails the following line
    TAG_CUE_POINT CuePoints[];
#else
    TAG_CUE_POINT CuePoints[1];
#endif
} TAG_CUE;

// RIFF "smpl" tag structures
typedef struct
{
    DWORD dwIdentifier;
    DWORD dwType;
    DWORD dwStart;
    DWORD dwEnd;
    DWORD dwFraction;
    DWORD dwPlayCount;
} TAG_SMPL_LOOP;

typedef struct
{
    DWORD dwManufacturer;
    DWORD dwProduct;
    DWORD dwSamplePeriod;
    DWORD dwMIDIUnityNote;
    DWORD dwMIDIPitchFraction;
    DWORD dwSMPTEFormat;
    DWORD dwSMPTEOffset;
    DWORD cSampleLoops;
    DWORD cbSamplerData;
#if defined(__GNUC__) && __GNUC__ < 3
    TAG_SMPL_LOOP SampleLoops[0];
#elif 1 // change to 0 if compiler fails the following line
    TAG_SMPL_LOOP SampleLoops[];
#else
    TAG_SMPL_LOOP SampleLoops[1];
#endif
} TAG_SMPL;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// CoreAudio codec info structure
typedef struct
{
    DWORD ftype;      // file format
    DWORD atype;      // audio format
    const char *name; // description
} TAG_CA_CODEC;

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
#pragma pack(push, 1)
typedef struct tWAVEFORMATEX
{
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#pragma pack(pop)
#endif

// BASS_ChannelGetLength/GetPosition/SetPosition modes
#define BASS_POS_BYTE 0              // byte position
#define BASS_POS_MUSIC_ORDER 1       // order.row position, MAKELONG(order,row)
#define BASS_POS_OGG 3               // OGG bitstream number
#define BASS_POS_END 0x10            // trimmed end position
#define BASS_POS_LOOP 0x11           // loop start positiom
#define BASS_POS_FLUSH 0x1000000     // flag: flush decoder/FX buffers
#define BASS_POS_RESET 0x2000000     // flag: reset user file buffers
#define BASS_POS_RELATIVE 0x4000000  // flag: seek relative to the current position
#define BASS_POS_INEXACT 0x8000000   // flag: allow seeking to inexact position
#define BASS_POS_DECODE 0x10000000   // flag: get the decoding (not playing) position
#define BASS_POS_DECODETO 0x20000000 // flag: decode to the position instead of seeking
#define BASS_POS_SCAN 0x40000000     // flag: scan to the position

// BASS_ChannelSetDevice/GetDevice option
#define BASS_NODEVICE 0x20000

// BASS_RecordSetInput flags
#define BASS_INPUT_OFF 0x10000
#define BASS_INPUT_ON 0x20000

#define BASS_INPUT_TYPE_MASK 0xff000000
#define BASS_INPUT_TYPE_UNDEF 0x00000000
#define BASS_INPUT_TYPE_DIGITAL 0x01000000
#define BASS_INPUT_TYPE_LINE 0x02000000
#define BASS_INPUT_TYPE_MIC 0x03000000
#define BASS_INPUT_TYPE_SYNTH 0x04000000
#define BASS_INPUT_TYPE_CD 0x05000000
#define BASS_INPUT_TYPE_PHONE 0x06000000
#define BASS_INPUT_TYPE_SPEAKER 0x07000000
#define BASS_INPUT_TYPE_WAVE 0x08000000
#define BASS_INPUT_TYPE_AUX 0x09000000
#define BASS_INPUT_TYPE_ANALOG 0x0a000000

// BASS_ChannelSetFX effect types
#define BASS_FX_DX8_CHORUS 0
#define BASS_FX_DX8_COMPRESSOR 1
#define BASS_FX_DX8_DISTORTION 2
#define BASS_FX_DX8_ECHO 3
#define BASS_FX_DX8_FLANGER 4
#define BASS_FX_DX8_GARGLE 5
#define BASS_FX_DX8_I3DL2REVERB 6
#define BASS_FX_DX8_PARAMEQ 7
#define BASS_FX_DX8_REVERB 8
#define BASS_FX_VOLUME 9

typedef struct
{
    float fWetDryMix;
    float fDepth;
    float fFeedback;
    float fFrequency;
    DWORD lWaveform; // 0=triangle, 1=sine
    float fDelay;
    DWORD lPhase; // BASS_DX8_PHASE_xxx
} BASS_DX8_CHORUS;

typedef struct
{
    float fGain;
    float fAttack;
    float fRelease;
    float fThreshold;
    float fRatio;
    float fPredelay;
} BASS_DX8_COMPRESSOR;

typedef struct
{
    float fGain;
    float fEdge;
    float fPostEQCenterFrequency;
    float fPostEQBandwidth;
    float fPreLowpassCutoff;
} BASS_DX8_DISTORTION;

typedef struct
{
    float fWetDryMix;
    float fFeedback;
    float fLeftDelay;
    float fRightDelay;
    BOOL lPanDelay;
} BASS_DX8_ECHO;

typedef struct
{
    float fWetDryMix;
    float fDepth;
    float fFeedback;
    float fFrequency;
    DWORD lWaveform; // 0=triangle, 1=sine
    float fDelay;
    DWORD lPhase; // BASS_DX8_PHASE_xxx
} BASS_DX8_FLANGER;

typedef struct
{
    DWORD dwRateHz;    // Rate of modulation in hz
    DWORD dwWaveShape; // 0=triangle, 1=square
} BASS_DX8_GARGLE;

typedef struct
{
    int lRoom;                 // [-10000, 0]      default: -1000 mB
    int lRoomHF;               // [-10000, 0]      default: 0 mB
    float flRoomRolloffFactor; // [0.0, 10.0]      default: 0.0
    float flDecayTime;         // [0.1, 20.0]      default: 1.49s
    float flDecayHFRatio;      // [0.1, 2.0]       default: 0.83
    int lReflections;          // [-10000, 1000]   default: -2602 mB
    float flReflectionsDelay;  // [0.0, 0.3]       default: 0.007 s
    int lReverb;               // [-10000, 2000]   default: 200 mB
    float flReverbDelay;       // [0.0, 0.1]       default: 0.011 s
    float flDiffusion;         // [0.0, 100.0]     default: 100.0 %
    float flDensity;           // [0.0, 100.0]     default: 100.0 %
    float flHFReference;       // [20.0, 20000.0]  default: 5000.0 Hz
} BASS_DX8_I3DL2REVERB;

typedef struct
{
    float fCenter;
    float fBandwidth;
    float fGain;
} BASS_DX8_PARAMEQ;

typedef struct
{
    float fInGain;          // [-96.0,0.0]            default: 0.0 dB
    float fReverbMix;       // [-96.0,0.0]            default: 0.0 db
    float fReverbTime;      // [0.001,3000.0]         default: 1000.0 ms
    float fHighFreqRTRatio; // [0.001,0.999]          default: 0.001
} BASS_DX8_REVERB;

#define BASS_DX8_PHASE_NEG_180 0
#define BASS_DX8_PHASE_NEG_90 1
#define BASS_DX8_PHASE_ZERO 2
#define BASS_DX8_PHASE_90 3
#define BASS_DX8_PHASE_180 4

typedef struct
{
    float fTarget;
    float fCurrent;
    float fTime;
    DWORD lCurve;
} BASS_FX_VOLUME_PARAM;

typedef void(CALLBACK IOSNOTIFYPROC)(DWORD status);
/* iOS notification callback function.
status : The notification (BASS_IOSNOTIFY_xxx) */

#define BASS_IOSNOTIFY_INTERRUPT 1     // interruption started
#define BASS_IOSNOTIFY_INTERRUPT_END 2 // interruption ended

#ifdef __cplusplus
}
#endif

#ifdef __OBJC__
#undef BOOL
#endif

#endif // BASS_H

// bassmix.h ------------------------------------------------------------------

#ifndef BASSMIX_H
#define BASSMIX_H

#if BASSVERSION != 0x204
#error conflicting BASS and BASSmix versions
#endif

#ifdef __OBJC__
typedef int BOOL32;
#define BOOL BOOL32 // override objc's BOOL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// Additional BASS_SetConfig options
#define BASS_CONFIG_MIXER_BUFFER 0x10601
#define BASS_CONFIG_MIXER_POSEX 0x10602
#define BASS_CONFIG_SPLIT_BUFFER 0x10610

// BASS_Mixer_StreamCreate flags
#define BASS_MIXER_RESUME 0x1000    // resume stalled immediately upon new/unpaused source
#define BASS_MIXER_POSEX 0x2000     // enable BASS_Mixer_ChannelGetPositionEx support
#define BASS_MIXER_NOSPEAKER 0x4000 // ignore speaker arrangement
#define BASS_MIXER_QUEUE 0x8000     // queue sources
#define BASS_MIXER_END 0x10000      // end the stream when there are no sources
#define BASS_MIXER_NONSTOP 0x20000  // don't stall when there are no sources

// BASS_Mixer_StreamAddChannel/Ex flags
#define BASS_MIXER_CHAN_ABSOLUTE 0x1000   // start is an absolute position
#define BASS_MIXER_CHAN_BUFFER 0x2000     // buffer data for BASS_Mixer_ChannelGetData/Level
#define BASS_MIXER_CHAN_LIMIT 0x4000      // limit mixer processing to the amount available from this source
#define BASS_MIXER_CHAN_MATRIX 0x10000    // matrix mixing
#define BASS_MIXER_CHAN_PAUSE 0x20000     // don't process the source
#define BASS_MIXER_CHAN_DOWNMIX 0x400000  // downmix to stereo/mono
#define BASS_MIXER_CHAN_NORAMPIN 0x800000 // don't ramp-in the start
#define BASS_MIXER_BUFFER BASS_MIXER_CHAN_BUFFER
#define BASS_MIXER_LIMIT BASS_MIXER_CHAN_LIMIT
#define BASS_MIXER_MATRIX BASS_MIXER_CHAN_MATRIX
#define BASS_MIXER_PAUSE BASS_MIXER_CHAN_PAUSE
#define BASS_MIXER_DOWNMIX BASS_MIXER_CHAN_DOWNMIX
#define BASS_MIXER_NORAMPIN BASS_MIXER_CHAN_NORAMPIN

// Mixer attributes
#define BASS_ATTRIB_MIXER_LATENCY 0x15000
#define BASS_ATTRIB_MIXER_THREADS 0x15001
#define BASS_ATTRIB_MIXER_VOL 0x15002

// Additional BASS_Mixer_ChannelIsActive return values
#define BASS_ACTIVE_WAITING 5
#define BASS_ACTIVE_QUEUED 6

// BASS_Split_StreamCreate flags
#define BASS_SPLIT_SLAVE 0x1000 // only read buffered data
#define BASS_SPLIT_POS 0x2000

// Splitter attributes
#define BASS_ATTRIB_SPLIT_ASYNCBUFFER 0x15010
#define BASS_ATTRIB_SPLIT_ASYNCPERIOD 0x15011

// Envelope node
typedef struct
{
    QWORD pos;
    float value;
} BASS_MIXER_NODE;

// Envelope types
#define BASS_MIXER_ENV_FREQ 1
#define BASS_MIXER_ENV_VOL 2
#define BASS_MIXER_ENV_PAN 3
#define BASS_MIXER_ENV_LOOP 0x10000   // flag: loop
#define BASS_MIXER_ENV_REMOVE 0x20000 // flag: remove at end

// Additional sync types
#define BASS_SYNC_MIXER_ENVELOPE 0x10200
#define BASS_SYNC_MIXER_ENVELOPE_NODE 0x10201
#define BASS_SYNC_MIXER_QUEUE 0x10202

// Additional BASS_Mixer_ChannelSetPosition flag
#define BASS_POS_MIXER_RESET 0x10000 // flag: clear mixer's playback buffer

// Additional BASS_Mixer_ChannelGetPosition mode
#define BASS_POS_MIXER_DELAY 5

// BASS_CHANNELINFO types
#define BASS_CTYPE_STREAM_MIXER 0x10800
#define BASS_CTYPE_STREAM_SPLIT 0x10801

#ifdef __cplusplus
}
#endif

#ifdef __OBJC__
#undef BOOL
#endif

#endif // BASSMIX_H

// basswasapi.h ------------------------------------------------------------------

#ifndef BASSWASAPI_H
#define BASSWASAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

// Additional error codes returned by BASS_ErrorGetCode
#define BASS_ERROR_WASAPI 5000          // no WASAPI
#define BASS_ERROR_WASAPI_BUFFER 5001   // buffer size is invalid
#define BASS_ERROR_WASAPI_CATEGORY 5002 // can't set category
#define BASS_ERROR_WASAPI_DENIED 5003   // access denied

// Device info structure
typedef struct
{
    const char *name;
    const char *id;
    DWORD type;
    DWORD flags;
    float minperiod;
    float defperiod;
    DWORD mixfreq;
    DWORD mixchans;
} BASS_WASAPI_DEVICEINFO;

typedef struct
{
    DWORD initflags;
    DWORD freq;
    DWORD chans;
    DWORD format;
    DWORD buflen;
    float volmax;
    float volmin;
    float volstep;
} BASS_WASAPI_INFO;

// BASS_WASAPI_DEVICEINFO "type"
#define BASS_WASAPI_TYPE_NETWORKDEVICE 0
#define BASS_WASAPI_TYPE_SPEAKERS 1
#define BASS_WASAPI_TYPE_LINELEVEL 2
#define BASS_WASAPI_TYPE_HEADPHONES 3
#define BASS_WASAPI_TYPE_MICROPHONE 4
#define BASS_WASAPI_TYPE_HEADSET 5
#define BASS_WASAPI_TYPE_HANDSET 6
#define BASS_WASAPI_TYPE_DIGITAL 7
#define BASS_WASAPI_TYPE_SPDIF 8
#define BASS_WASAPI_TYPE_HDMI 9
#define BASS_WASAPI_TYPE_UNKNOWN 10

// BASS_WASAPI_DEVICEINFO flags
#define BASS_DEVICE_ENABLED 1
#define BASS_DEVICE_DEFAULT 2
#define BASS_DEVICE_INIT 4
#define BASS_DEVICE_LOOPBACK 8
#define BASS_DEVICE_INPUT 16
#define BASS_DEVICE_UNPLUGGED 32
#define BASS_DEVICE_DISABLED 64

// BASS_WASAPI_Init flags
#define BASS_WASAPI_EXCLUSIVE 1
#define BASS_WASAPI_AUTOFORMAT 2
#define BASS_WASAPI_BUFFER 4
#define BASS_WASAPI_EVENT 16
#define BASS_WASAPI_SAMPLES 32
#define BASS_WASAPI_DITHER 64
#define BASS_WASAPI_RAW 128
#define BASS_WASAPI_ASYNC 0x100

#define BASS_WASAPI_CATEGORY_MASK 0xf000
#define BASS_WASAPI_CATEGORY_OTHER 0x0000
#define BASS_WASAPI_CATEGORY_FOREGROUNDONLYMEDIA 0x1000
#define BASS_WASAPI_CATEGORY_BACKGROUNDCAPABLEMEDIA 0x2000
#define BASS_WASAPI_CATEGORY_COMMUNICATIONS 0x3000
#define BASS_WASAPI_CATEGORY_ALERTS 0x4000
#define BASS_WASAPI_CATEGORY_SOUNDEFFECTS 0x5000
#define BASS_WASAPI_CATEGORY_GAMEEFFECTS 0x6000
#define BASS_WASAPI_CATEGORY_GAMEMEDIA 0x7000
#define BASS_WASAPI_CATEGORY_GAMECHAT 0x8000
#define BASS_WASAPI_CATEGORY_SPEECH 0x9000
#define BASS_WASAPI_CATEGORY_MOVIE 0xa000
#define BASS_WASAPI_CATEGORY_MEDIA 0xb000

// BASS_WASAPI_INFO "format"
#define BASS_WASAPI_FORMAT_FLOAT 0
#define BASS_WASAPI_FORMAT_8BIT 1
#define BASS_WASAPI_FORMAT_16BIT 2
#define BASS_WASAPI_FORMAT_24BIT 3
#define BASS_WASAPI_FORMAT_32BIT 4

// BASS_WASAPI_Set/GetVolume modes
#define BASS_WASAPI_CURVE_DB 0
#define BASS_WASAPI_CURVE_LINEAR 1
#define BASS_WASAPI_CURVE_WINDOWS 2
#define BASS_WASAPI_VOL_SESSION 8

    typedef DWORD(CALLBACK WASAPIPROC)(void *buffer, DWORD length, void *user);
/* WASAPI callback function.
buffer : Buffer containing the sample data
length : Number of bytes
user   : The 'user' parameter given when calling BASS_WASAPI_Init
RETURN : The number of bytes written (output devices), 0/1 = stop/continue (input devices) */

// Special WASAPIPROCs
#define WASAPIPROC_PUSH (WASAPIPROC *)0  // push output
#define WASAPIPROC_BASS (WASAPIPROC *)-1 // BASS channel

    typedef void(CALLBACK WASAPINOTIFYPROC)(DWORD notify, DWORD device, void *user);
/* WASAPI device notification callback function.
notify : The notification (BASS_WASAPI_NOTIFY_xxx)
device : Device that the notification applies to
user   : The 'user' parameter given when calling BASS_WASAPI_SetNotify */

// Device notifications
#define BASS_WASAPI_NOTIFY_ENABLED 0
#define BASS_WASAPI_NOTIFY_DISABLED 1
#define BASS_WASAPI_NOTIFY_DEFOUTPUT 2
#define BASS_WASAPI_NOTIFY_DEFINPUT 3
#define BASS_WASAPI_NOTIFY_FAIL 0x100

#ifdef __cplusplus
}
#endif

#endif // BASSWASAPI_H

// bassasio.h ------------------------------------------------------------------

#ifndef BASSASIO_H
#define BASSASIO_H

#include <wtypes.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BASSASIODEF
#define BASSASIODEF(f) WINAPI f
#endif

#define BASSASIOVERSION 0x104 // API version

// BASS_ASIO_Init flags
#define BASS_ASIO_THREAD 1    // host driver in dedicated thread
#define BASS_ASIO_JOINORDER 2 // order joined channels by when they were joined

// device info structure
typedef struct
{
    const char *name;   // description
    const char *driver; // driver
} BASS_ASIO_DEVICEINFO;

typedef struct
{
    char name[32];   // driver name
    DWORD version;   // driver version
    DWORD inputs;    // number of inputs
    DWORD outputs;   // number of outputs
    DWORD bufmin;    // minimum buffer length
    DWORD bufmax;    // maximum buffer length
    DWORD bufpref;   // preferred/default buffer length
    int bufgran;     // buffer length granularity
    DWORD initflags; // BASS_ASIO_Init "flags" parameter
} BASS_ASIO_INFO;

typedef struct
{
    DWORD group;
    DWORD format;  // sample format (BASS_ASIO_FORMAT_xxx)
    char name[32]; // channel name
} BASS_ASIO_CHANNELINFO;

// sample formats
#define BASS_ASIO_FORMAT_16BIT 16     // 16-bit integer
#define BASS_ASIO_FORMAT_24BIT 17     // 24-bit integer
#define BASS_ASIO_FORMAT_32BIT 18     // 32-bit integer
#define BASS_ASIO_FORMAT_FLOAT 19     // 32-bit floating-point
#define BASS_ASIO_FORMAT_32BIT16 24   // 32-bit integer with 16-bit alignment
#define BASS_ASIO_FORMAT_32BIT18 25   // 32-bit integer with 18-bit alignment
#define BASS_ASIO_FORMAT_32BIT20 26   // 32-bit integer with 20-bit alignment
#define BASS_ASIO_FORMAT_32BIT24 27   // 32-bit integer with 24-bit alignment
#define BASS_ASIO_FORMAT_DSD_LSB 32   // DSD (LSB 1st)
#define BASS_ASIO_FORMAT_DSD_MSB 33   // DSD (MSB 1st)
#define BASS_ASIO_FORMAT_DITHER 0x100 // flag: apply dither when converting from floating-point to integer

// BASS_ASIO_ChannelReset flags
#define BASS_ASIO_RESET_ENABLE 1       // disable channel
#define BASS_ASIO_RESET_JOIN 2         // unjoin channel
#define BASS_ASIO_RESET_PAUSE 4        // unpause channel
#define BASS_ASIO_RESET_FORMAT 8       // reset sample format to native format
#define BASS_ASIO_RESET_RATE 16        // reset sample rate to device rate
#define BASS_ASIO_RESET_VOLUME 32      // reset volume to 1.0
#define BASS_ASIO_RESET_JOINED 0x10000 // apply to joined channels too

// BASS_ASIO_ChannelIsActive return values
#define BASS_ASIO_ACTIVE_DISABLED 0
#define BASS_ASIO_ACTIVE_ENABLED 1
#define BASS_ASIO_ACTIVE_PAUSED 2

typedef DWORD(CALLBACK ASIOPROC)(BOOL input, DWORD channel, void *buffer, DWORD length, void *user);
/* ASIO channel callback function.
input  : Input? else output
channel: Channel number
buffer : Buffer containing the sample data
length : Number of bytes
user   : The 'user' parameter given when calling BASS_ASIO_ChannelEnable
RETURN : The number of bytes written (ignored with input channels) */

typedef void(CALLBACK ASIONOTIFYPROC)(DWORD notify, void *user);
/* Driver notification callback function.
notify : The notification (BASS_ASIO_NOTIFY_xxx)
user   : The 'user' parameter given when calling BASS_ASIO_SetNotify */

// driver notifications
#define BASS_ASIO_NOTIFY_RATE 1  // sample rate change
#define BASS_ASIO_NOTIFY_RESET 2 // reset (reinitialization) request

// BASS_ASIO_ChannelGetLevel flags
#define BASS_ASIO_LEVEL_RMS 0x1000000

#ifdef __cplusplus
}
#endif

#endif

// --------------------------------------------------------------------------

class BassDll
{
private:
    BassDll() {}
    ~BassDll() { Free(); }
    BassDll(const BassDll &) = delete;
    BassDll(BassDll &&) = delete;
    BassDll &operator=(const BassDll &) = delete;
    BassDll &operator=(BassDll &&) = delete;
    HMODULE m_hmodule = NULL;
    bool m_isLoad;

public:
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SetConfig)(DWORD option, DWORD value);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_GetConfig)(DWORD option);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SetConfigPtr)(DWORD option, const void *value);
    typedef const void *PIV_BASS_FUNC_DEF(BASS_GetConfigPtr)(DWORD option);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_GetVersion)(void);
    typedef int PIV_BASS_FUNC_DEF(BASS_ErrorGetCode)(void);

    typedef BOOL PIV_BASS_FUNC_DEF(BASS_GetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Init)(int device, DWORD freq, DWORD flags, HWND win, const void *dsguid);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Free)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SetDevice)(DWORD device);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_GetDevice)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_GetInfo)(BASS_INFO *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Start)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Stop)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Pause)(void);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_IsStarted)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Update)(DWORD length);
    typedef float PIV_BASS_FUNC_DEF(BASS_GetCPU)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SetVolume)(float volume);
    typedef float PIV_BASS_FUNC_DEF(BASS_GetVolume)(void);
    typedef void *PIV_BASS_FUNC_DEF(BASS_GetDSoundObject)(DWORD object);

    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Set3DFactors)(float distf, float rollf, float doppf);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Get3DFactors)(float *distf, float *rollf, float *doppf);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Set3DPosition)(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Get3DPosition)(BASS_3DVECTOR *pos, BASS_3DVECTOR *vel, BASS_3DVECTOR *front, BASS_3DVECTOR *top);
    typedef void PIV_BASS_FUNC_DEF(BASS_Apply3D)(void);

    typedef HPLUGIN PIV_BASS_FUNC_DEF(BASS_PluginLoad)(const WCHAR *file, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_PluginFree)(HPLUGIN handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_PluginEnable)(HPLUGIN handle, BOOL enable);
    typedef const BASS_PLUGININFO *PIV_BASS_FUNC_DEF(BASS_PluginGetInfo)(HPLUGIN handle);

    typedef HSAMPLE PIV_BASS_FUNC_DEF(BASS_SampleLoad)(BOOL mem, const void *file, QWORD offset, DWORD length, DWORD max, DWORD flags);
    typedef HSAMPLE PIV_BASS_FUNC_DEF(BASS_SampleCreate)(DWORD length, DWORD freq, DWORD chans, DWORD max, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleFree)(HSAMPLE handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleSetData)(HSAMPLE handle, const void *buffer);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleGetData)(HSAMPLE handle, void *buffer);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleGetInfo)(HSAMPLE handle, BASS_SAMPLE *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleSetInfo)(HSAMPLE handle, const BASS_SAMPLE *info);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_SampleGetChannel)(HSAMPLE handle, DWORD flags);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_SampleGetChannels)(HSAMPLE handle, HCHANNEL *channels);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_SampleStop)(HSAMPLE handle);

    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, STREAMPROC *proc, void *user);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_StreamCreateURL)(const WCHAR *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_StreamFree)(HSTREAM handle);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_StreamGetFilePosition)(HSTREAM handle, DWORD mode);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_StreamPutData)(HSTREAM handle, const void *buffer, DWORD length);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_StreamPutFileData)(HSTREAM handle, const void *buffer, DWORD length);

    typedef HMUSIC PIV_BASS_FUNC_DEF(BASS_MusicLoad)(BOOL mem, const WCHAR *file, QWORD offset, DWORD length, DWORD flags, DWORD freq);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_MusicFree)(HMUSIC handle);

    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordGetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordInit)(int device);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordFree)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordSetDevice)(DWORD device);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_RecordGetDevice)(void);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordGetInfo)(BASS_RECORDINFO *info);
    typedef const char *PIV_BASS_FUNC_DEF(BASS_RecordGetInputName)(int input);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_RecordSetInput)(int input, DWORD flags, float volume);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_RecordGetInput)(int input, float *volume);
    typedef HRECORD PIV_BASS_FUNC_DEF(BASS_RecordStart)(DWORD freq, DWORD chans, DWORD flags, RECORDPROC *proc, void *user);

    typedef double PIV_BASS_FUNC_DEF(BASS_ChannelBytes2Seconds)(DWORD handle, QWORD pos);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_ChannelSeconds2Bytes)(DWORD handle, double pos);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetDevice)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSetDevice)(DWORD handle, DWORD device);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelIsActive)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelGetInfo)(DWORD handle, BASS_CHANNELINFO *info);
    typedef const char *PIV_BASS_FUNC_DEF(BASS_ChannelGetTags)(DWORD handle, DWORD tags);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelLock)(DWORD handle, BOOL lock);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelFree)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelPlay)(DWORD handle, BOOL restart);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelStart)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelStop)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelPause)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelUpdate)(DWORD handle, DWORD length);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSetAttribute)(DWORD handle, DWORD attrib, float value);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelGetAttribute)(DWORD handle, DWORD attrib, float *value);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSlideAttribute)(DWORD handle, DWORD attrib, float value, DWORD time);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelIsSliding)(DWORD handle, DWORD attrib);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSetAttributeEx)(DWORD handle, DWORD attrib, void *value, DWORD size);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetAttributeEx)(DWORD handle, DWORD attrib, void *value, DWORD size);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSet3DAttributes)(DWORD handle, int mode, float min, float max, int iangle, int oangle, float outvol);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelGet3DAttributes)(DWORD handle, DWORD *mode, float *min, float *max, DWORD *iangle, DWORD *oangle, float *outvol);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSet3DPosition)(DWORD handle, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelGet3DPosition)(DWORD handle, BASS_3DVECTOR *pos, BASS_3DVECTOR *orient, BASS_3DVECTOR *vel);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetLength)(DWORD handle, DWORD mode);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetPosition)(DWORD handle, DWORD mode);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetLevel)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelGetLevelEx)(DWORD handle, float *levels, float length, DWORD flags);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
    typedef HSYNC PIV_BASS_FUNC_DEF(BASS_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelRemoveSync)(DWORD handle, HSYNC sync);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelSetLink)(DWORD handle, DWORD chan);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelRemoveLink)(DWORD handle, DWORD chan);
    typedef HDSP PIV_BASS_FUNC_DEF(BASS_ChannelSetDSP)(DWORD handle, DSPPROC *proc, void *user, int priority);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelRemoveDSP)(DWORD handle, HDSP dsp);
    typedef HFX PIV_BASS_FUNC_DEF(BASS_ChannelSetFX)(DWORD handle, DWORD type, int priority);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ChannelRemoveFX)(DWORD handle, HFX fx);

    typedef BOOL PIV_BASS_FUNC_DEF(BASS_FXSetParameters)(HFX handle, const void *params);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_FXGetParameters)(HFX handle, void *params);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_FXSetPriority)(HFX handle, int priority);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_FXReset)(DWORD handle);

    PIV_BASS_FUNC_VAR(BASS_SetConfig)
    PIV_BASS_FUNC_VAR(BASS_GetConfig)
    PIV_BASS_FUNC_VAR(BASS_SetConfigPtr)
    PIV_BASS_FUNC_VAR(BASS_GetConfigPtr)
    PIV_BASS_FUNC_VAR(BASS_GetVersion)
    PIV_BASS_FUNC_VAR(BASS_ErrorGetCode)
    PIV_BASS_FUNC_VAR(BASS_GetDeviceInfo)
    PIV_BASS_FUNC_VAR(BASS_Init)
    PIV_BASS_FUNC_VAR(BASS_Free)
    PIV_BASS_FUNC_VAR(BASS_SetDevice)
    PIV_BASS_FUNC_VAR(BASS_GetDevice)
    PIV_BASS_FUNC_VAR(BASS_GetInfo)
    PIV_BASS_FUNC_VAR(BASS_Start)
    PIV_BASS_FUNC_VAR(BASS_Stop)
    PIV_BASS_FUNC_VAR(BASS_Pause)
    PIV_BASS_FUNC_VAR(BASS_IsStarted)
    PIV_BASS_FUNC_VAR(BASS_Update)
    PIV_BASS_FUNC_VAR(BASS_GetCPU)
    PIV_BASS_FUNC_VAR(BASS_SetVolume)
    PIV_BASS_FUNC_VAR(BASS_GetVolume)
    PIV_BASS_FUNC_VAR(BASS_GetDSoundObject)
    PIV_BASS_FUNC_VAR(BASS_Set3DFactors)
    PIV_BASS_FUNC_VAR(BASS_Get3DFactors)
    PIV_BASS_FUNC_VAR(BASS_Set3DPosition)
    PIV_BASS_FUNC_VAR(BASS_Get3DPosition)
    PIV_BASS_FUNC_VAR(BASS_Apply3D)
    PIV_BASS_FUNC_VAR(BASS_PluginLoad)
    PIV_BASS_FUNC_VAR(BASS_PluginFree)
    PIV_BASS_FUNC_VAR(BASS_PluginEnable)
    PIV_BASS_FUNC_VAR(BASS_PluginGetInfo)
    PIV_BASS_FUNC_VAR(BASS_SampleLoad)
    PIV_BASS_FUNC_VAR(BASS_SampleCreate)
    PIV_BASS_FUNC_VAR(BASS_SampleFree)
    PIV_BASS_FUNC_VAR(BASS_SampleSetData)
    PIV_BASS_FUNC_VAR(BASS_SampleGetData)
    PIV_BASS_FUNC_VAR(BASS_SampleGetInfo)
    PIV_BASS_FUNC_VAR(BASS_SampleSetInfo)
    PIV_BASS_FUNC_VAR(BASS_SampleGetChannel)
    PIV_BASS_FUNC_VAR(BASS_SampleGetChannels)
    PIV_BASS_FUNC_VAR(BASS_SampleStop)
    PIV_BASS_FUNC_VAR(BASS_StreamCreate)
    PIV_BASS_FUNC_VAR(BASS_StreamCreateFile)
    PIV_BASS_FUNC_VAR(BASS_StreamCreateURL)
    PIV_BASS_FUNC_VAR(BASS_StreamCreateFileUser)
    PIV_BASS_FUNC_VAR(BASS_StreamFree)
    PIV_BASS_FUNC_VAR(BASS_StreamGetFilePosition)
    PIV_BASS_FUNC_VAR(BASS_StreamPutData)
    PIV_BASS_FUNC_VAR(BASS_StreamPutFileData)
    PIV_BASS_FUNC_VAR(BASS_MusicLoad)
    PIV_BASS_FUNC_VAR(BASS_MusicFree)
    PIV_BASS_FUNC_VAR(BASS_RecordGetDeviceInfo)
    PIV_BASS_FUNC_VAR(BASS_RecordInit)
    PIV_BASS_FUNC_VAR(BASS_RecordFree)
    PIV_BASS_FUNC_VAR(BASS_RecordSetDevice)
    PIV_BASS_FUNC_VAR(BASS_RecordGetDevice)
    PIV_BASS_FUNC_VAR(BASS_RecordGetInfo)
    PIV_BASS_FUNC_VAR(BASS_RecordGetInputName)
    PIV_BASS_FUNC_VAR(BASS_RecordSetInput)
    PIV_BASS_FUNC_VAR(BASS_RecordGetInput)
    PIV_BASS_FUNC_VAR(BASS_RecordStart)
    PIV_BASS_FUNC_VAR(BASS_ChannelBytes2Seconds)
    PIV_BASS_FUNC_VAR(BASS_ChannelSeconds2Bytes)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetDevice)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetDevice)
    PIV_BASS_FUNC_VAR(BASS_ChannelIsActive)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetInfo)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetTags)
    PIV_BASS_FUNC_VAR(BASS_ChannelFlags)
    PIV_BASS_FUNC_VAR(BASS_ChannelLock)
    PIV_BASS_FUNC_VAR(BASS_ChannelFree)
    PIV_BASS_FUNC_VAR(BASS_ChannelPlay)
    PIV_BASS_FUNC_VAR(BASS_ChannelStart)
    PIV_BASS_FUNC_VAR(BASS_ChannelStop)
    PIV_BASS_FUNC_VAR(BASS_ChannelPause)
    PIV_BASS_FUNC_VAR(BASS_ChannelUpdate)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetAttribute)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetAttribute)
    PIV_BASS_FUNC_VAR(BASS_ChannelSlideAttribute)
    PIV_BASS_FUNC_VAR(BASS_ChannelIsSliding)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetAttributeEx)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetAttributeEx)
    PIV_BASS_FUNC_VAR(BASS_ChannelSet3DAttributes)
    PIV_BASS_FUNC_VAR(BASS_ChannelGet3DAttributes)
    PIV_BASS_FUNC_VAR(BASS_ChannelSet3DPosition)
    PIV_BASS_FUNC_VAR(BASS_ChannelGet3DPosition)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetLength)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetPosition)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetPosition)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetLevel)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetLevelEx)
    PIV_BASS_FUNC_VAR(BASS_ChannelGetData)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetSync)
    PIV_BASS_FUNC_VAR(BASS_ChannelRemoveSync)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetLink)
    PIV_BASS_FUNC_VAR(BASS_ChannelRemoveLink)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetDSP)
    PIV_BASS_FUNC_VAR(BASS_ChannelRemoveDSP)
    PIV_BASS_FUNC_VAR(BASS_ChannelSetFX)
    PIV_BASS_FUNC_VAR(BASS_ChannelRemoveFX)
    PIV_BASS_FUNC_VAR(BASS_FXSetParameters)
    PIV_BASS_FUNC_VAR(BASS_FXGetParameters)
    PIV_BASS_FUNC_VAR(BASS_FXSetPriority)
    PIV_BASS_FUNC_VAR(BASS_FXReset)

    static BassDll &instance()
    {
        static BassDll inst;
        return inst;
    }

    static BassDll &Fn()
    {
        if (!BassDll::instance().Load())
        {
            HWND hParentWnd = NULL;
#ifdef _AFX
            CWinApp* pWinApp = AfxGetApp();
            if (pWinApp != NULL)
                hParentWnd = pWinApp->m_pMainWnd->GetSafeHwnd ();
#endif
            if (hParentWnd == NULL)
                hParentWnd = ::GetActiveWindow();
            if (hParentWnd == NULL)
                hParentWnd = ::GetDesktopWindow();
            ::MessageBoxW(hParentWnd, L"未能加载 bass.dll，即将退出应用程序!", L"错误", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TASKMODAL);
            exit(0);
        }
        return BassDll::instance();
    }

    inline BOOL Load(const wchar_t *dll = L"bass.dll")
    {
        if (m_isLoad)
            return TRUE;
        m_hmodule = ::LoadLibraryW(dll);
        if (m_hmodule == NULL)
            return FALSE;

        PIV_BASS_FUNC_GET(BASS_SetConfig)
        PIV_BASS_FUNC_GET(BASS_GetConfig)
        PIV_BASS_FUNC_GET(BASS_SetConfigPtr)
        PIV_BASS_FUNC_GET(BASS_GetConfigPtr)
        PIV_BASS_FUNC_GET(BASS_GetVersion)
        PIV_BASS_FUNC_GET(BASS_ErrorGetCode)
        PIV_BASS_FUNC_GET(BASS_GetDeviceInfo)
        PIV_BASS_FUNC_GET(BASS_Init)
        PIV_BASS_FUNC_GET(BASS_Free)
        PIV_BASS_FUNC_GET(BASS_SetDevice)
        PIV_BASS_FUNC_GET(BASS_GetDevice)
        PIV_BASS_FUNC_GET(BASS_GetInfo)
        PIV_BASS_FUNC_GET(BASS_Start)
        PIV_BASS_FUNC_GET(BASS_Stop)
        PIV_BASS_FUNC_GET(BASS_Pause)
        PIV_BASS_FUNC_GET(BASS_IsStarted)
        PIV_BASS_FUNC_GET(BASS_Update)
        PIV_BASS_FUNC_GET(BASS_GetCPU)
        PIV_BASS_FUNC_GET(BASS_SetVolume)
        PIV_BASS_FUNC_GET(BASS_GetVolume)
        PIV_BASS_FUNC_GET(BASS_GetDSoundObject)
        PIV_BASS_FUNC_GET(BASS_Set3DFactors)
        PIV_BASS_FUNC_GET(BASS_Get3DFactors)
        PIV_BASS_FUNC_GET(BASS_Set3DPosition)
        PIV_BASS_FUNC_GET(BASS_Get3DPosition)
        PIV_BASS_FUNC_GET(BASS_Apply3D)
        PIV_BASS_FUNC_GET(BASS_PluginLoad)
        PIV_BASS_FUNC_GET(BASS_PluginFree)
        PIV_BASS_FUNC_GET(BASS_PluginEnable)
        PIV_BASS_FUNC_GET(BASS_PluginGetInfo)
        PIV_BASS_FUNC_GET(BASS_SampleLoad)
        PIV_BASS_FUNC_GET(BASS_SampleCreate)
        PIV_BASS_FUNC_GET(BASS_SampleFree)
        PIV_BASS_FUNC_GET(BASS_SampleSetData)
        PIV_BASS_FUNC_GET(BASS_SampleGetData)
        PIV_BASS_FUNC_GET(BASS_SampleGetInfo)
        PIV_BASS_FUNC_GET(BASS_SampleSetInfo)
        PIV_BASS_FUNC_GET(BASS_SampleGetChannel)
        PIV_BASS_FUNC_GET(BASS_SampleGetChannels)
        PIV_BASS_FUNC_GET(BASS_SampleStop)
        PIV_BASS_FUNC_GET(BASS_StreamCreate)
        PIV_BASS_FUNC_GET(BASS_StreamCreateFile)
        PIV_BASS_FUNC_GET(BASS_StreamCreateURL)
        PIV_BASS_FUNC_GET(BASS_StreamCreateFileUser)
        PIV_BASS_FUNC_GET(BASS_StreamFree)
        PIV_BASS_FUNC_GET(BASS_StreamGetFilePosition)
        PIV_BASS_FUNC_GET(BASS_StreamPutData)
        PIV_BASS_FUNC_GET(BASS_StreamPutFileData)
        PIV_BASS_FUNC_GET(BASS_MusicLoad)
        PIV_BASS_FUNC_GET(BASS_MusicFree)
        PIV_BASS_FUNC_GET(BASS_RecordGetDeviceInfo)
        PIV_BASS_FUNC_GET(BASS_RecordInit)
        PIV_BASS_FUNC_GET(BASS_RecordFree)
        PIV_BASS_FUNC_GET(BASS_RecordSetDevice)
        PIV_BASS_FUNC_GET(BASS_RecordGetDevice)
        PIV_BASS_FUNC_GET(BASS_RecordGetInfo)
        PIV_BASS_FUNC_GET(BASS_RecordGetInputName)
        PIV_BASS_FUNC_GET(BASS_RecordSetInput)
        PIV_BASS_FUNC_GET(BASS_RecordGetInput)
        PIV_BASS_FUNC_GET(BASS_RecordStart)
        PIV_BASS_FUNC_GET(BASS_ChannelBytes2Seconds)
        PIV_BASS_FUNC_GET(BASS_ChannelSeconds2Bytes)
        PIV_BASS_FUNC_GET(BASS_ChannelGetDevice)
        PIV_BASS_FUNC_GET(BASS_ChannelSetDevice)
        PIV_BASS_FUNC_GET(BASS_ChannelIsActive)
        PIV_BASS_FUNC_GET(BASS_ChannelGetInfo)
        PIV_BASS_FUNC_GET(BASS_ChannelGetTags)
        PIV_BASS_FUNC_GET(BASS_ChannelFlags)
        PIV_BASS_FUNC_GET(BASS_ChannelLock)
        PIV_BASS_FUNC_GET(BASS_ChannelFree)
        PIV_BASS_FUNC_GET(BASS_ChannelPlay)
        PIV_BASS_FUNC_GET(BASS_ChannelStart)
        PIV_BASS_FUNC_GET(BASS_ChannelStop)
        PIV_BASS_FUNC_GET(BASS_ChannelPause)
        PIV_BASS_FUNC_GET(BASS_ChannelUpdate)
        PIV_BASS_FUNC_GET(BASS_ChannelSetAttribute)
        PIV_BASS_FUNC_GET(BASS_ChannelGetAttribute)
        PIV_BASS_FUNC_GET(BASS_ChannelSlideAttribute)
        PIV_BASS_FUNC_GET(BASS_ChannelIsSliding)
        PIV_BASS_FUNC_GET(BASS_ChannelSetAttributeEx)
        PIV_BASS_FUNC_GET(BASS_ChannelGetAttributeEx)
        PIV_BASS_FUNC_GET(BASS_ChannelSet3DAttributes)
        PIV_BASS_FUNC_GET(BASS_ChannelGet3DAttributes)
        PIV_BASS_FUNC_GET(BASS_ChannelSet3DPosition)
        PIV_BASS_FUNC_GET(BASS_ChannelGet3DPosition)
        PIV_BASS_FUNC_GET(BASS_ChannelGetLength)
        PIV_BASS_FUNC_GET(BASS_ChannelSetPosition)
        PIV_BASS_FUNC_GET(BASS_ChannelGetPosition)
        PIV_BASS_FUNC_GET(BASS_ChannelGetLevel)
        PIV_BASS_FUNC_GET(BASS_ChannelGetLevelEx)
        PIV_BASS_FUNC_GET(BASS_ChannelGetData)
        PIV_BASS_FUNC_GET(BASS_ChannelSetSync)
        PIV_BASS_FUNC_GET(BASS_ChannelRemoveSync)
        PIV_BASS_FUNC_GET(BASS_ChannelSetLink)
        PIV_BASS_FUNC_GET(BASS_ChannelRemoveLink)
        PIV_BASS_FUNC_GET(BASS_ChannelSetDSP)
        PIV_BASS_FUNC_GET(BASS_ChannelRemoveDSP)
        PIV_BASS_FUNC_GET(BASS_ChannelSetFX)
        PIV_BASS_FUNC_GET(BASS_ChannelRemoveFX)
        PIV_BASS_FUNC_GET(BASS_FXSetParameters)
        PIV_BASS_FUNC_GET(BASS_FXGetParameters)
        PIV_BASS_FUNC_GET(BASS_FXSetPriority)
        PIV_BASS_FUNC_GET(BASS_FXReset)

        m_isLoad = true;
        BASS_SetConfig(BASS_CONFIG_UNICODE, 1);
        return TRUE;
    }

    inline BOOL Free()
    {
        if (m_isLoad && m_hmodule != NULL)
            return ::FreeLibrary(m_hmodule);
        return false;
    }

    inline bool IsLoaded()
    {
        return m_isLoad;
    }

    inline HMODULE GetHandle()
    {
        return m_hmodule;
    }

}; // class BassDll

class BassMix
{
private:
    BassMix() {}
    ~BassMix() { Free(); }
    BassMix(const BassMix &) = delete;
    BassMix(BassMix &&) = delete;
    BassMix &operator=(const BassMix &) = delete;
    BassMix &operator=(BassMix &&) = delete;
    HMODULE m_hmodule = NULL;
    bool m_isLoad;

public:
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_GetVersion)(void);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_Mixer_StreamCreate)(DWORD freq, DWORD chans, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_StreamAddChannel)(HSTREAM handle, DWORD channel, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_StreamAddChannelEx)(HSTREAM handle, DWORD channel, DWORD flags, QWORD start, QWORD length);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_StreamGetChannels)(HSTREAM handle, DWORD *channels, DWORD count);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetMixer)(DWORD handle);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelIsActive)(DWORD handle);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelRemove)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetPosition)(DWORD handle, DWORD mode);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetPositionEx)(DWORD channel, DWORD mode, DWORD delay);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetLevel)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetLevelEx)(DWORD handle, float *levels, float length, DWORD flags);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
    typedef HSYNC PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelRemoveSync)(DWORD channel, HSYNC sync);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetMatrix)(DWORD handle, const void *matrix);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetMatrixEx)(DWORD handle, const void *matrix, float time);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetMatrix)(DWORD handle, void *matrix);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetEnvelope)(DWORD handle, DWORD type, const BASS_MIXER_NODE *nodes, DWORD count);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelSetEnvelopePos)(DWORD handle, DWORD type, QWORD pos);
    typedef QWORD PIV_BASS_FUNC_DEF(BASS_Mixer_ChannelGetEnvelopePos)(DWORD handle, DWORD type, float *value);
    typedef HSTREAM PIV_BASS_FUNC_DEF(BASS_Split_StreamCreate)(DWORD channel, DWORD flags, const int *chanmap);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Split_StreamGetSource)(HSTREAM handle);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Split_StreamGetSplits)(DWORD handle, HSTREAM *splits, DWORD count);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Split_StreamReset)(DWORD handle);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_Split_StreamResetEx)(DWORD handle, DWORD offset);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_Split_StreamGetAvailable)(DWORD handle);

    PIV_BASS_FUNC_VAR(BASS_Mixer_GetVersion)
    PIV_BASS_FUNC_VAR(BASS_Mixer_StreamCreate)
    PIV_BASS_FUNC_VAR(BASS_Mixer_StreamAddChannel)
    PIV_BASS_FUNC_VAR(BASS_Mixer_StreamAddChannelEx)
    PIV_BASS_FUNC_VAR(BASS_Mixer_StreamGetChannels)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetMixer)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelIsActive)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelFlags)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelRemove)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetPosition)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetPosition)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetPositionEx)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetLevel)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetLevelEx)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetData)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetSync)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelRemoveSync)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetMatrix)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetMatrixEx)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetMatrix)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetEnvelope)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelSetEnvelopePos)
    PIV_BASS_FUNC_VAR(BASS_Mixer_ChannelGetEnvelopePos)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamCreate)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamGetSource)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamGetSplits)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamReset)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamResetEx)
    PIV_BASS_FUNC_VAR(BASS_Split_StreamGetAvailable)

    static BassMix &instance()
    {
        static BassMix inst;
        return inst;
    }

    static BassMix &Fn()
    {
        if (!BassMix::instance().Load())
        {
            HWND hParentWnd = NULL;
#ifdef _AFX
            CWinApp* pWinApp = AfxGetApp();
            if (pWinApp != NULL)
                hParentWnd = pWinApp->m_pMainWnd->GetSafeHwnd ();
#endif
            if (hParentWnd == NULL)
                hParentWnd = ::GetActiveWindow();
            if (hParentWnd == NULL)
                hParentWnd = ::GetDesktopWindow();
            ::MessageBoxW(hParentWnd, L"未能加载 bassmix.dll，即将退出应用程序!", L"错误", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TASKMODAL);
            exit(0);
        }
        return BassMix::instance();
    }

    inline BOOL Load(const wchar_t *dll = L"bassmix.dll")
    {
        if (m_isLoad)
            return TRUE;
        BassDll::instance().Load();
        m_hmodule = ::LoadLibraryW(dll);
        if (m_hmodule == NULL)
            return FALSE;
        PIV_BASS_FUNC_GET(BASS_Mixer_GetVersion)
        PIV_BASS_FUNC_GET(BASS_Mixer_StreamCreate)
        PIV_BASS_FUNC_GET(BASS_Mixer_StreamAddChannel)
        PIV_BASS_FUNC_GET(BASS_Mixer_StreamAddChannelEx)
        PIV_BASS_FUNC_GET(BASS_Mixer_StreamGetChannels)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetMixer)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelIsActive)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelFlags)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelRemove)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetPosition)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetPosition)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetPositionEx)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetLevel)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetLevelEx)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetData)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetSync)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelRemoveSync)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetMatrix)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetMatrixEx)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetMatrix)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetEnvelope)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelSetEnvelopePos)
        PIV_BASS_FUNC_GET(BASS_Mixer_ChannelGetEnvelopePos)
        PIV_BASS_FUNC_GET(BASS_Split_StreamCreate)
        PIV_BASS_FUNC_GET(BASS_Split_StreamGetSource)
        PIV_BASS_FUNC_GET(BASS_Split_StreamGetSplits)
        PIV_BASS_FUNC_GET(BASS_Split_StreamReset)
        PIV_BASS_FUNC_GET(BASS_Split_StreamResetEx)
        PIV_BASS_FUNC_GET(BASS_Split_StreamGetAvailable)
        m_isLoad = true;
        return TRUE;
    }

    inline BOOL Free()
    {
        if (m_isLoad && m_hmodule != NULL)
            return ::FreeLibrary(m_hmodule);
        return false;
    }

    inline bool IsLoaded()
    {
        return m_isLoad;
    }

    inline HMODULE GetHandle()
    {
        return m_hmodule;
    }

}; // class BassMix

class BassWasApi
{
private:
    BassWasApi() {}
    ~BassWasApi() { Free(); }
    BassWasApi(const BassWasApi &) = delete;
    BassWasApi(BassWasApi &&) = delete;
    BassWasApi &operator=(const BassWasApi &) = delete;
    BassWasApi &operator=(BassWasApi &&) = delete;
    HMODULE m_hmodule = NULL;
    bool m_isLoad;

public:
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_GetVersion)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_SetNotify)(WASAPINOTIFYPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_GetDeviceInfo)(DWORD device, BASS_WASAPI_DEVICEINFO *info);
    typedef float PIV_BASS_FUNC_DEF(BASS_WASAPI_GetDeviceLevel)(DWORD device, int chan);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_SetDevice)(DWORD device);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_GetDevice)();
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_CheckFormat)(int device, DWORD freq, DWORD chans, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_Init)(int device, DWORD freq, DWORD chans, DWORD flags, float buffer, float period, WASAPIPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_Free)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_GetInfo)(BASS_WASAPI_INFO *info);
    typedef float PIV_BASS_FUNC_DEF(BASS_WASAPI_GetCPU)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_Lock)(BOOL lock);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_Start)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_Stop)(BOOL reset);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_IsStarted)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_SetVolume)(DWORD mode, float volume);
    typedef float PIV_BASS_FUNC_DEF(BASS_WASAPI_GetVolume)(DWORD mode);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_SetMute)(DWORD mode, BOOL mute);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_GetMute)(DWORD mode);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_PutData)(void *buffer, DWORD length);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_GetData)(void *buffer, DWORD length);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_WASAPI_GetLevel)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_WASAPI_GetLevelEx)(float *levels, float length, DWORD flags);

    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetVersion)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_SetNotify)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetDeviceInfo)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetDeviceLevel)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_SetDevice)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetDevice)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_CheckFormat)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_Init)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_Free)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetInfo)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetCPU)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_Lock)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_Start)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_Stop)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_IsStarted)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_SetVolume)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetVolume)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_SetMute)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetMute)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_PutData)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetData)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetLevel)
    PIV_BASS_FUNC_VAR(BASS_WASAPI_GetLevelEx)

    static BassWasApi &instance()
    {
        static BassWasApi inst;
        return inst;
    }

    static BassWasApi &Fn()
    {
        if (!BassWasApi::instance().Load())
        {
            HWND hParentWnd = NULL;
#ifdef _AFX
            CWinApp* pWinApp = AfxGetApp();
            if (pWinApp != NULL)
                hParentWnd = pWinApp->m_pMainWnd->GetSafeHwnd ();
#endif
            if (hParentWnd == NULL)
                hParentWnd = ::GetActiveWindow();
            if (hParentWnd == NULL)
                hParentWnd = ::GetDesktopWindow();
            ::MessageBoxW(hParentWnd, L"未能加载 basswasapi.dll，即将退出应用程序!", L"错误", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TASKMODAL);
            exit(0);
        }
        return BassWasApi::instance();
    }

    inline BOOL Load(const wchar_t *dll = L"basswasapi.dll")
    {
        if (m_isLoad)
            return TRUE;
        if (!BassDll::instance().Load())
            return FALSE;
        m_hmodule = ::LoadLibraryW(dll);
        if (m_hmodule == NULL)
            return FALSE;
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetVersion)
        PIV_BASS_FUNC_GET(BASS_WASAPI_SetNotify)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetDeviceInfo)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetDeviceLevel)
        PIV_BASS_FUNC_GET(BASS_WASAPI_SetDevice)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetDevice)
        PIV_BASS_FUNC_GET(BASS_WASAPI_CheckFormat)
        PIV_BASS_FUNC_GET(BASS_WASAPI_Init)
        PIV_BASS_FUNC_GET(BASS_WASAPI_Free)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetInfo)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetCPU)
        PIV_BASS_FUNC_GET(BASS_WASAPI_Lock)
        PIV_BASS_FUNC_GET(BASS_WASAPI_Start)
        PIV_BASS_FUNC_GET(BASS_WASAPI_Stop)
        PIV_BASS_FUNC_GET(BASS_WASAPI_IsStarted)
        PIV_BASS_FUNC_GET(BASS_WASAPI_SetVolume)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetVolume)
        PIV_BASS_FUNC_GET(BASS_WASAPI_SetMute)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetMute)
        PIV_BASS_FUNC_GET(BASS_WASAPI_PutData)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetData)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetLevel)
        PIV_BASS_FUNC_GET(BASS_WASAPI_GetLevelEx)
        m_isLoad = true;
        return TRUE;
    }

    inline BOOL Free()
    {
        if (m_isLoad && m_hmodule != NULL)
            return ::FreeLibrary(m_hmodule);
        return false;
    }

    inline bool IsLoaded()
    {
        return m_isLoad;
    }

    inline HMODULE GetHandle()
    {
        return m_hmodule;
    }

}; // class BassWasApi

class BassTags
{
private:
    BassTags() {}
    ~BassTags() { Free(); }
    BassTags(const BassTags &) = delete;
    BassTags(BassTags &&) = delete;
    BassTags &operator=(const BassTags &) = delete;
    BassTags &operator=(BassTags &&) = delete;
    HMODULE m_hmodule = NULL;
    bool m_isLoad;

public:
    typedef DWORD PIV_BASS_FUNC_DEF(TAGS_GetVersion)();
    typedef BOOL PIV_BASS_FUNC_DEF(TAGS_SetUTF8)(BOOL enable);
    typedef const char *PIV_BASS_FUNC_DEF(TAGS_Read)(DWORD dwHandle, const char *fmt);
    typedef const char *PIV_BASS_FUNC_DEF(TAGS_ReadEx)(DWORD dwHandle, const char *fmt, DWORD tagtype, int codepage);
    typedef const char *PIV_BASS_FUNC_DEF(TAGS_GetLastErrorDesc)();

    PIV_BASS_FUNC_VAR(TAGS_GetVersion)
    PIV_BASS_FUNC_VAR(TAGS_SetUTF8)
    PIV_BASS_FUNC_VAR(TAGS_Read)
    PIV_BASS_FUNC_VAR(TAGS_ReadEx)
    PIV_BASS_FUNC_VAR(TAGS_GetLastErrorDesc)

    static BassTags &instance()
    {
        static BassTags inst;
        return inst;
    }

    static BassTags &Fn()
    {
        if (!BassTags::instance().Load())
        {
            HWND hParentWnd = NULL;
#ifdef _AFX
            CWinApp* pWinApp = AfxGetApp();
            if (pWinApp != NULL)
                hParentWnd = pWinApp->m_pMainWnd->GetSafeHwnd ();
#endif
            if (hParentWnd == NULL)
                hParentWnd = ::GetActiveWindow();
            if (hParentWnd == NULL)
                hParentWnd = ::GetDesktopWindow();
            ::MessageBoxW(hParentWnd, L"未能加载 tags.dll，即将退出应用程序!", L"错误", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TASKMODAL);
            exit(0);
        }
        return BassTags::instance();
    }

    inline BOOL Load(const wchar_t *dll = L"tags.dll")
    {
        if (m_isLoad)
            return TRUE;
        if (!BassDll::instance().Load())
            return FALSE;
        m_hmodule = ::LoadLibraryW(dll);
        if (m_hmodule == NULL)
            return FALSE;
        PIV_BASS_FUNC_GET(TAGS_GetVersion)
        PIV_BASS_FUNC_GET(TAGS_SetUTF8)
        PIV_BASS_FUNC_GET(TAGS_Read)
        PIV_BASS_FUNC_GET(TAGS_ReadEx)
        PIV_BASS_FUNC_GET(TAGS_GetLastErrorDesc)
        m_isLoad = true;
        TAGS_SetUTF8(TRUE);
        return TRUE;
    }

    inline BOOL Free()
    {
        if (m_isLoad && m_hmodule != NULL)
            return ::FreeLibrary(m_hmodule);
        return false;
    }

    inline bool IsLoaded()
    {
        return m_isLoad;
    }

    inline HMODULE GetHandle()
    {
        return m_hmodule;
    }

}; // class BassTags

class BassAsio
{
private:
    BassAsio() {}
    ~BassAsio() { Free(); }
    BassAsio(const BassAsio &) = delete;
    BassAsio(BassAsio &&) = delete;
    BassAsio &operator=(const BassAsio &) = delete;
    BassAsio &operator=(BassAsio &&) = delete;
    HMODULE m_hmodule = NULL;
    bool m_isLoad;

public:
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_GetVersion)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_SetUnicode)(BOOL unicode);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_ErrorGetCode)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_GetDeviceInfo)(DWORD device, BASS_ASIO_DEVICEINFO *info);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_AddDevice)(const GUID *clsid, const char *driver, const char *name);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_SetDevice)(DWORD device);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_GetDevice)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Init)(int device, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Free)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Lock)(BOOL lock);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_SetNotify)(ASIONOTIFYPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ControlPanel)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_GetInfo)(BASS_ASIO_INFO *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_CheckRate)(double rate);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_SetRate)(double rate);
    typedef double PIV_BASS_FUNC_DEF(BASS_ASIO_GetRate)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Start)(DWORD buflen, DWORD threads);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Stop)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_IsStarted)();
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_GetLatency)(BOOL input);
    typedef float PIV_BASS_FUNC_DEF(BASS_ASIO_GetCPU)();
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Monitor)(int input, DWORD output, DWORD gain, DWORD state, DWORD pan);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_SetDSD)(BOOL dsd);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_Future)(DWORD selector, void *param);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelGetInfo)(BOOL input, DWORD channel, BASS_ASIO_CHANNELINFO *info);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelReset)(BOOL input, int channel, DWORD flags);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelEnable)(BOOL input, DWORD channel, ASIOPROC *proc, void *user);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelEnableMirror)(DWORD channel, BOOL input2, DWORD channel2);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelEnableBASS)(BOOL input, DWORD channel, DWORD handle, BOOL join);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelJoin)(BOOL input, DWORD channel, int channel2);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelPause)(BOOL input, DWORD channel);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelIsActive)(BOOL input, DWORD channel);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelSetFormat)(BOOL input, DWORD channel, DWORD format);
    typedef DWORD PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelGetFormat)(BOOL input, DWORD channel);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelSetRate)(BOOL input, DWORD channel, double rate);
    typedef double PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelGetRate)(BOOL input, DWORD channel);
    typedef BOOL PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelSetVolume)(BOOL input, int channel, float volume);
    typedef float PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelGetVolume)(BOOL input, int channel);
    typedef float PIV_BASS_FUNC_DEF(BASS_ASIO_ChannelGetLevel)(BOOL input, DWORD channel);

    PIV_BASS_FUNC_VAR(BASS_ASIO_GetVersion)
    PIV_BASS_FUNC_VAR(BASS_ASIO_SetUnicode)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ErrorGetCode)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetDeviceInfo)
    PIV_BASS_FUNC_VAR(BASS_ASIO_AddDevice)
    PIV_BASS_FUNC_VAR(BASS_ASIO_SetDevice)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetDevice)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Init)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Free)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Lock)
    PIV_BASS_FUNC_VAR(BASS_ASIO_SetNotify)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ControlPanel)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetInfo)
    PIV_BASS_FUNC_VAR(BASS_ASIO_CheckRate)
    PIV_BASS_FUNC_VAR(BASS_ASIO_SetRate)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetRate)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Start)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Stop)
    PIV_BASS_FUNC_VAR(BASS_ASIO_IsStarted)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetLatency)
    PIV_BASS_FUNC_VAR(BASS_ASIO_GetCPU)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Monitor)
    PIV_BASS_FUNC_VAR(BASS_ASIO_SetDSD)
    PIV_BASS_FUNC_VAR(BASS_ASIO_Future)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelGetInfo)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelReset)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelEnable)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelEnableMirror)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelEnableBASS)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelJoin)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelPause)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelIsActive)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelSetFormat)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelGetFormat)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelSetRate)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelGetRate)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelSetVolume)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelGetVolume)
    PIV_BASS_FUNC_VAR(BASS_ASIO_ChannelGetLevel)

    static BassAsio &instance()
    {
        static BassAsio inst;
        return inst;
    }

    static BassAsio &Fn()
    {
        if (!BassAsio::instance().Load())
        {
            HWND hParentWnd = NULL;
#ifdef _AFX
            CWinApp* pWinApp = AfxGetApp();
            if (pWinApp != NULL)
                hParentWnd = pWinApp->m_pMainWnd->GetSafeHwnd ();
#endif
            if (hParentWnd == NULL)
                hParentWnd = ::GetActiveWindow();
            if (hParentWnd == NULL)
                hParentWnd = ::GetDesktopWindow();
            ::MessageBoxW(hParentWnd, L"未能加载 bassasio.dll，即将退出应用程序!", L"错误", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_TASKMODAL);
            exit(0);
        }
        return BassAsio::instance();
    }

    inline BOOL Load(const wchar_t *dll = L"bassasio.dll")
    {
        if (m_isLoad)
            return TRUE;
        if (!BassDll::instance().Load())
            return FALSE;
        m_hmodule = ::LoadLibraryW(dll);
        if (m_hmodule == NULL)
            return FALSE;

        PIV_BASS_FUNC_GET(BASS_ASIO_GetVersion)
        PIV_BASS_FUNC_GET(BASS_ASIO_SetUnicode)
        PIV_BASS_FUNC_GET(BASS_ASIO_ErrorGetCode)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetDeviceInfo)
        PIV_BASS_FUNC_GET(BASS_ASIO_AddDevice)
        PIV_BASS_FUNC_GET(BASS_ASIO_SetDevice)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetDevice)
        PIV_BASS_FUNC_GET(BASS_ASIO_Init)
        PIV_BASS_FUNC_GET(BASS_ASIO_Free)
        PIV_BASS_FUNC_GET(BASS_ASIO_Lock)
        PIV_BASS_FUNC_GET(BASS_ASIO_SetNotify)
        PIV_BASS_FUNC_GET(BASS_ASIO_ControlPanel)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetInfo)
        PIV_BASS_FUNC_GET(BASS_ASIO_CheckRate)
        PIV_BASS_FUNC_GET(BASS_ASIO_SetRate)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetRate)
        PIV_BASS_FUNC_GET(BASS_ASIO_Start)
        PIV_BASS_FUNC_GET(BASS_ASIO_Stop)
        PIV_BASS_FUNC_GET(BASS_ASIO_IsStarted)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetLatency)
        PIV_BASS_FUNC_GET(BASS_ASIO_GetCPU)
        PIV_BASS_FUNC_GET(BASS_ASIO_Monitor)
        PIV_BASS_FUNC_GET(BASS_ASIO_SetDSD)
        PIV_BASS_FUNC_GET(BASS_ASIO_Future)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelGetInfo)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelReset)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelEnable)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelEnableMirror)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelEnableBASS)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelJoin)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelPause)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelIsActive)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelSetFormat)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelGetFormat)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelSetRate)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelGetRate)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelSetVolume)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelGetVolume)
        PIV_BASS_FUNC_GET(BASS_ASIO_ChannelGetLevel)

        m_isLoad = true;
        BASS_ASIO_SetUnicode(TRUE);
        return TRUE;
    }

    inline BOOL Free()
    {
        if (m_isLoad && m_hmodule != NULL)
            return ::FreeLibrary(m_hmodule);
        return false;
    }

    inline bool IsLoaded()
    {
        return m_isLoad;
    }

    inline HMODULE GetHandle()
    {
        return m_hmodule;
    }

}; // class BassAsio

#endif // _PIV_BASS_HPP

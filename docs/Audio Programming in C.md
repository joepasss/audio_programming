## AUDIO PROGRAMMING IN C

In order to play such a soundfile, we needed to tell the program, not only the sample rate, sample type and channel count; but also the endianness of the samples themselves.

The purpose of a dedicated soundfile format such as WAVE and AIFF is to encapsulate this information in a header section prefixed to the audio data itself, which an application can read to determine the format of the file. Such a file format can thus be described as "self-describing."

### 2.1 A Simple Soundfile Library: portsf

### 2.1.1 The Portsf Header Files--Public and Private Files

The `portsf` soundfile library, was designed to be as simple as possible to use It is not intended in any way to be an alternative to "industial-sterngth" libraries such as `libsndfile`. It comprises only two source files, each with an associated header file:

* ieee80.c
* ieee80.h
* portsf.c
* portsf.h

This relatively small size reflects the fact that only the two most common sondfile formats are supported, (AIFF and WAVE) though this includes 24-bit and 32-bit formats (both integer and float), and multi-channel files, including the new `WAVE_FORMAT_EXTENSIBLE` format to support srround sound and high-precision audio (e.g. 24-bit and beyond)

Libraries such as `libsndfile` supports a very wide range of formats (including compressed formats such as ADPCM), and are highly efficient, but are also very much larger,

``` c
// HOW TO USE portsf library

#include <portsf.h>
```

### 2.1.2 The Makefile

Makefile 작성 -> make 로 build

### 2.1.3 Soundfile Formats--`enum` and `typedef`

``` c
// possible speaker formats supported by portsf

typedef enum { STDWAVE, MC_STD, MC_MONO, MC_STEREO, MC_QUAD,
               MC_LCRS, MC_BFMT, MC_DOLBY_5_1, MC_WAVE_EX } 
    psf_channelformat;
```

``` c
// available file formats and the all-important sample type

typedef enum {
    PSF_FMT_UNKNOWN = 0,
    PSF_STDWAVE,
    PSF_WAVE_EX,
    PSF_AIFF,
    PSF_AIFC
} psf_format;

typedef enum {
    PSF_SAMP_UNKNOWN = 0,
    PSF_SAMP_8,
    PSF_SAMP_16,
    PSF_SAMP_24,
    PSF_SAMP_32,
    PSF_SAMP_IEEE_FLOAT
} psf_stype;

```

``` c
// .aif, aiff or newer AIFF-C format

typedef struct psf_props {
    long srate;
    long chans;
    psf_stype samptype;
    psf_format format;
    psf_channelformat chformat;
} PSF_PROPS;

```

* A soundfile contains audio data at a particular `sample rate`
    - 22050, 44100, 48000, 96000, 192000 ...
    - store this parameter in `PSF_PROPS` structure element: srate

* A soundfile can contain several channels of audio
    - one for mono, two for stereo, six for Dolby 5.1 ...
    - In multi-channel file, the sample rate defines the `frame rate` where one frame contains one sample for each channel
    - This is represented by the following structure element: `PSF_PROPS` element: chans

* A soundfile can contain audio samples in a range of data formats
    - 16-bit (C `short` data type)
    - 24-bit ("packed", stored in three bytes)
    - 32-bit (C `float` data type)
    - Less common, but possible 32-bit integer format, represented by C `long` (and usually `int`)
    - 64-bit floating-point sample
    - `WAVEFORMATEEXTENSIBLE`
        - `PSP_PROPS` element: `samptype`

### 2.1.4 Initializing the `portsf` Library

``` c
// portsf includes an initialization function

int psf_init(void)

// ...

// on end of program
int psf_finish(void)

// This colses any soundfiles that have been left open and returns the library to the same clean state
// In CLI programms, operating system will close any open files anyway when program terminates
// but in GUI-based programs that can in effect restart multiple times, these functions will ensure that the library is always in statble state.

// both functions return the value 0 to indicate success

```

### 2.1.5 Basic Soundfile Handling in `portsf`--the `switch...case` Keywords

**Opening a Soundfile for Reading**

``` c

int psf_sndOpen(const char *path, PSF_PROPS *props, int rescale);

```

``` c
PSF_PROPS props;
int sf;

sf = psf_sndOpen("sample.wav", &props, 0);

if (sf < 0) {
    printf("Error: unable to open soundfile\n");
    return 1;
}

printf("Sample rate = %d\n", props.srate);
printf("number of channels = %d\n", props.chans);

```

**Opening a Soundfile for Writing**

``` c
int psf_sndCreate(const char *path, const PSF_PROPS *props,
                int clip_floats, int minheader, int mode);

/* int clip_floats
*   used to set the way in which floating-point data is written to the file
*   the unique aspect of floating-point soundfiles is that the samples can contain over-range values.
*/

/* int minheader
*   It is an unfortunate fact of life that many applications fail to deal with WAVE formats that contain optional chunks before the audio data
*   many older UNIX_originated programs suffer from this
*   By setting minheader to 1, the soundfile is created with a "minimun header" containing just required format and data chunks
*   this means that no PEAK data will be written to the file.
*/

/* int mode
*   provided to offer some control over read-write access to the file.
*   the possible modes are defined by the custom enum type psf_create_mode
*/

// second two are speculative at present!
typedef enum {PSF_CREATE_RDWR, PSF_CREATE_TEMPORARY, PSF_CREATE_WRONLY} ps_create_mode;

```
To create a soundfile, we need as for `psf_sndOpen()`, to define a `PSF_PROPS` variable,with the essential difference that this time we have to fill in the format properties explicity

``` c
int ofd;
PSF_PROPS props;

/* define a hi-res 5.1 surround WAVE-EX file, with PEAK chunk support */

props.srate = 96000;
props.chans = 6;
props.smptype = PSF_SAMP_24;
props.format = PSF_WAVE_EX;
props.chformat = MC_DOLBY_5_1;

ofd = psf_sndCreate("soundtrack.wav", &props,1,0, PSF_CREATE_RDWR);

if (ofd < 0) {
    printf("ERROR: unable to create output file\n");
    return 1;
}
```

**Setting a File format from the Name**

A common requirement among users who have to move soundfiles between platforms (e.g. from Mac to a PC) is to convert from one format to another for example, from WAVE to AIFF.

But who does the user indicate this? In command-line applications the simplest and best method is for the user to specify the format by using a filename with the appropriate file extension; `.wav`, `.aiff`, `.aifc`, etc. With this approach, there  is no danger of a user specifying 

The `portsf` API supports this system by means of a function that returns the file formaat from the given filename:

``` c
psf_format format;
format = psf_getFormatExt("soundtrack.wav")
```

This returns a value of type `psf_format`, which can therefore be assigned directly to the appropriate element of the `PSF_PROPS` structure:

```c
props.format = format;
```

Note that this function will return the value `PSF_FMT_UNKNOWN` if an unsupported file extension is used

**Closing Soundfiles (and Recording Peak Sample Values with the PEAK Chunk)**

Having opened a soundfile, we will have to close it again at some point.

``` c
int psf_sndClose(int sfd);
```

The return value is used to indicate any error arising from closing the file.

As sample frames are written to disk, the maximum value per channel is automatically tracked, so that when the file is closed, the PEAK data can be written to the header.

For this to happen, `minheader` in `psf_sndCreate` must set to 0

support access to this information, to hold the peak data for one channel.

``` c
typedef struct psf_chpeak {
    float val;
    unsigned long pos;
} PSF_CHPEAK;
```

The library necessarily includes a function to read the current PEAK data from the file:

``` c
long psf_sndReadPeaks(int sfd, PSF_CHPEAK peakdata[], long *peaktime);
```

This functio ntakes a soundfile descriptor (as returned from `psf_sndCreate`), and a pointer to an array of `PSF_CHPEAK` structures. It is the responsibility of the user to ensure that the array is at least large enough to contain the data for each channel in the file (e.g. as given in props.chans):

``` c
PSF_CHPEAK peaks[6];
long peaks_valid, peaktime;
peaks_valid = psf_sndReadPeaks(ofd, peaks, &peaktime);
```

in most cases, the number of channels in a file will have been chosen by the user, so that you will have to allocate space for the peaks array using `malloc`

``` c
PSF_CHPEAK *peaks;
peaks = (PSF_CHPEAK *) malloc(props.chans * sizeof(PSF_CHPEAK));
```

The `peaktime` value is rarely needed and can generally be ignored.

### 2.1.6 Reading and Writing -- The Sample Frame

The working unit in `portsf` is the multi-channel sample frame. This means that it is not posible, for example, to read just the first (left-channel) sample of a stereo file. This would in all but the most abnormal circumstances be an error, and `portsf` prevents the user from making such errors.

the library automaticaly performs all the low-level calculations and conversions required to read sample frames (of any supported type) from a soundfile into the user's working sample buffer.

the recommanded sample format is the 32-bit float, into which 24-bit sampels can be converted withhout loss of percision:

``` c
long psf_sndReadFloatFrames(int sfd, float *buf, DWORD nFrames);
long psf_sndWriteFloatFrames(int sfd, const float *buf, DWORD nFrames);

// The functions both return the value -1 for an error
// psf_sndReadFloatFrames a return value than nFrames will indicate that the end of the file has been reached.
```

``` c
float frame * = (float *) malloc(props.chans * sizeof(float));

/* copy file one (multi-channel) frame at a time */
framesread = psf_sndReadFloatFrames(ifd, frame, 1);

while (framesread == 1) {
    // SOME PROCESSING

    psf_sndWriteFloatFrames(ofd, frame, 1);
    framesread = psf_sndReadFloatFrames(ifd, frame, 1);
}
```

This will copy a soundfile having any number of channels, and in any of the supported sample formats, while also tracking the PEAK value and position for each channel.

in a program that performs any processing, this compact loop probably will expand to accommodate code to apply filtering or other signal processing.

To change the file and sample format from that of the input file, all that is required is to modify the `PSF_PROPS` structre as desired



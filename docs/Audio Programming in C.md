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


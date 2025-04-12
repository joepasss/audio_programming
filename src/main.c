#include <portsf.h>
#include <stdio.h>
#include <stdlib.h>

int
check_sampletype(psf_stype type)
{
    int accept = 1;

    printf("sample type: ");
    switch (type)
    {
        case (PSF_SAMP_8):
            printf("8 bit\n");
            accept = 0;
            break;
        case (PSF_SAMP_16):
            printf("16 bit\n");
            break;
        case (PSF_SAMP_24):
            printf("24 bit\n");
            break;
        case (PSF_SAMP_32):
            printf("32 bit\n");
            break;
        case (PSF_SAMP_IEEE_FLOAT):
            printf("32 bit floating point\n");
            break;
        default:
            printf("unknown\n");
            accept = 0;
    }

    return accept;
}

void
read_wav_file(char *filename, PSF_PROPS *props)
{
    int sf;
    sf = psf_sndOpen(filename, props, 0);

    if (sf < 0)
    {
        printf("ERROR: unable to open soundfile\n");
        exit(EXIT_FAILURE);
    }

    printf("Sample rate = %d\n", props->srate);
    printf("Number of channels = %d\n", props->chans);

    if (!check_sampletype(props->samptype))
    {
        printf("file has unsupported sample type\n");
        exit(EXIT_FAILURE);
    }
}

void
write_wav_file(char *filename, PSF_PROPS *props)
{
    int ofd;

    // define a hi-res 5.1 surround WAVE-EX file. with PEAK chunk
    props->srate = 96000;
    props->chans = 6;
    props->samptype = PSF_SAMP_24;
    props->format = PSF_WAVE_EX;
    props->chformat = MC_DOLBY_5_1;

    ofd = psf_sndCreate(filename, props, 1, 0, PSF_CREATE_RDWR);

    if (ofd < 0)
    {
        printf("ERROR: unable to create output file\n");
        exit(EXIT_FAILURE);
    }
}

int
main(void)
{

    PSF_PROPS props;
    write_wav_file("soundtrack.wav", &props);

    return 0;
}

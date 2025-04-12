#include <portsf.h>
#include <stdio.h>

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

int
main(void)
{

    PSF_PROPS props;
    int sf;

    sf = psf_sndOpen("riviera_paradise_intro.wav", &props, 0);

    if (sf < 0)
    {
        printf("Error: unable to open soundfile\n");
        return 1;
    }

    printf("Sample rate = %d\n", props.srate);
    printf("Number of channels = %d\n", props.chans);

    if (!check_sampletype(props.samptype))
    {
        printf("file has unsupported sample type\n");
        return 1;
    }

    return 0;
}

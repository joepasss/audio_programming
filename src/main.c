/*
 * convert soundfile to floats
 */

#include <portsf.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    ARG_PROGNAME,
    ARG_INFILE,
    ARG_OUTFILE,
    ARG_NARGS
};

int
main(int argc, char *argv[])
{
    PSF_PROPS props;
    long framesread, totalread;

    /* init all resource vars to deafult states */
    int ifd = -1, ofd = -1;
    int error = 0;
    psf_format outformat = PSF_FMT_UNKNOWN;
    PSF_CHPEAK *peaks = NULL;
    float *frame = NULL;

    printf("SF2FLOAT: convert soundfile to floats format\n");

    if (argc < ARG_NARGS)
    {
        printf("insufficient arguments.\n"
               "usage:\n\t%s infile outfile\n",
               argv[0]);

        return 1;
    }

    if (psf_init())
    {
        printf("unable to start portsf\n");
        return 1;
    }

    ifd = psf_sndOpen(argv[ARG_INFILE], &props, 0);
    if (ifd < 0)
    {
        printf("Error: unable to open infile %s\n", argv[ARG_INFILE]);
        return 1;
    }

    if (props.samptype == PSF_SAMP_IEEE_FLOAT)
    {
        printf("Info: infile is already in floats format.\n");
    }

    props.samptype = PSF_SAMP_IEEE_FLOAT;

    outformat = psf_getFormatExt(argv[ARG_OUTFILE]);
    if (outformat == PSF_FMT_UNKNOWN)
    {
        printf("outfile name %s has unknown format.\n"
               "Use any of .wav, .aiff, .aif, .afc, .aifc\n",
               argv[ARG_OUTFILE]);

        error++;
        goto exit;
    }
    props.format = outformat;

    ofd = psf_sndCreate(argv[2], &props, 0, 0, PSF_CREATE_RDWR);
    if (ofd < 0)
    {
        printf("ERROR: unable to create outfile %s\n", argv[ARG_OUTFILE]);
        error++;
        goto exit;
    }

    frame = (float *)malloc(props.chans * sizeof(float));
    if (frame == NULL)
    {
        puts("No memory!\n");
        error++;
        goto exit;
    }

    peaks = (PSF_CHPEAK *)malloc(props.chans * sizeof(PSF_CHPEAK));
    if (peaks == NULL)
    {
        puts("No memory!\n");
        error++;
        goto exit;
    }

    printf("copying ...\n");

    framesread = psf_sndReadFloatFrames(ifd, frame, 1);
    totalread = 0;
    while (framesread == 1)
    {
        totalread++;
        if (psf_sndWriteFloatFrames(ofd, frame, 1) != 1)
        {
            printf("Error writing to outfile\n");
            error++;
            break;
        }

        /* do any processing here! */
        framesread = psf_sndReadFloatFrames(ifd, frame, 1);
    }

    if (framesread < 0)
    {
        printf("Error reading infile. Outfile is incomplete.\n");
        error++;
    }
    else
        printf("Done. %ld sample frames copied to %s\n", totalread,
               argv[ARG_OUTFILE]);

    if (psf_sndReadPeaks(ofd, peaks, NULL) > 0)
    {
        long i;
        double peaktime;

        printf("PEAK information:\n");
        for (i = 0; i < props.chans; i++)
        {
            peaktime = (double)peaks[i].pos / props.srate;

            printf("CH %ld:\t%.4f at %.4f secs\n", i + 1, peaks[i].val,
                   peaktime);
        }
    }

exit:
    if (ifd >= 0)
        psf_sndClose(ifd);
    if (ofd >= 0)
        psf_sndClose(ofd);
    if (frame)
        free(frame);
    if (peaks)
        free(peaks);
    psf_finish();
    return error;

    return 0;
}

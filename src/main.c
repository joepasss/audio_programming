#include <math.h>
#include <stdio.h>

int
main()
{
    double semitone_ratio;
    double c0; /* for frequency of MIDI Note 0 */
    double c5; /* for frequency of Middle c */
    double frequency;
    int midinote;

    semitone_ratio = pow(2, 1 / 12.0);
    c5 = 220.0 * pow(semitone_ratio, 3);
    c0 = c5 * pow(0.5, 5);

    midinote = 73; /* C# above A = 440 */
    frequency = c0 * pow(semitone_ratio, midinote);

    printf("Midi Note %d has frequency %f\n", midinote, frequency);

    return 0;
}

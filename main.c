#include <stdio.h>
#include <string.h>
#include <stdlib.h>

union EndianTest {
	unsigned int a;
	unsigned char b[4];
};

void swpendian(unsigned char *d, int n)
{
	unsigned char t;
	switch (n) {
	case 2:
		t = d[0];
		d[0] = d[1];
		d[1] = t;
		break;
	case 3:
		t = d[0];
		d[0] = d[2];
		d[2] = t;
		break;
	case 4:
		t = d[0];
		d[0] = d[3];
		d[3] = t;
		t = d[1];
		d[1] = d[2];
		d[2] = t;
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	union EndianTest et;
	FILE *rawf;
	FILE *wavf;
	int bigendian;
	int i;
#define BUF_SIZE 4096
	unsigned char buf[BUF_SIZE];
	size_t n;
	long curpos;
	long endpos;

	unsigned int size1;
	unsigned int size2;
	unsigned short pcm;
	unsigned short ch;
	unsigned int sample;
	unsigned int BPS;
	unsigned short block;
	unsigned short bit;
	unsigned short size3;
	unsigned int wavsize;

	unsigned int sample_n;

	if (argc != 6) {
		printf
		    ("Usage:\t%s BIT CHANNEL SAMPLING_HZ INPUT_RAW_FILE OUTPUT_WAV_FILE\n\n", argv[0]);
		return 1;
	}
	bit = atoi(argv[1]);
	switch (bit) {
		case 16:
		case 8:
			break;
		default:
			goto end1;
			break;
	}
	ch = atoi(argv[2]);
	switch (ch) {
		case 1:
		case 2:
			break;
		default:
			goto end1;
			break;
	}
	sample = atoi(argv[3]);
	switch (sample) {
		case 44100:
		case 48000:
		case 96000:
			break;
		default:
			goto end1;
			break;
	}
	BPS = sample * ch * (bit / 8);
	block = (bit / 8) * ch;
	et.a = 0x12345678;
	bigendian = (et.b[0] == 0x12 &&
		     et.b[1] == 0x34 &&
		     et.b[2] == 0x56 && et.b[3] == 0x78);

	rawf = fopen(argv[4], "rb");
	if (!rawf) {
		goto end1;
	}
	wavf = fopen(argv[5], "wb");
	if (!wavf) {
		goto end2;
	}
	curpos = ftell(rawf);
	fseek(rawf, 0, SEEK_END);
	endpos = ftell(rawf);
	fseek(rawf, 0, SEEK_SET);

	fwrite("RIFF", 1, 4, wavf);
	size1 = endpos - curpos + 38;
	if (bigendian) {
		swpendian((unsigned char *) &size1, 4);
	}
	fwrite(&size1, 4, 1, wavf);
	fwrite("WAVE", 1, 4, wavf);
	fwrite("fmt ", 1, 4, wavf);
	size2 = 16;
	if (bigendian) {
		swpendian((unsigned char *) &size2, 4);
	}
	fwrite(&size2, 4, 1, wavf);
	pcm = 1;
	if (bigendian) {
		swpendian((unsigned char *) &pcm, 2);
	}
	fwrite(&pcm, 2, 1, wavf);
	if (bigendian) {
		swpendian((unsigned char *) &ch, 2);
	}
	fwrite(&ch, 2, 1, wavf);
	if (bigendian) {
		swpendian((unsigned char *) &sample, 4);
	}
	fwrite(&sample, 4, 1, wavf);
	if (bigendian) {
		swpendian((unsigned char *) &BPS, 4);
	}
	fwrite(&BPS, 4, 1, wavf);
	if (bigendian) {
		swpendian((unsigned char *) &block, 2);
	}
	fwrite(&block, 2, 1, wavf);
	if (bigendian) {
		swpendian((unsigned char *) &bit, 2);
	}
	fwrite(&bit, 2, 1, wavf);
	fwrite("data", 1, 4, wavf);
	wavsize = endpos - curpos;
	if (bigendian) {
		swpendian((unsigned char *) &wavsize, 4);
	}
	fwrite(&wavsize, 4, 1, wavf);
	while ((n = fread(buf, 1, BUF_SIZE, rawf)) > 0) {
		fwrite(buf, 1, n, wavf);
	}

	fclose(wavf);
      end2:
	fclose(rawf);
      end1:

	return 0;
}

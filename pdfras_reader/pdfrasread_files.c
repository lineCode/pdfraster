#include "pdfrasread_files.h"
#include <string.h>
#include <ctype.h>
#include "..\pdfras_writer\PdfPlatform.h"

// Return TRUE if the file starts with the signature of a PDF/raster file.
// FALSE otherwise.
int pdfrasread_recognize_file(FILE* f)
{
	if (!f) {
		return FALSE;
	}
	char sig[32];
	size_t nb = fread(sig, sizeof(char), sizeof sig - 1, f);
	sig[nb] = (char)0;
	return pdfras_recognize_signature(sig);
}

// Return TRUE if the file starts with the signature of a PDF/raster file.
// FALSE otherwise.
int pdfrasread_recognize_filename(const char* fn)
{
	int bResult = FALSE;
	FILE* f = fopen(fn, "rb");
	if (f) {
		bResult = pdfrasread_recognize_file(f);
		fclose(f);
	}
	return bResult;
}

static size_t file_reader(void *source, pduint32 offset, size_t length, char *buffer)
{
	FILE* f = (FILE*)source;
	if (0 != fseek(f, offset, SEEK_SET)) {
		return 0;
	}
	return fread(buffer, sizeof(pduint8), length, f);
}

static void file_closer(void* source)
{
	if (source) {
		FILE* f = (FILE*)source;
		fclose(f);
	}
}

// Return the page count of the PDF/raster file f
// Does NOT close f.
int pdfrasread_page_count_file(FILE* f)
{
	int nPages = -1;
	// construct a PDF/raster reader based on the file
	t_pdfrasreader* reader = pdfrasread_create(PDFRAS_API_LEVEL, &file_reader, NULL);
	if (reader) {
		if (pdfrasread_open(reader, f)) {
			// count its pages
			nPages = pdfrasread_page_count(reader);
		}
		// destroy the reader
		pdfrasread_destroy(reader);
	}
	return nPages;
}

int pdfrasread_page_count_filename(const char* fn)
{
	int nPages = -1;
	FILE* f = fopen(fn, "rb");
	if (f) {
		nPages = pdfrasread_page_count_file(f);
		fclose(f);
	}
	return nPages;
}

t_pdfrasreader* pdfrasread_open_file(int apiLevel, FILE* f)
{
	t_pdfrasreader* reader = pdfrasread_create(apiLevel, &file_reader, &file_closer);
	if (reader) {
		if (!pdfrasread_open(reader, f)) {
			pdfrasread_destroy(reader);
			reader = NULL;
		}
	}
	return reader;
}

t_pdfrasreader* pdfrasread_open_filename(int apiLevel, const char* fn)
{
	t_pdfrasreader* reader = NULL;
	FILE* f = fopen(fn, "rb");
	if (f) {
		// construct a PDF/raster reader based on the file
		reader = pdfrasread_open_file(apiLevel, f);
		if (!reader) {
			// open failed, we have to close the file ourselves
			fclose(f);
		}
	}
	return reader;
}

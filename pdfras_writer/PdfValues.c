#include "PdfValues.h"
#include "PdfString.h"
#include "PdfStrings.h"
#include "PdfDict.h"
#include "PdfArray.h"

static t_pdvalue __pdnull = { 0, TPDNULL, { 0 } };
static t_pdvalue __pderr = { 0, TPDERRVALUE, { 0 } };

t_pdvalue pdatomvalue(t_pdatom v)
{
	t_pdvalue val = { 0, TPDNAME, { .namevalue = v } };
	return val;
}

t_pdvalue pderrvalue()
{
	return __pderr;
}

t_pdvalue pdnullvalue()
{
	return __pdnull;
}

t_pdvalue pdintvalue(pdint32 v)
{
	t_pdvalue val = { 0, TPDNUMBERINT, { v } };
	return val;
}

t_pdvalue pdfloatvalue(double v)
{
	t_pdvalue val = { 0, TPDNUMBERFLOAT, { .floatvalue = v } };
	return val;
}

t_pdvalue pdboolvalue(pdbool v)
{
	t_pdvalue val = { 0, TPDBOOL, { .boolvalue = v } };
	return val;
}

t_pdvalue pdarrayvalue(t_pdarray *arr)
{
	t_pdvalue val = { 0, TPDARRAY, { .arrvalue = arr } };
	return val;
}

t_pdvalue pdstringvalue(t_pdstring *str)
{
	t_pdvalue val = { 0, TPDSTRING, { .stringvalue = str } };
	return val;
}

t_pdvalue pdcstrvalue(t_pdallocsys *alloc, const char *s)
{
	t_pdstring *str = pd_string_new(alloc, s, pdstrlen(s), PD_FALSE);
	if (!str) return pderrvalue();
	return pdstringvalue(str);
}

int pd_value_eq(t_pdvalue a, t_pdvalue b)
{
	if (a.pdtype == b.pdtype) {
		switch (a.pdtype) {
		case TPDARRAY:
			return a.value.arrvalue == b.value.arrvalue;
		case TPDBOOL:
			return a.value.boolvalue == b.value.boolvalue;
		case TPDDICT:
			return a.value.dictvalue == b.value.dictvalue;
		case TPDNULL:				// all null values are EQ
		case TPDERRVALUE:			// all error values are EQ
			return 1;
		case TPDNAME:
			// why does this work? Because names (atoms) are
			// supposed to be uniquified: EQ iff EQUAL.
			return a.value.namevalue == b.value.namevalue;
		case TPDNUMBERFLOAT:
			return a.value.floatvalue == b.value.floatvalue;
		case TPDNUMBERINT:
			return a.value.intvalue == b.value.intvalue;
		case TPDREFERENCE:
			return a.value.refvalue == b.value.refvalue;
		case TPDSTRING:
			return pd_string_equal(a.value.stringvalue, b.value.stringvalue);
		}
	}
	return 1;
}

void pd_value_free(t_pdvalue *v)
{
	if (v) {
		switch (v->pdtype) {
		case TPDDICT:
			pd_dict_free(*v);
			break;
		case TPDARRAY:
			pd_array_free(v->value.arrvalue);
			break;
		case TPDSTRING:
			pd_string_free(v->value.stringvalue);
			break;
		default:
			break;
		}
		*v = pdnullvalue();
	}
}
/*
 * E164 Type PostgreSQL interface
 * $Id: e164.c 52 2007-08-27 15:56:09Z glaesema $
 */
#ifndef E164_C
#define E164_C

#include <postgres.h>
#include "libpq/pqformat.h"
#include "access/hash.h"
#include "e164_base.c"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


/*
 * PostgreSQL Interface functions
 */

/*
 * makeText, textLength, and setTextLength functions taken from
 * AndrewSN's ip4r
 */

static
text * makeText (char * aString, int stringLength)
{
    text * textString = (text *) palloc(stringLength + VARHDRSZ);
    VARATT_SIZEP(textString) = stringLength + VARHDRSZ;
    if (aString)
        memcpy(VARDATA(textString), aString, stringLength);
    else
        memset(VARDATA(textString), 0, stringLength);
    return textString;
}

static inline
void setTextLength (text * textString, int stringLength)
{
    if ((stringLength + VARHDRSZ) < VARATT_SIZEP(textString))
        VARATT_SIZEP(textString) = stringLength + VARHDRSZ;
}

static inline
int textLength(text * textString)
{
    return VARSIZE(textString) - VARHDRSZ;
}

static void handleE164ParseError (E164ParseResult error, char * string);

static
void handleE164ParseError (E164ParseResult error, char * string)
{
    switch (error) {
        case E164ParseErrorBadFormat:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("invalid E164 number format: \"%s\"", string),
                     errhint("E164 numbers begin with a \"+\" followed by digits.")));
            break;
        case E164ParseErrorInvalidPrefix:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("invalid E164 prefix: \"%s\"", string),
                     errhint("E164 numbers must begin with \"%c\".", E164_PREFIX)));
            break;
        case E164ParseErrorStringTooLong:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("string too long: \"%s\"", string),
                     errhint("E164 values must have at most %d digits.", E164MaximumNumberOfDigits)));
            break;
        case E164ParseErrorStringTooShort:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("string too short \"%s\"", string),
                     errhint("E164 numbers must have at least %d digits.", E164MinimumNumberOfDigits)));
            break;
        case E164ParseErrorInvalidType:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("invalid E164 country code for E164 number \"%s\"", string)));
            break;
        case E164ParseErrorNoSubscriberNumberDigits:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("no subscriber number digits in E164 number \"%s\"", string)));
            break;
        case E164ParseErrorUnassignedType:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("unassigned country code for E164 number \"%s\"", string)));
            break;
        case E164ParseErrorTypeLengthMismatch:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("inconsistent length and country code: \"%s\"", string)));
            break;
        default:
            /*
             * FIXME
             * Handle the default cause properly: shouldn't ever reach this point
             */
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("unknown input error for E164 number \"%s\"", string)));
    }
}

#define DatumGetE164P(X) ((E164 *) DatumGetPointer(X))
#define E164PGetDatum(X) PointerGetDatum(X)
#define PG_GETARG_E164_P(X) DatumGetE164P(PG_GETARG_DATUM(X))
#define PG_RETURN_E164_P(X) return E164PGetDatum(X)
#define PG_GETARG_E164(X) PG_GETARG_INT64((int64) X)
#define PG_RETURN_E164(X) PG_RETURN_INT64((int64) X)

Datum e164_in(PG_FUNCTION_ARGS);
Datum e164_out(PG_FUNCTION_ARGS);

Datum e164_hash(PG_FUNCTION_ARGS);
Datum e164_send(PG_FUNCTION_ARGS);
Datum e164_recv(PG_FUNCTION_ARGS);

Datum e164_lt(PG_FUNCTION_ARGS);
Datum e164_le(PG_FUNCTION_ARGS);
Datum e164_eq(PG_FUNCTION_ARGS);
Datum e164_ge(PG_FUNCTION_ARGS);
Datum e164_gt(PG_FUNCTION_ARGS);
Datum e164_ne(PG_FUNCTION_ARGS);

Datum e164_cmp(PG_FUNCTION_ARGS);

Datum e164_cast_to_text(PG_FUNCTION_ARGS);

Datum country_code(PG_FUNCTION_ARGS);
Datum is_consistent(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(e164_in);
Datum
e164_in(PG_FUNCTION_ARGS)
{
    char * theString = PG_GETARG_CSTRING(0);
    E164 theNumber;
    E164ParseResult parseResult = e164FromString(&theNumber, theString);
    if (E164NoParseError == parseResult)
    {
        E164 * numberResult = palloc(sizeof(E164));
        *numberResult = theNumber;
        PG_RETURN_E164_P(numberResult);
    }
    else
        handleE164ParseError(parseResult, theString);
}

PG_FUNCTION_INFO_V1(e164_out);
Datum
e164_out(PG_FUNCTION_ARGS)
{
    E164 * theNumber = PG_GETARG_E164_P(0);
    char * theString = palloc(E164MaximumStringLength + 1);
    (void) stringFromE164(theString, theNumber, E164MaximumStringLength);
    PG_RETURN_CSTRING(theString);
}

PG_FUNCTION_INFO_V1(e164_recv);
Datum
e164_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    PG_RETURN_E164((E164) pq_getmsgint(buf, sizeof(E164)));
}

PG_FUNCTION_INFO_V1(e164_send);
Datum
e164_send(PG_FUNCTION_ARGS)
{
    E164 arg1 = PG_GETARG_E164(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendint(&buf, arg1, sizeof(E164));
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(e164_cast_to_text);
Datum
e164_cast_to_text(PG_FUNCTION_ARGS)
{
    E164 * theNumber = PG_GETARG_E164_P(0);
    text * textString = makeText(NULL, E164MaximumStringLength);
    setTextLength(textString,
                  stringFromE164(VARDATA(textString),
                                 theNumber,
                                 E164MaximumStringLength));
    PG_RETURN_TEXT_P(textString);
}

PG_FUNCTION_INFO_V1(is_consistent);
Datum
is_consistent(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsConsistent(PG_GETARG_E164_P(0)));
}

PG_FUNCTION_INFO_V1(country_code);
Datum
country_code(PG_FUNCTION_ARGS)
{
    E164 * aNumber = PG_GETARG_E164_P(0);
    text * textString = makeText(NULL, e164CountryCodeLength(aNumber));
    setTextLength(textString,
                  countryCodeStringFromE164(VARDATA(textString), aNumber));
    PG_RETURN_TEXT_P(textString);
}

PG_FUNCTION_INFO_V1(e164_lt);
Datum
e164_lt(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsLessThan(PG_GETARG_E164_P(0),
                                  PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_le);
Datum
e164_le(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsLessThanOrEqualTo(PG_GETARG_E164_P(0),
                                           PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_eq);
Datum
e164_eq(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsEqualTo(PG_GETARG_E164_P(0),
                                 PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_ge);
Datum
e164_ge(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsGreaterThanOrEqualTo(PG_GETARG_E164_P(0),
                                              PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_gt);
Datum
e164_gt(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsGreaterThan(PG_GETARG_E164_P(0),
                                     PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_ne);
Datum
e164_ne(PG_FUNCTION_ARGS)
{
    PG_RETURN_BOOL(e164IsNotEqualTo(PG_GETARG_E164_P(0),
                                    PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_cmp);
Datum
e164_cmp(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT32(e164Comparison(PG_GETARG_E164_P(0),
                                   PG_GETARG_E164_P(1)));
}

PG_FUNCTION_INFO_V1(e164_hash);
Datum
e164_hash(PG_FUNCTION_ARGS)
{
    E164 arg1 = PG_GETARG_E164(0);
    return hash_any((unsigned char *)&arg1, sizeof(E164));
}

#endif /* !E164_C */

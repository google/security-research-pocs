#include "yaml.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#define BUFFER_SIZE 65536
#define MAX_DOCUMENTS  16
#define MAX_EVENTS  1024

static int canonical = 0;
static int unicode = 0;
static char *curpos = NULL;
static size_t remainder = 0;

int
reformatter_main(void)
{
    int k;
    int done = 0;

    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_event_t event;

    /* Clear the objects. */

    memset(&parser, 0, sizeof(parser));
    memset(&emitter, 0, sizeof(emitter));
    memset(&event, 0, sizeof(event));

    /* Initialize the parser and emitter objects. */

    if (!yaml_parser_initialize(&parser))
        goto parser_error;

    if (!yaml_emitter_initialize(&emitter))
        goto emitter_error;

    /* Set the parser parameters. */

    yaml_parser_set_input_string(&parser, curpos, remainder);

    /* Set the emitter parameters. */

    yaml_emitter_set_output_file(&emitter, stdout);

    yaml_emitter_set_canonical(&emitter, canonical);
    yaml_emitter_set_unicode(&emitter, unicode);

    /* The main loop. */

    while (!done)
    {
        /* Get the next event. */

        if (!yaml_parser_parse(&parser, &event))
            goto parser_error;

        /* Check if this is the stream end. */

        if (event.type == YAML_STREAM_END_EVENT) {
            done = 1;
        }

        /* Emit the event. */

        if (!yaml_emitter_emit(&emitter, &event))
            goto emitter_error;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 0;

parser_error:

    /* Display a parser error message. */

    switch (parser.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", parser.problem,
                        parser.problem_value, parser.problem_offset);
            }
            else {
                fprintf(stderr, "Reader error: %s at %d\n", parser.problem,
                        parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

emitter_error:

    /* Display an emitter error message. */

    switch (emitter.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            fprintf(stderr, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            fprintf(stderr, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;
}

int
reformatter_alt_main(void)
{
    int k;
    int done = 0;

    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_document_t document;

    /* Clear the objects. */

    memset(&parser, 0, sizeof(parser));
    memset(&emitter, 0, sizeof(emitter));
    memset(&document, 0, sizeof(document));

    /* Initialize the parser and emitter objects. */

    if (!yaml_parser_initialize(&parser))
        goto parser_error;

    if (!yaml_emitter_initialize(&emitter))
        goto emitter_error;

    /* Set the parser parameters. */

    yaml_parser_set_input_string(&parser, curpos, remainder);

    /* Set the emitter parameters. */

    yaml_emitter_set_output_file(&emitter, stdout);

    yaml_emitter_set_canonical(&emitter, canonical);
    yaml_emitter_set_unicode(&emitter, unicode);

    /* The main loop. */

    while (!done)
    {
        /* Get the next event. */

        if (!yaml_parser_load(&parser, &document))
            goto parser_error;

        /* Check if this is the stream end. */

        if (!yaml_document_get_root_node(&document)) {
            done = 1;
        }

        /* Emit the event. */

        if (!yaml_emitter_dump(&emitter, &document))
            goto emitter_error;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 0;

parser_error:

    /* Display a parser error message. */

    switch (parser.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", parser.problem,
                        parser.problem_value, parser.problem_offset);
            }
            else {
                fprintf(stderr, "Reader error: %s at %d\n", parser.problem,
                        parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_COMPOSER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Composer error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Composer error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

emitter_error:

    /* Display an emitter error message. */

    switch (emitter.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            fprintf(stderr, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            fprintf(stderr, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;
}

int
deconstructor_main(void)
{
    int k;
    int done = 0;

    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_event_t input_event;
    yaml_event_t output_event;

    /* Clear the objects. */

    memset(&parser, 0, sizeof(parser));
    memset(&emitter, 0, sizeof(emitter));
    memset(&input_event, 0, sizeof(input_event));
    memset(&output_event, 0, sizeof(output_event));

    /* Initialize the parser and emitter objects. */

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Could not initialize the parser object\n");
        return 1;
    }

    if (!yaml_emitter_initialize(&emitter)) {
        yaml_parser_delete(&parser);
        fprintf(stderr, "Could not inialize the emitter object\n");
        return 1;
    }

    /* Set the parser parameters. */

    yaml_parser_set_input_string(&parser, curpos, remainder);

    /* Set the emitter parameters. */

    yaml_emitter_set_output_file(&emitter, stdout);

    yaml_emitter_set_canonical(&emitter, canonical);
    yaml_emitter_set_unicode(&emitter, unicode);

    /* Create and emit the STREAM-START event. */

    if (!yaml_stream_start_event_initialize(&output_event, YAML_UTF8_ENCODING))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    /* Create and emit the DOCUMENT-START event. */

    if (!yaml_document_start_event_initialize(&output_event,
                NULL, NULL, NULL, 0))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    /* Create and emit the SEQUENCE-START event. */

    if (!yaml_sequence_start_event_initialize(&output_event,
                NULL, "tag:yaml.org,2002:seq", 1,
                YAML_BLOCK_SEQUENCE_STYLE))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    /* Loop through the input events. */

    while (!done)
    {
        /* Get the next event. */

        if (!yaml_parser_parse(&parser, &input_event))
            goto parser_error;

        /* Check if this is the stream end. */

        if (input_event.type == YAML_STREAM_END_EVENT) {
            done = 1;
        }

        /* Create and emit a MAPPING-START event. */

        if (!yaml_mapping_start_event_initialize(&output_event,
                    NULL, "tag:yaml.org,2002:map", 1,
                    YAML_BLOCK_MAPPING_STYLE))
            goto event_error;
        if (!yaml_emitter_emit(&emitter, &output_event))
            goto emitter_error;

        /* Analyze the event. */

        switch (input_event.type)
        {
            case YAML_STREAM_START_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'STREAM-START'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "STREAM-START", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display encoding information. */

                if (input_event.data.stream_start.encoding)
                {
                    yaml_encoding_t encoding
                        = input_event.data.stream_start.encoding;

                    /* Write 'encoding'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "encoding", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the stream encoding. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                (encoding == YAML_UTF8_ENCODING ? "utf-8" :
                                 encoding == YAML_UTF16LE_ENCODING ? "utf-16-le" :
                                 encoding == YAML_UTF16BE_ENCODING ? "utf-16-be" :
                                 "unknown"), -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                break;

            case YAML_STREAM_END_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'STREAM-END'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "STREAM-END", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            case YAML_DOCUMENT_START_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'DOCUMENT-START'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "DOCUMENT-START", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the document version numbers. */

                if (input_event.data.document_start.version_directive)
                {
                    yaml_version_directive_t *version
                        = input_event.data.document_start.version_directive;
                    char number[64];

                    /* Write 'version'. */
                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "version", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write '{'. */

                    if (!yaml_mapping_start_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:map", 1,
                                YAML_FLOW_MAPPING_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write 'major'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "major", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write a number. */

                    sprintf(number, "%d", version->major);
                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:int", number, -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write 'minor'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "minor", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write a number. */

                    sprintf(number, "%d", version->minor);
                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:int", number, -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write '}'. */

                    if (!yaml_mapping_end_event_initialize(&output_event))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Display the document tag directives. */

                if (input_event.data.document_start.tag_directives.start
                        != input_event.data.document_start.tag_directives.end)
                {
                    yaml_tag_directive_t *tag;

                    /* Write 'tags'. */
                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "tags", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Start a block sequence. */

                    if (!yaml_sequence_start_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:seq", 1,
                                YAML_BLOCK_SEQUENCE_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    for (tag = input_event.data.document_start.tag_directives.start;
                            tag != input_event.data.document_start.tag_directives.end;
                            tag ++)
                    {
                        /* Write '{'. */

                        if (!yaml_mapping_start_event_initialize(&output_event,
                                    NULL, "tag:yaml.org,2002:map", 1,
                                    YAML_FLOW_MAPPING_STYLE))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;

                        /* Write 'handle'. */

                        if (!yaml_scalar_event_initialize(&output_event,
                                    NULL, "tag:yaml.org,2002:str", "handle", -1,
                                    1, 1, YAML_PLAIN_SCALAR_STYLE))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;

                        /* Write the tag directive handle. */

                        if (!yaml_scalar_event_initialize(&output_event,
                                    NULL, "tag:yaml.org,2002:str",
                                    tag->handle, -1,
                                    0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;

                        /* Write 'prefix'. */

                        if (!yaml_scalar_event_initialize(&output_event,
                                    NULL, "tag:yaml.org,2002:str", "prefix", -1,
                                    1, 1, YAML_PLAIN_SCALAR_STYLE))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;

                        /* Write the tag directive prefix. */

                        if (!yaml_scalar_event_initialize(&output_event,
                                    NULL, "tag:yaml.org,2002:str",
                                    tag->prefix, -1,
                                    0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;

                        /* Write '}'. */

                        if (!yaml_mapping_end_event_initialize(&output_event))
                            goto event_error;
                        if (!yaml_emitter_emit(&emitter, &output_event))
                            goto emitter_error;
                    }

                    /* End a block sequence. */

                    if (!yaml_sequence_end_event_initialize(&output_event))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Write 'implicit'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "implicit", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the document is implicit. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.document_start.implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            case YAML_DOCUMENT_END_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'DOCUMENT-END'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "DOCUMENT-END", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'implicit'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "implicit", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the document is implicit. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.document_end.implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            case YAML_ALIAS_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'ALIAS'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "ALIAS", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'anchor'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "anchor", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write the alias anchor. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str",
                            input_event.data.alias.anchor, -1,
                            0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            case YAML_SCALAR_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'SCALAR'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "SCALAR", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the scalar anchor. */

                if (input_event.data.scalar.anchor)
                {
                    /* Write 'anchor'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "anchor", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the scalar anchor. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.scalar.anchor, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Display the scalar tag. */

                if (input_event.data.scalar.tag)
                {
                    /* Write 'tag'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "tag", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the scalar tag. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.scalar.tag, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Display the scalar value. */

                /* Write 'value'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "value", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write the scalar value. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str",
                            input_event.data.scalar.value,
                            input_event.data.scalar.length,
                            0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display if the scalar tag is implicit. */

                /* Write 'implicit'. */
                
                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "implicit", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write '{'. */

                if (!yaml_mapping_start_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:map", 1,
                            YAML_FLOW_MAPPING_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'plain'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "plain", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the scalar is implicit in the plain style. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.scalar.plain_implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'quoted'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "non-plain", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the scalar is implicit in a non-plain style. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.scalar.quoted_implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write '}'. */

                if (!yaml_mapping_end_event_initialize(&output_event))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the style information. */

                if (input_event.data.scalar.style)
                {
                    yaml_scalar_style_t style = input_event.data.scalar.style;

                    /* Write 'style'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "style", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the scalar style. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                (style == YAML_PLAIN_SCALAR_STYLE ? "plain" :
                                 style == YAML_SINGLE_QUOTED_SCALAR_STYLE ?
                                        "single-quoted" :
                                 style == YAML_DOUBLE_QUOTED_SCALAR_STYLE ?
                                        "double-quoted" :
                                 style == YAML_LITERAL_SCALAR_STYLE ? "literal" :
                                 style == YAML_FOLDED_SCALAR_STYLE ? "folded" :
                                 "unknown"), -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                break;

            case YAML_SEQUENCE_START_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'SEQUENCE-START'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "SEQUENCE-START", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the sequence anchor. */

                if (input_event.data.sequence_start.anchor)
                {
                    /* Write 'anchor'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "anchor", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the sequence anchor. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.sequence_start.anchor, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Display the sequence tag. */

                if (input_event.data.sequence_start.tag)
                {
                    /* Write 'tag'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "tag", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the sequence tag. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.sequence_start.tag, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Write 'implicit'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "implicit", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the sequence tag is implicit. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.sequence_start.implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the style information. */

                if (input_event.data.sequence_start.style)
                {
                    yaml_sequence_style_t style
                        = input_event.data.sequence_start.style;

                    /* Write 'style'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "style", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the scalar style. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                (style == YAML_BLOCK_SEQUENCE_STYLE ? "block" :
                                 style == YAML_FLOW_SEQUENCE_STYLE ? "flow" :
                                 "unknown"), -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                break;

            case YAML_SEQUENCE_END_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'SEQUENCE-END'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "SEQUENCE-END", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            case YAML_MAPPING_START_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'MAPPING-START'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "MAPPING-START", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the mapping anchor. */

                if (input_event.data.mapping_start.anchor)
                {
                    /* Write 'anchor'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "anchor", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the mapping anchor. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.mapping_start.anchor, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Display the mapping tag. */

                if (input_event.data.mapping_start.tag)
                {
                    /* Write 'tag'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "tag", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the mapping tag. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                input_event.data.mapping_start.tag, -1,
                                0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                /* Write 'implicit'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "implicit", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write if the mapping tag is implicit. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:bool",
                            (input_event.data.mapping_start.implicit ?
                             "true" : "false"), -1,
                            1, 0, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Display the style information. */

                if (input_event.data.mapping_start.style)
                {
                    yaml_mapping_style_t style
                        = input_event.data.mapping_start.style;

                    /* Write 'style'. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str", "style", -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;

                    /* Write the scalar style. */

                    if (!yaml_scalar_event_initialize(&output_event,
                                NULL, "tag:yaml.org,2002:str",
                                (style == YAML_BLOCK_MAPPING_STYLE ? "block" :
                                 style == YAML_FLOW_MAPPING_STYLE ? "flow" :
                                 "unknown"), -1,
                                1, 1, YAML_PLAIN_SCALAR_STYLE))
                        goto event_error;
                    if (!yaml_emitter_emit(&emitter, &output_event))
                        goto emitter_error;
                }

                break;

            case YAML_MAPPING_END_EVENT:

                /* Write 'type'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "type", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                /* Write 'MAPPING-END'. */

                if (!yaml_scalar_event_initialize(&output_event,
                            NULL, "tag:yaml.org,2002:str", "MAPPING-END", -1,
                            1, 1, YAML_PLAIN_SCALAR_STYLE))
                    goto event_error;
                if (!yaml_emitter_emit(&emitter, &output_event))
                    goto emitter_error;

                break;

            default:
                /* It couldn't really happen. */
                break;
        }

        /* Delete the event object. */

        yaml_event_delete(&input_event);

        /* Create and emit a MAPPING-END event. */

        if (!yaml_mapping_end_event_initialize(&output_event))
            goto event_error;
        if (!yaml_emitter_emit(&emitter, &output_event))
            goto emitter_error;
    }

    /* Create and emit the SEQUENCE-END event. */

    if (!yaml_sequence_end_event_initialize(&output_event))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    /* Create and emit the DOCUMENT-END event. */

    if (!yaml_document_end_event_initialize(&output_event, 0))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    /* Create and emit the STREAM-END event. */

    if (!yaml_stream_end_event_initialize(&output_event))
        goto event_error;
    if (!yaml_emitter_emit(&emitter, &output_event))
        goto emitter_error;

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 0;

parser_error:

    /* Display a parser error message. */

    switch (parser.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", parser.problem,
                        parser.problem_value, parser.problem_offset);
            }
            else {
                fprintf(stderr, "Reader error: %s at %d\n", parser.problem,
                        parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_event_delete(&input_event);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

emitter_error:

    /* Display an emitter error message. */

    switch (emitter.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            fprintf(stderr, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            fprintf(stderr, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_event_delete(&input_event);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

event_error:

    fprintf(stderr, "Memory error: Not enough memory for creating an event\n");

    yaml_event_delete(&input_event);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;
}

int
deconstructor_alt_main(void)
{
    int k;
    int done = 0;

    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_event_t input_event;
    yaml_document_t output_document;

    int root;

    /* Clear the objects. */

    memset(&parser, 0, sizeof(parser));
    memset(&emitter, 0, sizeof(emitter));
    memset(&input_event, 0, sizeof(input_event));
    memset(&output_document, 0, sizeof(output_document));

    /* Initialize the parser and emitter objects. */

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Could not initialize the parser object\n");
        return 1;
    }

    if (!yaml_emitter_initialize(&emitter)) {
        yaml_parser_delete(&parser);
        fprintf(stderr, "Could not inialize the emitter object\n");
        return 1;
    }

    /* Set the parser parameters. */

    yaml_parser_set_input_string(&parser, curpos, remainder);

    /* Set the emitter parameters. */

    yaml_emitter_set_output_file(&emitter, stdout);

    yaml_emitter_set_canonical(&emitter, canonical);
    yaml_emitter_set_unicode(&emitter, unicode);

    /* Create and emit the STREAM-START event. */

    if (!yaml_emitter_open(&emitter))
        goto emitter_error;

    /* Create a output_document object. */

    if (!yaml_document_initialize(&output_document, NULL, NULL, NULL, 0, 0))
        goto document_error;

    /* Create the root sequence. */

    root = yaml_document_add_sequence(&output_document, NULL,
            YAML_BLOCK_SEQUENCE_STYLE);
    if (!root) goto document_error;

    /* Loop through the input events. */

    while (!done)
    {
        int properties, key, value, map, seq;

        /* Get the next event. */

        if (!yaml_parser_parse(&parser, &input_event))
            goto parser_error;

        /* Check if this is the stream end. */

        if (input_event.type == YAML_STREAM_END_EVENT) {
            done = 1;
        }

        /* Create a mapping node and attach it to the root sequence. */

        properties = yaml_document_add_mapping(&output_document, NULL,
                YAML_BLOCK_MAPPING_STYLE);
        if (!properties) goto document_error;
        if (!yaml_document_append_sequence_item(&output_document,
                    root, properties)) goto document_error;

        /* Analyze the event. */

        switch (input_event.type)
        {
            case YAML_STREAM_START_EVENT:

                /* Add 'type': 'STREAM-START'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "STREAM-START", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'encoding': <encoding>. */

                if (input_event.data.stream_start.encoding)
                {
                    yaml_encoding_t encoding
                        = input_event.data.stream_start.encoding;

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "encoding", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            (encoding == YAML_UTF8_ENCODING ? "utf-8" :
                             encoding == YAML_UTF16LE_ENCODING ? "utf-16-le" :
                             encoding == YAML_UTF16BE_ENCODING ? "utf-16-be" :
                             "unknown"), -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }
                    
                break;

            case YAML_STREAM_END_EVENT:

                /* Add 'type': 'STREAM-END'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "STREAM-END", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            case YAML_DOCUMENT_START_EVENT:

                /* Add 'type': 'DOCUMENT-START'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "DOCUMENT-START", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Display the output_document version numbers. */

                if (input_event.data.document_start.version_directive)
                {
                    yaml_version_directive_t *version
                        = input_event.data.document_start.version_directive;
                    char number[64];

                    /* Add 'version': {}. */
                    
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "version", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    map = yaml_document_add_mapping(&output_document, NULL,
                            YAML_FLOW_MAPPING_STYLE);
                    if (!map) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, map)) goto document_error;

                    /* Add 'major': <number>. */

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "major", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    sprintf(number, "%d", version->major);
                    value = yaml_document_add_scalar(&output_document, YAML_INT_TAG,
                        number, -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                map, key, value)) goto document_error;

                    /* Add 'minor': <number>. */

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "minor", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    sprintf(number, "%d", version->minor);
                    value = yaml_document_add_scalar(&output_document, YAML_INT_TAG,
                        number, -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                map, key, value)) goto document_error;
                }

                /* Display the output_document tag directives. */

                if (input_event.data.document_start.tag_directives.start
                        != input_event.data.document_start.tag_directives.end)
                {
                    yaml_tag_directive_t *tag;

                    /* Add 'tags': []. */
                    
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "tags", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    seq = yaml_document_add_sequence(&output_document, NULL,
                            YAML_BLOCK_SEQUENCE_STYLE);
                    if (!seq) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, seq)) goto document_error;

                    for (tag = input_event.data.document_start.tag_directives.start;
                            tag != input_event.data.document_start.tag_directives.end;
                            tag ++)
                    {
                        /* Add {}. */

                        map = yaml_document_add_mapping(&output_document, NULL,
                                YAML_FLOW_MAPPING_STYLE);
                        if (!map) goto document_error;
                        if (!yaml_document_append_sequence_item(&output_document,
                                    seq, map)) goto document_error;

                        /* Add 'handle': <handle>. */

                        key = yaml_document_add_scalar(&output_document, NULL,
                            "handle", -1, YAML_PLAIN_SCALAR_STYLE);
                        if (!key) goto document_error;
                        value = yaml_document_add_scalar(&output_document, NULL,
                            tag->handle, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                        if (!value) goto document_error;
                        if (!yaml_document_append_mapping_pair(&output_document,
                                    map, key, value)) goto document_error;

                        /* Add 'prefix': <prefix>. */

                        key = yaml_document_add_scalar(&output_document, NULL,
                            "prefix", -1, YAML_PLAIN_SCALAR_STYLE);
                        if (!key) goto document_error;
                        value = yaml_document_add_scalar(&output_document, NULL,
                            tag->prefix, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                        if (!value) goto document_error;
                        if (!yaml_document_append_mapping_pair(&output_document,
                                    map, key, value)) goto document_error;
                    }
                }

                /* Add 'implicit': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "implicit", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.document_start.implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            case YAML_DOCUMENT_END_EVENT:

                /* Add 'type': 'DOCUMENT-END'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "DOCUMENT-END", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'implicit': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "implicit", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.document_end.implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            case YAML_ALIAS_EVENT:

                /* Add 'type': 'ALIAS'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "ALIAS", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'anchor': <anchor>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "anchor", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                        input_event.data.alias.anchor, -1,
                        YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            case YAML_SCALAR_EVENT:

                /* Add 'type': 'SCALAR'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "SCALAR", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'anchor': <anchor>. */

                if (input_event.data.scalar.anchor)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "anchor", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.scalar.anchor, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'tag': <tag>. */

                if (input_event.data.scalar.tag)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "tag", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.scalar.tag, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'value': <value>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "value", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                        input_event.data.scalar.value,
                        input_event.data.scalar.length,
                        YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Display if the scalar tag is implicit. */

                /* Add 'implicit': {} */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "version", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                map = yaml_document_add_mapping(&output_document, NULL,
                        YAML_FLOW_MAPPING_STYLE);
                if (!map) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, map)) goto document_error;

                /* Add 'plain': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "plain", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.scalar.plain_implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            map, key, value)) goto document_error;

                /* Add 'quoted': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "quoted", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.scalar.quoted_implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            map, key, value)) goto document_error;

                /* Display the style information. */

                if (input_event.data.scalar.style)
                {
                    yaml_scalar_style_t style = input_event.data.scalar.style;

                    /* Add 'style': <style>. */

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "style", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            (style == YAML_PLAIN_SCALAR_STYLE ? "plain" :
                             style == YAML_SINGLE_QUOTED_SCALAR_STYLE ?
                                    "single-quoted" :
                             style == YAML_DOUBLE_QUOTED_SCALAR_STYLE ?
                                    "double-quoted" :
                             style == YAML_LITERAL_SCALAR_STYLE ? "literal" :
                             style == YAML_FOLDED_SCALAR_STYLE ? "folded" :
                             "unknown"), -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                break;

            case YAML_SEQUENCE_START_EVENT:

                /* Add 'type': 'SEQUENCE-START'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "SEQUENCE-START", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'anchor': <anchor>. */

                if (input_event.data.sequence_start.anchor)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "anchor", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.sequence_start.anchor, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'tag': <tag>. */

                if (input_event.data.sequence_start.tag)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "tag", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.sequence_start.tag, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'implicit': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "implicit", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.sequence_start.implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Display the style information. */

                if (input_event.data.sequence_start.style)
                {
                    yaml_sequence_style_t style
                        = input_event.data.sequence_start.style;

                    /* Add 'style': <style>. */

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "style", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            (style == YAML_BLOCK_SEQUENCE_STYLE ? "block" :
                             style == YAML_FLOW_SEQUENCE_STYLE ? "flow" :
                             "unknown"), -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                break;

            case YAML_SEQUENCE_END_EVENT:

                /* Add 'type': 'SEQUENCE-END'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "SEQUENCE-END", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            case YAML_MAPPING_START_EVENT:

                /* Add 'type': 'MAPPING-START'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "MAPPING-START", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Add 'anchor': <anchor>. */

                if (input_event.data.mapping_start.anchor)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "anchor", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.mapping_start.anchor, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'tag': <tag>. */

                if (input_event.data.mapping_start.tag)
                {
                    key = yaml_document_add_scalar(&output_document, NULL,
                        "tag", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            input_event.data.mapping_start.tag, -1,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                /* Add 'implicit': <flag>. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "implicit", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, YAML_BOOL_TAG,
                        (input_event.data.mapping_start.implicit ?
                         "true" : "false"), -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                /* Display the style information. */

                if (input_event.data.sequence_start.style)
                {
                    yaml_sequence_style_t style
                        = input_event.data.mapping_start.style;

                    /* Add 'style': <style>. */

                    key = yaml_document_add_scalar(&output_document, NULL,
                        "style", -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!key) goto document_error;
                    value = yaml_document_add_scalar(&output_document, NULL,
                            (style == YAML_BLOCK_MAPPING_STYLE ? "block" :
                             style == YAML_FLOW_MAPPING_STYLE ? "flow" :
                             "unknown"), -1, YAML_PLAIN_SCALAR_STYLE);
                    if (!value) goto document_error;
                    if (!yaml_document_append_mapping_pair(&output_document,
                                properties, key, value)) goto document_error;
                }

                break;

            case YAML_MAPPING_END_EVENT:

                /* Add 'type': 'MAPPING-END'. */

                key = yaml_document_add_scalar(&output_document, NULL,
                    "type", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!key) goto document_error;
                value = yaml_document_add_scalar(&output_document, NULL,
                    "MAPPING-END", -1, YAML_PLAIN_SCALAR_STYLE);
                if (!value) goto document_error;
                if (!yaml_document_append_mapping_pair(&output_document,
                            properties, key, value)) goto document_error;

                break;

            default:
                /* It couldn't really happen. */
                break;
        }

        /* Delete the event object. */

        yaml_event_delete(&input_event);
    }

    if (!yaml_emitter_dump(&emitter, &output_document))
        goto emitter_error;
    if (!yaml_emitter_close(&emitter))
        goto emitter_error;

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 0;

parser_error:

    /* Display a parser error message. */

    switch (parser.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", parser.problem,
                        parser.problem_value, parser.problem_offset);
            }
            else {
                fprintf(stderr, "Reader error: %s at %d\n", parser.problem,
                        parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", parser.context,
                        parser.context_mark.line+1, parser.context_mark.column+1,
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_event_delete(&input_event);
    yaml_document_delete(&output_document);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

emitter_error:

    /* Display an emitter error message. */

    switch (emitter.error)
    {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            fprintf(stderr, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            fprintf(stderr, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_event_delete(&input_event);
    yaml_document_delete(&output_document);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

document_error:

    fprintf(stderr, "Memory error: Not enough memory for creating a document\n");

    yaml_event_delete(&input_event);
    yaml_document_delete(&output_document);
    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;
}

int copy_document(yaml_document_t *document_to, yaml_document_t *document_from)
{
    yaml_node_t *node;
    yaml_node_item_t *item;
    yaml_node_pair_t *pair;

    if (!yaml_document_initialize(document_to, document_from->version_directive,
                document_from->tag_directives.start,
                document_from->tag_directives.end,
                document_from->start_implicit, document_from->end_implicit))
        return 0;

    for (node = document_from->nodes.start;
            node < document_from->nodes.top; node ++) {
        switch (node->type) {
            case YAML_SCALAR_NODE:
                if (!yaml_document_add_scalar(document_to, node->tag,
                            node->data.scalar.value, node->data.scalar.length,
                            node->data.scalar.style)) goto error;
                break;
            case YAML_SEQUENCE_NODE:
                if (!yaml_document_add_sequence(document_to, node->tag,
                            node->data.sequence.style)) goto error;
                break;
            case YAML_MAPPING_NODE:
                if (!yaml_document_add_mapping(document_to, node->tag,
                            node->data.mapping.style)) goto error;
                break;
            default:
                goto error;
        }
    }

    for (node = document_from->nodes.start;
            node < document_from->nodes.top; node ++) {
        switch (node->type) {
            case YAML_SEQUENCE_NODE:
                for (item = node->data.sequence.items.start;
                        item < node->data.sequence.items.top; item ++) {
                    if (!yaml_document_append_sequence_item(document_to,
                                node - document_from->nodes.start + 1,
                                *item)) goto error;
                }
                break;
            case YAML_MAPPING_NODE:
                for (pair = node->data.mapping.pairs.start;
                        pair < node->data.mapping.pairs.top; pair ++) {
                    if (!yaml_document_append_mapping_pair(document_to,
                                node - document_from->nodes.start + 1,
                                pair->key, pair->value)) goto error;
                }
                break;
            default:
                break;
        }
    }
    return 1;

error:
    yaml_document_delete(document_to);
    return 0;
}

int compare_nodes(yaml_document_t *document1, int index1,
        yaml_document_t *document2, int index2, int level)
{
    if (level++ > 1000) return 0;
    yaml_node_t *node1 = yaml_document_get_node(document1, index1);

    if(!node1)
      return 0;

    yaml_node_t *node2 = yaml_document_get_node(document2, index2);

    if(!node2)
      return 0;

    int k;

    if (node1->type != node2->type)
        return 0;

    if (strcmp((char *)node1->tag, (char *)node2->tag) != 0) return 0;

    switch (node1->type) {
        case YAML_SCALAR_NODE:
            if (node1->data.scalar.length != node2->data.scalar.length)
                return 0;
            if (strncmp((char *)node1->data.scalar.value, (char *)node2->data.scalar.value,
                        node1->data.scalar.length) != 0) return 0;
            break;
        case YAML_SEQUENCE_NODE:
            if ((node1->data.sequence.items.top - node1->data.sequence.items.start) !=
                    (node2->data.sequence.items.top - node2->data.sequence.items.start))
                return 0;
            for (k = 0; k < (node1->data.sequence.items.top - node1->data.sequence.items.start); k ++) {
                if (!compare_nodes(document1, node1->data.sequence.items.start[k],
                            document2, node2->data.sequence.items.start[k], level)) return 0;
            }
            break;
        case YAML_MAPPING_NODE:
            if ((node1->data.mapping.pairs.top - node1->data.mapping.pairs.start) !=
                    (node2->data.mapping.pairs.top - node2->data.mapping.pairs.start))
                return 0;
            for (k = 0; k < (node1->data.mapping.pairs.top - node1->data.mapping.pairs.start); k ++) {
                if (!compare_nodes(document1, node1->data.mapping.pairs.start[k].key,
                            document2, node2->data.mapping.pairs.start[k].key, level)) return 0;
                if (!compare_nodes(document1, node1->data.mapping.pairs.start[k].value,
                            document2, node2->data.mapping.pairs.start[k].value, level)) return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

int compare_documents(yaml_document_t *document1, yaml_document_t *document2)
{
    int k;

    if ((document1->version_directive && !document2->version_directive)
            || (!document1->version_directive && document2->version_directive)
            || (document1->version_directive && document2->version_directive
                && (document1->version_directive->major != document2->version_directive->major
                    || document1->version_directive->minor != document2->version_directive->minor)))
        return 0;

    if ((document1->tag_directives.end - document1->tag_directives.start) !=
            (document2->tag_directives.end - document2->tag_directives.start))
        return 0;
    for (k = 0; k < (document1->tag_directives.end - document1->tag_directives.start); k ++) {
        if ((strcmp((char *)document1->tag_directives.start[k].handle,
                        (char *)document2->tag_directives.start[k].handle) != 0)
                || (strcmp((char *)document1->tag_directives.start[k].prefix,
                    (char *)document2->tag_directives.start[k].prefix) != 0))
            return 0;
    }

    if ((document1->nodes.top - document1->nodes.start) !=
            (document2->nodes.top - document2->nodes.start))
        return 0;

    if (document1->nodes.top != document1->nodes.start) {
        if (!compare_nodes(document1, 1, document2, 1, 0))
            return 0;
    }

    return 1;
}

int
dumper_main(void)
{
        yaml_parser_t parser;
        yaml_emitter_t emitter;

        yaml_document_t document;
        unsigned char buffer[BUFFER_SIZE+1];
        size_t written = 0;
        yaml_document_t documents[MAX_DOCUMENTS];
        size_t document_number = 0;
        int done = 0;
        int count = 0;
        int error = 0;
        int k;
        memset(buffer, 0, BUFFER_SIZE+1);
        memset(documents, 0, MAX_DOCUMENTS*sizeof(yaml_document_t));

        if(!yaml_parser_initialize(&parser))
          return 0;

        yaml_parser_set_input_string(&parser, curpos, remainder);
        if(!yaml_emitter_initialize(&emitter))
          return 0;

        if (canonical) {
            yaml_emitter_set_canonical(&emitter, 1);
        }
        if (unicode) {
            yaml_emitter_set_unicode(&emitter, 1);
        }
        yaml_emitter_set_output_string(&emitter, buffer, BUFFER_SIZE, &written);
        yaml_emitter_open(&emitter);

        while (!done)
        {
            if (!yaml_parser_load(&parser, &document)) {
                error = 1;
                break;
            }

            done = (!yaml_document_get_root_node(&document));
            if (!done) {
                if(document_number >= MAX_DOCUMENTS) {
                  yaml_document_delete(&document);
                  error = 1;
                  break;
                }

                if(!copy_document(&(documents[document_number++]), &document)) {
                  yaml_document_delete(&document);
                  error = 1;
                  break;
                }
                if(!(yaml_emitter_dump(&emitter, &document) || (yaml_emitter_flush(&emitter) && 0))) {
                  error = 1;
                  break;
                }

                count ++;
            }
            else {
                yaml_document_delete(&document);
            }
        }

        yaml_parser_delete(&parser);
        yaml_emitter_close(&emitter);
        yaml_emitter_delete(&emitter);

        if (!error)
        {
            count = done = 0;
            if(!yaml_parser_initialize(&parser))
              return 0;

            yaml_parser_set_input_string(&parser, buffer, written);

            while (!done)
            {
                if(!(yaml_parser_load(&parser, &document) || 0)) {
                  yaml_parser_delete(&parser);
                  return 0;
                }

                done = (!yaml_document_get_root_node(&document));
                if (!done) {
                    if(!(compare_documents(documents+count, &document) || 0)) {
                      yaml_parser_delete(&parser);
                      return 0;
                    }
                    count ++;
                }
                yaml_document_delete(&document);
            }
            yaml_parser_delete(&parser);
        }

        for (k = 0; k < document_number; k ++) {
            yaml_document_delete(documents+k);
        }

        printf("PASSED (length: %d)\n", written);

    return 0;
}

int copy_event(yaml_event_t *event_to, yaml_event_t *event_from)
{
    switch (event_from->type)
    {
        case YAML_STREAM_START_EVENT:
            return yaml_stream_start_event_initialize(event_to,
                    event_from->data.stream_start.encoding);

        case YAML_STREAM_END_EVENT:
            return yaml_stream_end_event_initialize(event_to);

        case YAML_DOCUMENT_START_EVENT:
            return yaml_document_start_event_initialize(event_to,
                    event_from->data.document_start.version_directive,
                    event_from->data.document_start.tag_directives.start,
                    event_from->data.document_start.tag_directives.end,
                    event_from->data.document_start.implicit);

        case YAML_DOCUMENT_END_EVENT:
            return yaml_document_end_event_initialize(event_to,
                    event_from->data.document_end.implicit);

        case YAML_ALIAS_EVENT:
            return yaml_alias_event_initialize(event_to,
                    event_from->data.alias.anchor);

        case YAML_SCALAR_EVENT:
            return yaml_scalar_event_initialize(event_to,
                    event_from->data.scalar.anchor,
                    event_from->data.scalar.tag,
                    event_from->data.scalar.value,
                    event_from->data.scalar.length,
                    event_from->data.scalar.plain_implicit,
                    event_from->data.scalar.quoted_implicit,
                    event_from->data.scalar.style);

        case YAML_SEQUENCE_START_EVENT:
            return yaml_sequence_start_event_initialize(event_to,
                    event_from->data.sequence_start.anchor,
                    event_from->data.sequence_start.tag,
                    event_from->data.sequence_start.implicit,
                    event_from->data.sequence_start.style);

        case YAML_SEQUENCE_END_EVENT:
            return yaml_sequence_end_event_initialize(event_to);

        case YAML_MAPPING_START_EVENT:
            return yaml_mapping_start_event_initialize(event_to,
                    event_from->data.mapping_start.anchor,
                    event_from->data.mapping_start.tag,
                    event_from->data.mapping_start.implicit,
                    event_from->data.mapping_start.style);

        case YAML_MAPPING_END_EVENT:
            return yaml_mapping_end_event_initialize(event_to);
    }

    return 0;
}

int compare_events(yaml_event_t *event1, yaml_event_t *event2)
{
    int k;

    if (event1->type != event2->type)
        return 0;

    switch (event1->type)
    {
        case YAML_STREAM_START_EVENT:
            return 1;
            /* return (event1->data.stream_start.encoding ==
                    event2->data.stream_start.encoding); */

        case YAML_DOCUMENT_START_EVENT:
            if ((event1->data.document_start.version_directive && !event2->data.document_start.version_directive)
                    || (!event1->data.document_start.version_directive && event2->data.document_start.version_directive)
                    || (event1->data.document_start.version_directive && event2->data.document_start.version_directive
                        && (event1->data.document_start.version_directive->major != event2->data.document_start.version_directive->major
                            || event1->data.document_start.version_directive->minor != event2->data.document_start.version_directive->minor)))
                return 0;
            if ((event1->data.document_start.tag_directives.end - event1->data.document_start.tag_directives.start) !=
                    (event2->data.document_start.tag_directives.end - event2->data.document_start.tag_directives.start))
                return 0;
            for (k = 0; k < (event1->data.document_start.tag_directives.end - event1->data.document_start.tag_directives.start); k ++) {
                if ((strcmp((char *)event1->data.document_start.tag_directives.start[k].handle,
                                (char *)event2->data.document_start.tag_directives.start[k].handle) != 0)
                        || (strcmp((char *)event1->data.document_start.tag_directives.start[k].prefix,
                            (char *)event2->data.document_start.tag_directives.start[k].prefix) != 0))
                    return 0;
            }
            /* if (event1->data.document_start.implicit != event2->data.document_start.implicit)
                return 0; */
            return 1;

        case YAML_DOCUMENT_END_EVENT:
            return 1;
            /* return (event1->data.document_end.implicit ==
                    event2->data.document_end.implicit); */

        case YAML_ALIAS_EVENT:
            return (strcmp((char *)event1->data.alias.anchor,
                        (char *)event2->data.alias.anchor) == 0);

        case YAML_SCALAR_EVENT:
            if ((event1->data.scalar.anchor && !event2->data.scalar.anchor)
                    || (!event1->data.scalar.anchor && event2->data.scalar.anchor)
                    || (event1->data.scalar.anchor && event2->data.scalar.anchor
                        && strcmp((char *)event1->data.scalar.anchor,
                            (char *)event2->data.scalar.anchor) != 0))
                return 0;
            if ((event1->data.scalar.tag && !event2->data.scalar.tag
                        && strcmp((char *)event1->data.scalar.tag, "!") != 0)
                    || (!event1->data.scalar.tag && event2->data.scalar.tag
                        && strcmp((char *)event2->data.scalar.tag, "!") != 0)
                    || (event1->data.scalar.tag && event2->data.scalar.tag
                        && strcmp((char *)event1->data.scalar.tag,
                            (char *)event2->data.scalar.tag) != 0))
                return 0;
            if ((event1->data.scalar.length != event2->data.scalar.length)
                    || memcmp(event1->data.scalar.value, event2->data.scalar.value,
                        event1->data.scalar.length) != 0)
                return 0;
            if ((event1->data.scalar.plain_implicit != event2->data.scalar.plain_implicit)
                    || (event2->data.scalar.quoted_implicit != event2->data.scalar.quoted_implicit)
                    /* || (event2->data.scalar.style != event2->data.scalar.style) */)
                return 0;
            return 1;

        case YAML_SEQUENCE_START_EVENT:
            if ((event1->data.sequence_start.anchor && !event2->data.sequence_start.anchor)
                    || (!event1->data.sequence_start.anchor && event2->data.sequence_start.anchor)
                    || (event1->data.sequence_start.anchor && event2->data.sequence_start.anchor
                        && strcmp((char *)event1->data.sequence_start.anchor,
                            (char *)event2->data.sequence_start.anchor) != 0))
                return 0;
            if ((event1->data.sequence_start.tag && !event2->data.sequence_start.tag)
                    || (!event1->data.sequence_start.tag && event2->data.sequence_start.tag)
                    || (event1->data.sequence_start.tag && event2->data.sequence_start.tag
                        && strcmp((char *)event1->data.sequence_start.tag,
                            (char *)event2->data.sequence_start.tag) != 0))
                return 0;
            if ((event1->data.sequence_start.implicit != event2->data.sequence_start.implicit)
                    /* || (event2->data.sequence_start.style != event2->data.sequence_start.style) */)
                return 0;
            return 1;

        case YAML_MAPPING_START_EVENT:
            if ((event1->data.mapping_start.anchor && !event2->data.mapping_start.anchor)
                    || (!event1->data.mapping_start.anchor && event2->data.mapping_start.anchor)
                    || (event1->data.mapping_start.anchor && event2->data.mapping_start.anchor
                        && strcmp((char *)event1->data.mapping_start.anchor,
                            (char *)event2->data.mapping_start.anchor) != 0))
                return 0;
            if ((event1->data.mapping_start.tag && !event2->data.mapping_start.tag)
                    || (!event1->data.mapping_start.tag && event2->data.mapping_start.tag)
                    || (event1->data.mapping_start.tag && event2->data.mapping_start.tag
                        && strcmp((char *)event1->data.mapping_start.tag,
                            (char *)event2->data.mapping_start.tag) != 0))
                return 0;
            if ((event1->data.mapping_start.implicit != event2->data.mapping_start.implicit)
                    /* || (event2->data.mapping_start.style != event2->data.mapping_start.style) */)
                return 0;
            return 1;

        default:
            return 1;
    }
}

int
emitter_main(void)
{
        yaml_parser_t parser;
        yaml_emitter_t emitter;
        yaml_event_t event;
        unsigned char buffer[BUFFER_SIZE+1];
        size_t written = 0;
        yaml_event_t events[MAX_EVENTS];
        size_t event_number = 0;
        int done = 0;
        int count = 0;
        int error = 0;
        int k;
        memset(buffer, 0, BUFFER_SIZE+1);
        memset(events, 0, MAX_EVENTS*sizeof(yaml_event_t));

        if(!yaml_parser_initialize(&parser))
          return 0;

        yaml_parser_set_input_string(&parser, curpos, remainder);
        if(!yaml_emitter_initialize(&emitter)) {
          yaml_parser_delete(&parser);
          return 0;
        }

        if (canonical) {
            yaml_emitter_set_canonical(&emitter, 1);
        }
        if (unicode) {
            yaml_emitter_set_unicode(&emitter, 1);
        }
        yaml_emitter_set_output_string(&emitter, buffer, BUFFER_SIZE, &written);

        while (!done)
        {
            if (!yaml_parser_parse(&parser, &event)) {
                error = 1;
                break;
            }

            done = (event.type == YAML_STREAM_END_EVENT);
            if(!(event_number < MAX_EVENTS)) {
              error = 1;
              break;
            }

            if(!(copy_event(&(events[event_number++]), &event))) {
              error = 1;
              break;
            }

            if(!(yaml_emitter_emit(&emitter, &event))) {
              error = 1;
              break;
            }

            count ++;
        }

        yaml_parser_delete(&parser);
        yaml_emitter_delete(&emitter);

        if (!error)
        {
            count = done = 0;
            if(!yaml_parser_initialize(&parser))
              return 0;

            yaml_parser_set_input_string(&parser, buffer, written);

            while (!done)
            {
                if(!yaml_parser_parse(&parser, &event))
                  break;

                done = (event.type == YAML_STREAM_END_EVENT);
                if(!compare_events(events+count, &event)) {
                  yaml_event_delete(&event);
                  break;
                }

                yaml_event_delete(&event);
                count ++;
            }
            yaml_parser_delete(&parser);
        }

        for (k = 0; k < event_number; k ++) {
            yaml_event_delete(events+k);
        }

        printf("PASSED (length: %d)\n", written);

    return 0;
}

int
loader_main(void)
{
        yaml_parser_t parser;
        yaml_document_t document;
        int done = 0;
        int count = 0;
        int error = 0;

        if(!yaml_parser_initialize(&parser))
          return 0;

        yaml_parser_set_input_string(&parser, curpos, remainder);

        while (!done)
        {
            if (!yaml_parser_load(&parser, &document)) {
                error = 1;
                break;
            }

            done = (!yaml_document_get_root_node(&document));

            yaml_document_delete(&document);

            if (!done) count ++;
        }

        yaml_parser_delete(&parser);

        printf("%s (%d documents)\n", (error ? "FAILURE" : "SUCCESS"), count);

    return 0;
}

int
parser_main(void)
{
        yaml_parser_t parser;
        yaml_event_t event;
        int done = 0;
        int count = 0;
        int error = 0;

        if(!yaml_parser_initialize(&parser))
          return 0;

        yaml_parser_set_input_string(&parser, curpos, remainder);

        while (!done)
        {
            if (!yaml_parser_parse(&parser, &event)) {
                error = 1;
                break;
            }

            done = (event.type == YAML_STREAM_END_EVENT);

            yaml_event_delete(&event);

            count ++;
        }

        yaml_parser_delete(&parser);

        printf("%s (%d events)\n", (error ? "FAILURE" : "SUCCESS"), count);

    return 0;
}

int
scanner_main(void)
{
        yaml_parser_t parser;
        yaml_token_t token;
        int done = 0;
        int count = 0;
        int error = 0;

        if(!yaml_parser_initialize(&parser))
          return 0;

        yaml_parser_set_input_string(&parser, curpos, remainder);

        while (!done)
        {
            if (!yaml_parser_scan(&parser, &token)) {
                error = 1;
                break;
            }

            done = (token.type == YAML_STREAM_END_TOKEN);

            yaml_token_delete(&token);

            count ++;
        }

        yaml_parser_delete(&parser);

        printf("%s (%d tokens)\n", (error ? "FAILURE" : "SUCCESS"), count);

    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 3) return 0;

  canonical = data[0] & 1;
  unicode = data[1] & 1;
  curpos = data + 3;
  remainder = size - 3;

  switch(data[2] % 9) {
    case 0: reformatter_main(); break;
    case 1: reformatter_alt_main(); break;
    case 2: deconstructor_main(); break;
    case 3: deconstructor_alt_main(); break;
    case 4: dumper_main(); break;
    case 5: emitter_main(); break;
    case 6: loader_main(); break;
    case 7: parser_main(); break;
    case 8: scanner_main(); break;
    default: return 0;
  }
  return 0;
}
